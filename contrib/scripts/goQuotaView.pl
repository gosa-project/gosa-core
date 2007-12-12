#!/usr/bin/perl
#
# Show user info from cache
#
# Igor Muratov <migor@altlinux.org>
#
# $Id: goQuotaView.pl,v 1.2 2005/04/03 00:46:14 migor-guest Exp $
#

use strict;
use DB_File;

my $CACHE_FILE = '/var/spool/squid/quota.db';
my $FORMAT = "A16 A5 S S L A5 L L L";

my %cache;

sub min2time
{
	my $min = shift;
	return sprintf("%2d:%02d",$min/60,$min%60);
}

sub show_user
{
	my $uid = shift;

	my (
		$modifyTimestamp, $gosaProxyAcctFlags, $gosaProxyWorkingStart,
		$gosaProxyWorkingStop, $gosaProxyQuota, $gosaProxyQuotaPeriod,
		$trafficUsage, $firstRequest, $lastRequest
	) = unpack($FORMAT, $cache{$uid});

	my ($ts_Y, $ts_M, $ts_D, $ts_h, $ts_m, $ts_s)
		= $modifyTimestamp =~ /(\d\d\d\d)(\d\d)(\d\d)(\d\d)(\d\d)(\d\d)/g;
	my $ts = "$ts_D\.$ts_M\.$ts_Y $ts_h:$ts_m:$ts_s GMT";

	$gosaProxyAcctFlags =~ s/[\[\]]//g;
	$gosaProxyAcctFlags =~ s/F/unwanted content, /g;
	$gosaProxyAcctFlags =~ s/T/work time, /g;
	$gosaProxyAcctFlags =~ s/B/traffic/g;

	$gosaProxyQuotaPeriod =~ s/h/hour/;
	$gosaProxyQuotaPeriod =~ s/d/day/;
	$gosaProxyQuotaPeriod =~ s/w/week/;
	$gosaProxyQuotaPeriod =~ s/m/month/;
	$gosaProxyQuotaPeriod =~ s/y/year/;

	$firstRequest = localtime($firstRequest);
	$lastRequest = localtime($lastRequest);

	printf "User: %s
  LDAP modify timestamp\t%s
  Limited by\t\t%s
  Work time from\t%s
  Work time to\t\t%s
  Quota period\t\tOne %s
  Traffic quota size\t%s bytes
  Current traffic usage\t%s bytes
  First request time\t%s
  Last request time\t%s\n",
	$uid, $ts, $gosaProxyAcctFlags, min2time($gosaProxyWorkingStart),
	min2time($gosaProxyWorkingStop), $gosaProxyQuotaPeriod, $gosaProxyQuota,
	$trafficUsage, $firstRequest, $lastRequest;
}

#------------------------
tie(%cache, 'DB_File', $CACHE_FILE, O_CREAT|O_RDWR);

if($ARGV[0])
{
	show_user($ARGV[0]);
}
else
{
	print "eee\n";
	printf "LAST STRING: %d\nLAST CACHE UPDATE: %s\nLDAP LAST CHANGE:  %s\n",
		$cache{STRING_NUMBER},
		time2str("%d.%m.%Y %H:%M:%S",$cache{TIMESTAMP}),
		$cache{MODIFY_TIMESTAMP};

	foreach my $user (keys %cache)
	{
		next if $user eq "TIMESTAMP";
		next if $user eq "STRING_NUMBER";
		next if $user eq "MODIFY_TIMESTAMP";
		show_user($user);
	}
}

untie %cache;
