# $Id: Sieve.pm,v 0.4.9b 2001/06/15 19:25:00 alain Exp $

package IMAP::Sieve;

use strict;
use Carp;
use IO::Select;
use IO::Socket;
use IO::Socket::INET;
#use Text::ParseWords qw(parse_line);
use Cwd;

use vars qw($VERSION);

$VERSION = '0.4.9b';

sub new {
    my $class = shift;
    my $self = {};
    bless $self, $class;
    if ((scalar(@_) % 2) != 0) {
	croak "$class called with incorrect number of arguments";
    }
    while (@_) {
	my $key = shift(@_);
	my $value = shift(@_);
	$self->{$key} = $value;
    }
    $self->{'CLASS'} = $class;
    $self->_initialize;
    return $self;
}

sub _initialize {
    my $self = shift;
    my ($len,$userpass,$encode);
    if (!defined($self->{'Server'})) {
	croak "$self->{'CLASS'} not initialized properly : Server parameter missing";
    }
    if (!defined($self->{'Port'})) {
	$self->{'Port'} = 2000; # default sieve port;
    }
    if (!defined($self->{'Login'})) {
	croak "$self->{'CLASS'} not initialized properly : Login parameter missing";
    }
    if (!defined($self->{'Password'})) {
	croak "$self->{'CLASS'} not initialized properly : Password parameter missing";
    }
    if (!defined($self->{'Proxy'})) {
	$self->{'Proxy'} = ''; # Proxy;
    }
    if (defined($self->{'SSL'})) {
	my $cwd= cwd;
	my %ssl_defaults = (
			  'SSL_use_cert' => 0,
			  'SSL_verify_mode' => 0x00,
			  'SSL_key_file' => $cwd."/certs/client-key.pem",
			  'SSL_cert_file' => $cwd."/certs/client-cert.pem",
			  'SSL_ca_path' => $cwd."/certs",
			  'SSL_ca_file' => $cwd."/certs/ca-cert.pem",
			  );
	my @ssl_options;
	my $ssl_key;
	my $key;
	foreach $ssl_key (keys(%ssl_defaults)) {
		if (!defined($self->{$ssl_key})) {
			$self->{$ssl_key} = $ssl_defaults{$ssl_key};
		}
	}
	foreach $ssl_key (keys(%{$self})) {
		if ($ssl_key =~ /^SSL_/) {
			push @ssl_options, $ssl_key,$self->{$ssl_key};
		}
	}
        my $SSL_try="use IO::Socket::SSL";
	eval $SSL_try;
	if (!eval {$self->{'Socket'} =
		IO::Socket::SSL->new(PeerAddr => $self->{'Server'},
				     PeerPort => $self->{'Port'},
				     Proto => 'tcp',
				     Reuse => 1,
				     Timeout => 5,
				     @ssl_options);}) {
		$self->_error("initialize", "couldn't establish a sieve SSL connection to",$self->{'Server'}, "[$!]","path=$cwd");
 		delete $self->{'Socket'};
		return;
	}
     }
     else {

    	if (!eval {$self->{'Socket'} = IO::Socket::INET->new(PeerAddr => $self->{'Server'},
							 PeerPort => $self->{'Port'},
							 Proto => 'tcp',
							 Reuse => 1); })
    	{
		$self->_error("initialize", "could'nt establish a Sieve connection to",$self->{'Server'});				
		return;
    	}
    } # if SSL

    my $fh = $self->{'Socket'};
     $_ = $self->_read; #get banner
    my $try=$_;
    if (!/timsieved/i) {
	$self->close;
	$self->_error("initialize","bad response from",$self->{'Server'},$try);
	return;
    }
    chomp;
    if (/\r$/) {
	chop;
    }
    if (/IMPLEMENTATION/) {
	$self->{'Implementation'}=$1 if /^"IMPLEMENTATION" +"(.*)"/;
    	#version 2 of cyrus imap/timsieved
	# get capability
	# get OK as well
	$_=$self->_read;
        while (!/^OK/) {
	   $self->{'Capability'}=$1 if /^"SASL" +"(.*)"/;
	   $self->{'Sieve'}=$1 if /^"SIEVE" +"(.*)"/;
	   $_ = $self->_read;
##	   $_=$self->_read;
	}
    }
    else {
	$self->{'Capability'}=$_;
    }
    $userpass = "$self->{'Proxy'}\x00".$self->{'Login'}."\x00".$self->{'Password'};
    $encode=encode_base64($userpass);
    $len=length($encode);
    print $fh "AUTHENTICATE \"PLAIN\" {$len+}\r\n";
 
    print $fh "$encode\r\n";
    
    $_ = $self->_read;
    $try=$_;
    if ($try=~/NO/) {
	$self->close;
	$self->_error("Login incorrect while connecting to $self->{'Server'}", $try);
	return;
    } elsif (/OK/) {
    	$self->{'Error'}= "No Errors";
	return;
    } else {
	#croak "$self->{'CLASS'}: Unknown error -- $_";
	$self->_error("Unknown error",$try);
	return;
    }
    $self->{'Error'}="No Errors";
    return;
}
sub encode_base64 ($;$)
{
    my $res = "";
    my $eol = $_[1];
    $eol = "\n" unless defined $eol;
    pos($_[0]) = 0;                          # ensure start at the beginning
    while ($_[0] =~ /(.{1,45})/gs) {
	$res .= substr(pack('u', $1), 1);
	chop($res);
    }
    $res =~ tr|` -_|AA-Za-z0-9+/|;               # `# help emacs
    # fix padding at the end
    my $padding = (3 - length($_[0]) % 3) % 3;
    $res =~ s/.{$padding}$/'=' x $padding/e if $padding;
    # break encoded string into lines of no more than 76 characters each
    if (length $eol) {
	$res =~ s/(.{1,76})/$1$eol/g;
    }
    $res;
}


sub _error {
    my $self = shift;
    my $func = shift;
    my @error = @_;

    $self->{'Error'} = join(" ",$self->{'CLASS'}, "[", $func, "]:", @error);
}

sub _read {
	my $self = shift;
	my $buffer ="";
	my $char = "";
	my $bytes= 1;
	while ($bytes == 1) {
		$bytes = sysread $self->{'Socket'},$char,1;
		if ($bytes == 0) {
			if (length ($buffer) != 0) {
				return $buffer;
			}
			else {
				return;
			}
		}
		else {
			if (($char eq "\n") or ($char eq "\r")) {
				if (length($buffer) ==0) {
					# remove any cr or nl leftover
				}
				else {
					return $buffer;
				}
			}
			else {
				$buffer.=$char;
			}
		}
	}
}
				
				
sub close {
    my $self = shift;
     if (!defined($self->{'Socket'})) {
     	return 0;
     }
     my $fh =$self->{'Socket'};
    print $fh "LOGOUT\r\n";
    close($self->{'Socket'});
    delete $self->{'Socket'};
}

sub putscript {
    my $self = shift;
    my $len;

    if (scalar(@_) != 2)  {
	$self->_error("putscript", "incorrect number of arguments");
	return 1;
    }

    my $scriptname = shift;
    my $script = shift;

    if (!defined($self->{'Socket'})) {
	$self->_error("putscript", "no connection open to", $self->{'Server'});
	return 1;
    }
    $len=length($script);
    my $fh = $self->{'Socket'};
    print $fh "PUTSCRIPT \"$scriptname\" {$len+}\r\n";
    print $fh "$script\r\n";
    $_ = $self->_read;
    if (/^OK/) {
	$self->{'Error'} = 'No Errors';
	return 0;
    } else {
	$self->_error("putscript", "couldn't save script", $scriptname, ":", $_);
	return 1;
    }
}

sub deletescript {
    my $self = shift;

    if (scalar(@_) != 1) {
	$self->_error("deletescript", "incorrect number of arguments");
	return 1;
    }
    my $script = shift;
    if (!defined($self->{'Socket'})) {
	$self->_error("deletescript", "no connection open to", $self->{'Server'});
	return 1;
    }
    my $fh = $self->{'Socket'};
    print $fh "DELETESCRIPT \"$script\"\r\n";
    $_ = $self->_read;
    if (/^OK/) {
	$self->{'Error'} = 'No Errors';
	return 0;
    } else {
	$self->_error("deletescript", "couldn't delete", $script, ":", $_);
	return 1;
    }
}
sub getscript { # returns a string
    my $self = shift;
    my $allscript;

    if (scalar(@_) != 1) {
	$self->_error("getscript", "incorrect number of arguments");
	return 1;
    }
    my $script = shift;
    if (!defined($self->{'Socket'})) {
	$self->_error("getscript", "no connection open to", $self->{'Server'});
	return 1;
    }
    my $fh = $self->{'Socket'};
    print $fh "GETSCRIPT \"$script\"\r\n";
    $_ = $self->_read;
    if (/^{.*}/) { $_ = $self->_read;  } # remove file size line

    # should probably use the file size to calculate how much to read in
    while ((!/^OK/) && (!/^NO/)) {
	$_.="\n" if $_ !~/\n.*$/; # replace newline that _read removes
	$allscript.=$_;	
	$_ = $self->_read;
    }
    if (/^OK/) {
	return $allscript;
    } else {
	$self->_error("getscript", "couldn't get script", $script, ":", $_);
	return;
    }
}

sub setactive {
    my $self = shift;

    if (scalar(@_) != 1) {
	$self->_error("setactive", "incorrect number of arguments");
	return 1;
    }
    my $script = shift;
    if (!defined($self->{'Socket'})) {
	$self->_error("setactive", "no connection open to", $self->{'Server'});
	return 1;
    }
    my $fh = $self->{'Socket'};
    print $fh "SETACTIVE \"$script\"\r\n";
    $_ = $self->_read;
    if (/^OK/) {
	$self->{'Error'} = "No Errors";
	return 0;
    } else {
	$self->_error("setactive", "couldn't set as active", $script, ":", $_);
	return 1;
    }
}


sub noop {
    my $self = shift;
    my ($id, $acl);

    if (!defined($self->{'Socket'})) {
	$self->_error("noop", "no connection open to", $self->{'Server'});
	return 1;
    }
    my $fh = $self->{'Socket'};
    print $fh "NOOP\r\n";
	$_ = $self->_read;
	if (!/^OK/) {
	    $self->_error("noop", "couldn't do noop"
			 );
	    return 1;
	}
    $self->{'Error'} = 'No Errors';
    return 0;
}


sub listscripts {
    my $self = shift;
    my (@scripts);

    if (!defined($self->{'Socket'})) {
	$self->_error("listscripts", "no connection open to", $self->{'Server'});
	return;
    }

    #send the command
    $self->{'Socket'}->print ("LISTSCRIPTS\r\n");

    # While we have more to read
    while (defined ($_ = $self->_read)) {

       		# Exit the loop if we're at the end of the text
        	last if (m/^OK.*/);

       		# Select the stuff between the quotes (without the asterisk)
      		# m/^"([^"]+?)\*?"\r?$/;
      		# Select including the asterisk (to determine the default script)
#		m/^"([^"]+?\*?)"\r?$/;
		$_=~s/"//g;
       		# Get the name of the script
       		push @scripts, $_;
     } 

     if (/^OK/) {
        return @scripts;
     } else {



    }
    if (/^OK/) {
	return @scripts;
    } else {
	$self->_error("list", "couldn't get list for",  ":", $_);
	return;
    }
}

1;
__END__

