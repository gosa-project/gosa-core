#!/usr/bin/perl -w -I/usr/local/lib/perl
#
# This code is part of GOsa (https://gosa.gonicus.de)
# Copyright (C) 2007 Frank Moeller
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

use strict;
use IMAP::Sieve;
use XML::Simple;
use Data::Dumper;
use Net::LDAP;
use URI;
use utf8;
use Getopt::Std;
use Date::Format;
use vars qw/ %opt /;

#
# Definitions
#
my $gosa_config = "/etc/gosa/gosa.conf";
my $opt_string = 'l:hs';
my $location = "";
my $today_gmt = time ();
my $today = $today_gmt + 3600;
my $server_attribute = "";
my $alternate_address_attribute = "";
my $gosa_sieve_script_name = "gosa";
my $simple_bind_dn = "";
my $simple_bind_dn_pwd = "";
my $gosa_sieve_script_status = "FALSE";
my $gosa_sieve_spam_header = "Sort mails with higher spam level";
my ($ss,$mm,$hh,$day,$month,$year,$zone);

#
# Templates
#
my $gosa_sieve_header = "\#\#\#GOSA\nrequire\ \[\"fileinto\",\ \"reject\",\ \"vacation\"\]\;\n\n";
my $vacation_header_template = "\# Begin vacation message";
my $vacation_footer_template = "\# End vacation message";

#
# Placeholder
#
my $start_date_ph = "##STARTDATE##";
my $stop_date_ph = "##STOPDATE##";

#
# Usage
#
sub usage {
	die "Usage:\nperl $0 [option]\n
	     \twithout any option $0 uses the default location\n
	     \tOptions:
	     \t\t-l <\"location name\">\tuse special location
	     \t\t-s\t\t\tshow all locations
	     \t\t-h\t\t\tthis help \n";
}

#
# Config import
#
sub read_config {
	my $input = shift || die "need config file: $!";
	my $stream = "";
	open ( FILE, "< $input" ) or die "Error opening file $input: $! \n";
	{
	        local $/ = undef;
	        $stream = <FILE>;
	}
	close ( FILE );
	return $stream;
}

#
# XML parser
#
sub parseconfig {
	my $c_location = shift;
	my $xmldata = shift;
	chomp $c_location;
	chomp $xmldata;
	my $data = $xmldata;
	my $xml = new XML::Simple ();
	my $c_data = $xml -> XMLin( $xmldata);
	my $config = {};
	my $config_base;
	my $ldap_admin;
	my $ldap_admin_pwd;
	my $url;
	my $mailMethod;
	#print Dumper ($c_data->{main}->{location}->{config});
	if ( $c_data->{main}->{location}->{config} ) {
		#print "IF\n";
		$config_base = $c_data->{main}->{location}->{config};
		$url = $c_data->{main}->{location}->{referral}->{url};
		$ldap_admin = $c_data->{main}->{location}->{referral}->{admin};
		$ldap_admin_pwd = $c_data->{main}->{location}->{referral}->{password};
		$mailMethod = $c_data->{main}->{location}->{mailMethod};
	} else {
		#print "ELSE\n";
		$config_base = $c_data->{main}->{location}->{$c_location}->{config};
		$url = $c_data->{main}->{location}->{$c_location}->{referral}->{url};
		$ldap_admin = $c_data->{main}->{location}->{$c_location}->{referral}->{admin};
		$ldap_admin_pwd = $c_data->{main}->{location}->{$c_location}->{referral}->{password};
		$mailMethod = $c_data->{main}->{location}->{$c_location}->{mailMethod};
	}
	print "$config_base -- $url -- $ldap_admin -- $ldap_admin_pwd -- $mailMethod\n";
	$config->{config_base} = $config_base;
	$config->{url} = $url;
	$config->{mailMethod} = $mailMethod;
	$config->{ldap_admin} = $ldap_admin;
	$config->{ldap_admin_pwd} = $ldap_admin_pwd;

	return $config;
}

#
# Get default location
#
sub get_default_location {
	my $xmldata = shift;
	my $xml = new XML::Simple ( RootName=>'conf' );
	my $c_data = $xml -> XMLin( $xmldata );
	my $default = $c_data->{main}->{default};

	return $default;
}

#
# List all location
#
sub list_locations {
	my $xmldata = shift;
	my $xml = new XML::Simple ( RootName=>'conf' );
	my $c_data = $xml -> XMLin( $xmldata );
	my $default = get_default_location ( $xmldata );
	$default = $default . " (default)";
	my @locations = ( $default );
	my $data_ref = $c_data->{main}->{location};
	my @keys = keys ( %{$data_ref} );
	@locations = (@locations, @keys);

	return @locations;
}

#
# LDAP error handling
#
sub ldap_error {
	my ($from, $mesg) = @_;
	print "Return code: ", $mesg->code;
	print "\tMessage: ", $mesg->error_name;
	print " :",          $mesg->error_text;
	print "MessageID: ", $mesg->mesg_id;
	print "\tDN: ", $mesg->dn;
}


#
# LDAP search
#
sub ldap_search {
	my $url = shift;
	my $searchString = shift;
	my $scope = shift;
	my $base = shift;
	my $attrs = shift;
	my $bind_dn = shift;
	my $bind_dn_pwd = shift;
	
	if ( $base eq "NULL" ) {
		$base = "";
	}
	my $ldap = Net::LDAP->new( $url ) or die "$@";
	if ( ( ! ( $bind_dn ) ) || ( ! ( $bind_dn_pwd ) ) ) {
		$ldap->bind;
	} else {
		$ldap->bind ( $bind_dn, password => $bind_dn_pwd );
	}

	my $result = $ldap->search (    base    => "$base",
					scope   => "$scope",
					filter  => "$searchString",
					attrs   =>  $attrs
					);
	if ( $result->code ) {
		ldap_error ( "Searching", $result );
	}

	$ldap->unbind;
	
	return $result;
}

#
# Retrieve LDAP server
#
sub get_ldap_server {
	my $url = shift;
	
	my $uri = URI->new($url);

	my $scheme = $uri->scheme;
	my $host = $uri->host;
	my $port = $uri->port;
	#print "$scheme - $host - $port\n";
	my $server = $scheme . "://" . $host . ":" . $port;

	return $server;
}

#
# Retrieve LDAP base
#
sub get_ldap_base {
	my $url = shift;
	my $config_base = shift;
	my $bind_dn = shift;
	my $bind_dn_pwd = shift;
	my $filter = "(objectClass=*)";
	my $init_base = "NULL";
	my $scope = "base";
	my $attributes = [ 'namingcontexts' ];
	my $entry = {};
	my $base = "";

	$config_base =~ s/\,\ +/\,/g;
	#print $url."\n";
	#print $config_base."\n";
	my $result = ldap_search ( $url, $filter, $scope, $init_base, $attributes, $bind_dn, $bind_dn_pwd );
	my @entries = $result->entries;
	my $noe = @entries;
	#print $noe."\n";
	foreach $entry ( @entries ) {
		my $tmp = $entry->get_value ( 'namingcontexts' );
		#print $tmp."\n";
		$tmp =~ s/\,\ +/\,/g;
		if ( $config_base =~ m/$tmp/ ) {
			$base = $entry->get_value ( 'namingcontexts' );
		}
	}

	return $base;
}

#
# SIEVE functions
#
sub opensieve {
	my $admin = shift;
	my $pass = shift;
	my $user = shift;
	my $server = shift;
	my $port = shift;

	#print ( "##### Proxy => $user, Server => $server, Login => $admin, Password => $pass, Port => $port ####\n" );

	my $sieve = IMAP::Sieve->new ( 'Proxy' => $user, 'Server' => $server, 'Login' => $admin, 'Password' => $pass, 'Port' => $port );
	return $sieve;
}

sub closesieve {
	my $sieve = shift;

	if ($sieve) {$sieve->close};
}

sub listscripts {
	my $sieve = shift;

	my @scripts = $sieve->listscripts;
	my $script_list = join("\n",@scripts)."\n";
	#print $script_list;
	return $script_list;
}

sub getscript {
	my $sieve = shift;
	my $script = shift;
	my $scriptfile;
	chomp $script;
	#print "$sieve\n";
	#print "$script\n";

	$scriptfile = $sieve->getscript($script);
	return $scriptfile;
}

sub putscript {
	my $sieve = shift;
	my $scriptname = shift;
	my $script = shift;
	#print "$sieve\n";
	#print "$scriptname\n";
	#print "$script\n";

	my $res=$sieve->putscript($scriptname,$script);
	if ($res) {print $sieve->{'Error'}}
	return;
}

sub setactive {
	my $sieve = shift;
	my $script = shift;

	my $res=$sieve->setactive($script);
	if ($res) { print $sieve->{'Error'};}
	return;
}

#
# main ()
#
# read options
getopts( "$opt_string", \%opt );

# read GOsa config
my $input_stream = read_config ( $gosa_config );

# get location
if ( $opt{l} ) {
	$location = $opt{l};
} elsif ( $opt{h} ) {
	usage ();
	exit (0);
} elsif ( $opt{s} ) {
	my $loc;
	my $counter = 1;
	my @locations = list_locations ( $input_stream );
	print "\nConfigured Locations: \n";
	print "---------------------\n";
	foreach $loc ( @locations ) {
		print $counter . ". " . $loc . "\n";
		$counter++;
	}
	print "\n\n";
	exit (0);
} else {
	$location = get_default_location ( $input_stream );
}

# parse config
my $config = parseconfig ( $location, $input_stream );
my $ldap_url = get_ldap_server ( $config->{url} );
my $gosa_config_base = $config->{config_base};
my $bind_dn = $config->{ldap_admin};
my $bind_dn_pwd = $config->{ldap_admin_pwd};
my $mailMethod = $config->{mailMethod};
utf8::encode($ldap_url);
utf8::encode($gosa_config_base);
utf8::encode($mailMethod);

# default mailMethod = kolab
if ( $mailMethod =~ m/kolab/i ) {
	$server_attribute = "kolabHomeServer";
	$alternate_address_attribute = "alias";
} elsif ( $mailMethod =~ m/cyrus/i ) {
	$server_attribute = "gosaMailServer";
	$alternate_address_attribute = "gosaMailAlternateAddress";
} else {
	exit (0);
}

# determine LDAP base
my $ldap_base = get_ldap_base ( $ldap_url, $gosa_config_base, $simple_bind_dn, $simple_bind_dn_pwd );

# retrieve user informations with activated vacation feature
my $filter = "(&(objectClass=gosaMailAccount)(gosaMailDeliveryMode=*V*)(!(gosaMailDeliveryMode=*C*)))";
my $list_of_attributes = [ 'uid', 'mail', $alternate_address_attribute, 'gosaVacationMessage', 'gosaVacationStart', 'gosaVacationStop', $server_attribute ];
my $search_scope = "sub";
my $result = ldap_search ( $ldap_url, $filter, $search_scope, $ldap_base, $list_of_attributes, $simple_bind_dn, $simple_bind_dn_pwd );

my @entries = $result->entries;
my $noe = @entries;
#print "NOE = $noe\n";
my $entry = {};
foreach $entry ( @entries ) {
	# INITIALISATIONS
	$gosa_sieve_script_status = "FALSE";
	my @sieve_scripts = "";
	my $script_name = "";
	my $sieve_script = "";
	my $sieve_vacation = "";
	# END INITIALISATIONS
	my $uid_v = $entry->get_value ( 'uid' );
	#print "$uid_v\n";
	my $mail_v = $entry->get_value ( 'mail' );
	my @mailalternate = $entry->get_value ( $alternate_address_attribute );
	my $vacation = $entry->get_value ( 'gosaVacationMessage' );
	my $start_v = $entry->get_value ( 'gosaVacationStart' );
	my $stop_v = $entry->get_value ( 'gosaVacationStop' );
	my $server_v = $entry->get_value ( $server_attribute );

	# temp. hack to compensate old gosa server name style
	#if ( $server_v =~ m/^imap\:\/\//i ) {
	#	$server_v =~ s/^imap\:\/\///;
	#}
	if ( ! ( $uid_v ) ) {
		$uid_v = "";
	}
	if ( ! ( $mail_v ) ) {
		$mail_v = "";
	}
	my @mailAddress = ($mail_v);
	my $alias = "";
	foreach $alias ( @mailalternate ) {
		push @mailAddress, $alias;
	}
	my $addresses = "";
	foreach $alias ( @mailAddress ) {
		$addresses .= "\"" . $alias . "\", ";
	}
	$addresses =~ s/\ *$//;
	$addresses =~ s/\,$//;
	if ( ! ( $vacation ) ) {
		$vacation = "";
	}

	if ( ! ( $start_v ) ) {
		$start_v = 0;
		next;
	}
	#print time2str("%d.%m.%Y", $start_v)."\n";
	my $start_date_string = time2str("%d.%m.%Y", $start_v)."\n";

	if ( ! ( $stop_v ) ) {
		$stop_v = 0;
		next;
	}
	#print time2str("%d.%m.%Y", $stop_v)."\n";
	my $stop_date_string = time2str("%d.%m.%Y", $stop_v)."\n";

	chomp $start_date_string;
	chomp $stop_date_string;
	$vacation =~ s/$start_date_ph/$start_date_string/g;
	$vacation =~ s/$stop_date_ph/$stop_date_string/g;

	if ( ! ( $server_v ) ) {
		$server_v = "";
		next;
	}
	#print $uid_v . " | " .
	#	$addresses . " | " .
	#	"\n";

	my ($sieve_user, $tmp) = split ( /\@/, $mail_v );

	print "today = $today\nstart = $start_v\nstop = $stop_v\n";
	my $real_stop = $stop_v + 86400;
	if ( ( $today >= $start_v ) && ( $today < $real_stop ) ) {
		print "activating vacation for user $uid_v\n";

		my $srv_filter = "(&(goImapName=$server_v)(objectClass=goImapServer))";
		my $srv_list_of_attributes = [ 'goImapSieveServer', 'goImapSievePort', 'goImapAdmin', 'goImapPassword' ];
		my $srv_result = ldap_search ( $ldap_url, $srv_filter, $search_scope, $ldap_base, $srv_list_of_attributes, $bind_dn, $bind_dn_pwd );
		my @srv_entries = $srv_result->entries;
		my $srv_entry = {};
		my $noe = @srv_entries;
		if ( $noe == 0 ) {
			printf STDERR "Error: no $server_attribute defined! Aboarting...";
		} elsif ( $noe > 1 ) {
			printf STDERR "Error: multiple $server_attribute defined! Aboarting...";
		} else {
			my $goImapSieveServer = $srv_entries[0]->get_value ( 'goImapSieveServer' );
			my $goImapSievePort = $srv_entries[0]->get_value ( 'goImapSievePort' );
			my $goImapAdmin = $srv_entries[0]->get_value ( 'goImapAdmin' );
			my $goImapPassword = $srv_entries[0]->get_value ( 'goImapPassword' );
			if ( ( $goImapSieveServer ) && ( $goImapSievePort ) && ( $goImapAdmin ) && ( $goImapPassword ) ) {
#				if ( ! ( $sieve_user = $uid_v ) ) {
#					$sieve_user = $uid_v;
#				}
				#my $sieve = opensieve ( $goImapAdmin, $goImapPassword, $sieve_user, $goImapSieveServer, $goImapSievePort);
				my $sieve = opensieve ( $goImapAdmin, $goImapPassword, $uid_v, $goImapSieveServer, $goImapSievePort);
				@sieve_scripts = listscripts ( $sieve );
				#print Dumper (@sieve_scripts);
				$script_name = "";
				if ( @sieve_scripts ) {
					foreach $script_name ( @sieve_scripts ) {
						if ( $script_name =~ m/$gosa_sieve_script_name/ ) {
							$gosa_sieve_script_status = "TRUE";
						}
					}
					if ( $gosa_sieve_script_status eq "TRUE" ) {
						print "retrieving and modifying gosa sieve script for user $uid_v\n";
						# requirements
						$sieve_script = getscript( $sieve, $gosa_sieve_script_name );
						#print "$sieve_script\n";
						if ( ! ( $sieve_script ) ) {
							print "No Sieve Script! Creating New One!\n";
							$sieve_script = $gosa_sieve_header;
						}
						if ( $sieve_script =~ m/require.*\[.*["|'] *vacation *["|'].*\]/ ) {
							print "require vacation ok\n";
						} else {
							print "require vacation not ok\n";
							print "modifying require statement\n";
							$sieve_script =~ s/require(.*\[.*)\]/require$1\, "vacation"\]/;
						}
						if ( ! ( $sieve_script =~ m/$vacation_header_template/ ) ) {
							print "no match header template\n";
							$sieve_vacation = $vacation_header_template .
										"\n" .
										"vacation :addresses [$addresses]\n" .
										"\"" .
										$vacation . 
										"\n\"\;" .
										"\n" .
										$vacation_footer_template .
										"\n\n";
						}
						#print ( "$sieve_vacation\n" );
						#print ( "$sieve_script\n" );
						# including vacation message
						if ( $sieve_script =~ m/$gosa_sieve_spam_header/ ) {
							#print "MATCH\n";
							$sieve_script =~ s/($gosa_sieve_spam_header[^{}]*{[^{}]*})/$1\n\n$sieve_vacation/;
						} else {
							$sieve_script =~ s/require(.*\[.*\]\;)/require$1\n\n$sieve_vacation/;
						}
						#print ( "START SIEVE $sieve_script\nSTOP SIEVE" );
						# uploading new sieve script
						putscript( $sieve, $gosa_sieve_script_name, $sieve_script );
						# activating new sieve script
						setactive( $sieve, $gosa_sieve_script_name );
					} else {
						print "no gosa script available for user $uid_v, creating new one";
						$sieve_script = $gosa_sieve_header . "\n\n" . $sieve_vacation;
						# uploading new sieve script
						putscript( $sieve, $gosa_sieve_script_name, $sieve_script );
						# activating new sieve script
						setactive( $sieve, $gosa_sieve_script_name );
					}
				}
				closesieve ( $sieve );
			}
		}
	} elsif ( $today >= $real_stop ) {
		print "deactivating vacation for user $uid_v\n";

		my $srv_filter = "(&(goImapName=$server_v)(objectClass=goImapServer))";
		my $srv_list_of_attributes = [ 'goImapSieveServer', 'goImapSievePort', 'goImapAdmin', 'goImapPassword' ];
		my $srv_result = ldap_search ( $ldap_url, $srv_filter, $search_scope, $ldap_base, $srv_list_of_attributes, $bind_dn, $bind_dn_pwd );
		my @srv_entries = $srv_result->entries;
		my $srv_entry = {};
		my $noe = @srv_entries;
		if ( $noe == 0 ) {
			printf STDERR "Error: no $server_attribute defined! Aboarting...";
		} elsif ( $noe > 1 ) {
			printf STDERR "Error: multiple $server_attribute defined! Aboarting...";
		} else {
			my $goImapSieveServer = $srv_entries[0]->get_value ( 'goImapSieveServer' );
			my $goImapSievePort = $srv_entries[0]->get_value ( 'goImapSievePort' );
			my $goImapAdmin = $srv_entries[0]->get_value ( 'goImapAdmin' );
			my $goImapPassword = $srv_entries[0]->get_value ( 'goImapPassword' );
			if ( ( $goImapSieveServer ) && ( $goImapSievePort ) && ( $goImapAdmin ) && ( $goImapPassword ) ) {
				#my $sieve = opensieve ( $goImapAdmin, $goImapPassword, $sieve_user, $goImapSieveServer, $goImapSievePort);
				my $sieve = opensieve ( $goImapAdmin, $goImapPassword, $uid_v, $goImapSieveServer, $goImapSievePort);
				@sieve_scripts = listscripts ( $sieve );
				$script_name = "";
				if ( @sieve_scripts ) {
					foreach $script_name ( @sieve_scripts ) {
						if ( $script_name =~ m/$gosa_sieve_script_name/ ) {
							$gosa_sieve_script_status = "TRUE";
						}
					}
					if ( $gosa_sieve_script_status eq "TRUE" ) {
						# removing vacation part
						$sieve_script = getscript( $sieve, $gosa_sieve_script_name );
						if ( $sieve_script ) {
							#print "OLD SIEVE SCRIPT:\n$sieve_script\n\n";
							$sieve_script =~ s/$vacation_header_template[^#]*$vacation_footer_template//;
							#print "NEW SIEVE SCRIPT:\n$sieve_script\n\n";
							# uploading new sieve script
							putscript( $sieve, $gosa_sieve_script_name, $sieve_script );
							# activating new sieve script
							setactive( $sieve, $gosa_sieve_script_name );
						}
					}
				}
				closesieve ( $sieve );
			}
		}
	} else {
		print "no vacation process necessary for user $uid_v\n";
	}
}
