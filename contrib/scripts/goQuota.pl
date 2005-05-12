#!/usr/bin/perl
#
# Parse squid log and write current traffic usage by users into cache
#
# Igor Muratov <migor@altlinux.org>
#
# $Id: goQuota.pl,v 1.4 2005/04/03 00:46:14 migor-guest Exp $
#

use strict;
use Time::Local;
use Net::LDAP;
use DB_File;
use POSIX qw(strftime);

my $debug = 0;
$|=1;

my $LDAP;
my $LDAP_HOST = "localhost";
my $LDAP_PORT = "389";
my $LDAP_BASE = "ou=People,dc=example,dc=com";

my $ACCESS_LOG = '/var/log/squid/access.log';
my $CACHE_FILE = '/var/spool/squid/quota.db';
my $DEFAULT_PERIOD = 'm';
my $FORMAT = "A16 A5 S S L A5 L L L";

my %cache;
my @lines;

sub timestamp
{
	return strftime("%a %b %X goQuota[$$]: ", localtime);
}

sub anonBind
{
	my $ldap = Net::LDAP->new( $LDAP_HOST, port => $LDAP_PORT );
	if($ldap)
	{
		my $mesg = $ldap->bind();
		$mesg->code && warn timestamp, "Can't bind to ldap://$LDAP_HOST:$LDAP_PORT:", $mesg->error, "\n";
		return $ldap;
	}
	else
	{
		warn timestamp, "Can't connect to ldap://$LDAP_HOST:$LDAP_PORT\n";
		return undef;
	}
}

# Retrive users's data from LDAP
sub update_userinfo
{
	my $user = shift;
	my $uid = $user->{uid};

	return undef unless $LDAP;

	# User unknown or cache field is expired
	my $result = $LDAP->search( base=>$LDAP_BASE,
		filter=>"(&(objectClass=gosaProxyAccount)(uid=$uid))",
		attrs=>[
			'uid',
			'gosaProxyAcctFlags',
			'gosaProxyQuota',
			'gosaProxyQuotaPeriod',
			'gosaProxyWorkingStop',
			'gosaProxyWorkingStart',
			'modifyTimestamp'
		]
	);
	$result->code && warn timestamp, "Failed to search: ", $result->error;

	# Get user's data
	if($result->count)
	{
		my $entry = ($result->entries)[0];

		$user->{uid} = ($entry->get_value('uid'))[0];
		$user->{modifyTimestamp} = ($entry->get_value('modifyTimestamp'))[0];
		$user->{gosaProxyWorkingStart} = ($entry->get_value('gosaProxyWorkingStart'))[0];
		$user->{gosaProxyWorkingStop} = ($entry->get_value('gosaProxyWorkingStop'))[0];
		$user->{gosaProxyAcctFlags} = ($entry->get_value('gosaProxyAcctFlags'))[0];

		my ($quota, $unit) = ($entry->get_value('gosaProxyQuota'))[0] =~ /(\d+)(\S)/g;
		$user->{gosaProxyQuota} = $quota;
		$user->{gosaProxyQuota} *= 1024 if $unit =~ /[Kk]/;
		$user->{gosaProxyQuota} *= 1048576 if $unit =~ /[Mm]/;
		$user->{gosaProxyQuota} *= 1073741824 if $unit =~ /[Gg]/;

		$user->{gosaProxyQuotaPeriod} = ($entry->get_value('gosaProxyQuotaPeriod'))[0] || $DEFAULT_PERIOD;
		# Return
		warn timestamp, "User $uid found in LDAP.\n";
		return 1;
	} else {
		# Unknown user
		warn timestamp, "User $uid does not exists in LDAP.\n";
		$user->{uid} = $uid;
		$user->{gosaProxyAcctFlags} = '[FTB]';
		$user->{gosaProxyQuota} = 0;
		$user->{gosaProxyQuotaPeriod} = 'y';
		return 0;
	}
}

sub get_update
{
	my $ts = shift;
	my %update;
	my $result = $LDAP->search( base=>$LDAP_BASE,
		filter=>"(&(objectClass=gosaProxyAccount)(modifyTimestamp>=$ts))",
		attrs=>'uid'
	);

	# Get user's data
	if($result->count)
	{
		my $entry = ($result->entries)[0];
		$update{($entry->get_value('uid'))[0]}++;
	}
	return %update;
}

# Check quota
sub update_quota
{
	my $user = shift;
	my $uid = $user->{uid};

	my $period = 0;
	$period = 3600 if $user->{gosaProxyQuotaPeriod} eq 'h';
	$period = 86400 if $user->{gosaProxyQuotaPeriod} eq 'd';
	$period = 604800 if $user->{gosaProxyQuotaPeriod} eq 'w';
	$period = 2592000 if $user->{gosaProxyQuotaPeriod} eq 'm';
	$period = 220752000 if $user->{gosaProxyQuotaPeriod} eq 'y';

	if($user->{lastRequest} - $user->{firstRequest} > $period)
	{
		if($user->{trafficUsage} > $user->{gosaProxyQuota})
		{
			warn timestamp, "Reduce quota for $uid while $period seconds.\n";
			$user->{trafficUsage} -= $user->{gosaProxyQuota};
			$user->{firstRequest} += $period;
		}
		else
		{
			warn timestamp, "Restart quota for $uid.\n";
			$user->{trafficUsage} = 0;
			$user->{firstRequest} = $user->{lastRequest};
		}
	}
}

sub dump_data
{
	my $user = shift;
	print "User: ",$user->{uid},"\n";
	print "\t",$user->{modifyTimestamp},"\n";
	print "\t",$user->{gosaProxyAcctFlags},"\n";
	print "\t",$user->{gosaProxyWorkingStart},"\n";
	print "\t",$user->{gosaProxyWorkingStop},"\n";
	print "\t",$user->{gosaProxyQuota},"\n";
	print "\t",$user->{gosaProxyQuotaPeriod},"\n";
	print "\t",$user->{trafficUsage},"\n";
	print "\t",$user->{firstRequest},"\n";
	print "\t",$user->{lastRequest},"\n";
}

sub unpack_user
{
	my $uid = shift;
	my $user;

	$user->{uid} = $uid;
	(
		$user->{modifyTimestamp},
		$user->{gosaProxyAcctFlags},
		$user->{gosaProxyWorkingStart},
		$user->{gosaProxyWorkingStop},
		$user->{gosaProxyQuota},
		$user->{gosaProxyQuotaPeriod},
		$user->{trafficUsage},
		$user->{firstRequest},
		$user->{lastRequest}
	) = unpack($FORMAT, $cache{$uid});

	return $user;
}

sub pack_user
{
	my $user = shift;

	$cache{$user->{uid}} = pack(
		$FORMAT,
		$user->{modifyTimestamp},
		$user->{gosaProxyAcctFlags},
		$user->{gosaProxyWorkingStart},
		$user->{gosaProxyWorkingStop},
		$user->{gosaProxyQuota},
		$user->{gosaProxyQuotaPeriod},
		$user->{trafficUsage},
		$user->{firstRequest},
		$user->{lastRequest}
	);
}

#--------------------------------------
$LDAP = anonBind or die timestamp, "No lines processed.\n";

# This is a first time parsing?
my $firstStart = 1;
$firstStart = 0 if -e $CACHE_FILE;

# Open log file and cache
my $cache = tie(%cache, 'DB_File', $CACHE_FILE, O_CREAT|O_RDWR);
my $log = tie(@lines, 'DB_File', $ACCESS_LOG, O_RDWR, 0640, $DB_RECNO)
	or die "Cannot open file $ACCESS_LOG: $!\n";

# Mark users which updated in LDAP
my %updated;
if(! $firstStart)
{
	my $ts = strftime("%Y%m%d%H%M%SZ", gmtime);
	%updated = get_update($cache{MODIFY_TIMESTAMP} || "19700101000000Z");

	my @count = %updated;
	$cache{MODIFY_TIMESTAMP} = $ts if $#count;

	foreach my $u (keys %updated)
	{
		warn timestamp, "User $u has been updated in LDAP. Refresh data.\n";
		my $user = unpack_user($u);
		update_userinfo($user);
		pack_user($user);
	}
}

# Processing log file
my $index = $cache{TIMESTAMP} < (split / +/, $lines[0])[0]
	? 0 : $cache{STRING_NUMBER};
warn timestamp, "Cache update start at line $index.\n";
while($lines[$index])
{
	# There are array named lines with elements
	# 0 - line timestamp
	# 1 - ?? (unused)
	# 2 - client's IP (unused)
	# 3 - squid's cache status TEXT_CODE/num_code (unused)
	# 4 - object size in bytes
	# 5 - metod (unused)
	# 6 - URL (unused)
	# 7 - username
	# 8 - load status TYPE/source
	# 9 - mime type (unused)
	my @line = split / +/, $lines[$index++];

	# Skip line if have no incoming traffic
	(my $errcode = $line[8]) =~ s/\/\S+//;
	next if $errcode eq "NONE";

	# Get data from cache
	(my $uid = $line[7]) =~ s/^-$/anonymous/;
	my $user = unpack_user($uid);

	# Update user info from LDAP if need
	if ( !exists($cache{$uid}) )
	{
		warn timestamp, "User $uid is not in cache. Go to search LDAP.\n";
		update_userinfo($user);
	}

	# Update traffic info
	$user->{trafficUsage} += $line[4];
	$user->{firstRequest} |= $line[0];
	$user->{lastRequest} = $line[0];

	update_quota($user);
	pack_user($user);

	dump_data($user) if $debug;

	$cache{TIMESTAMP} = $user->{lastRequest};
}

warn timestamp, $index - $cache{STRING_NUMBER}, " new lines processed.\n";
$cache{STRING_NUMBER} = $index;

$LDAP->unbind;
untie @lines;
untie %cache;

