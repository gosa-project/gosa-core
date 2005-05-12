#!/usr/bin/perl
#
# Squid redirect programm for GOsa project
#
# Igor Muratov <migor@altlinux.org>
#
# $Id: goSquid.pl,v 1.3 2005/04/03 00:46:14 migor-guest Exp $
#

use strict;
use POSIX qw(strftime);
use Time::Local;
use DB_File;

my $debug = 0;
$|=1;

my $DEFAULT_URL = "http://www.squid-cache.org/Squidlogo2.gif";
my $black_list = '/var/spool/squid/domains.db';
my $cache_file = '/var/spool/squid/quota.db';
my $format = "A16 A5 S S L A5 L L L";

my %cache;
my %blacklist;

sub timestamp
{
	return strftime("%a %b %X goSquid[$$]: ", localtime);
}

# Check url in our blacklist
sub unwanted_content
{
	my $url = shift;
	my $host = (split(/\//, $url))[2];

	return 1 if exists($blacklist{$host}) and $blacklist{$host} > 0;
	return undef;
}

# Check work time limit
sub work_time
{
	my $user = shift;
	my ($min,$hour) = (localtime)[1,2];
	my $time = $hour * 60 + $min;

	return 1 if $user->{gosaProxyWorkingStart} < $time and $user->{gosaProxyWorkingStop} > $time;
	return undef;
}

sub quota_exceed
{
	my $user = shift;

	return 1 if $user->{trafficUsage} > $user->{gosaProxyQuota};
	return undef;
}

sub check_access
{
	my ($user, $url) = @_;

	$user->{timed} = 0;
	$user->{quoted} = 0;
	$user->{filtered} = 0;

	if($user->{gosaProxyAcctFlags} =~ m/[F]/)
	{
		# Filter unwanted content
		$user->{filtered} = 1 if unwanted_content($url);
	}
	if($user->{gosaProxyAcctFlags} =~ m/[T]/)
	{
		# Filter unwanted content during working hours only
		$user->{timed} = 1 if work_time($user);
	}
	if($user->{gosaProxyAcctFlags} =~ m/B/)
	{
		$user->{quoted} = 1 if quota_exceed($user);
	}
}

#--------------------------------------
while (<>) {
	my ($url, $addr, $uid, $method) = split;
	my $time = timelocal(localtime);
	tie(%blacklist, 'DB_File', $black_list, O_RDONLY);
	tie(%cache, 'DB_File', $cache_file, O_RDONLY);

	if( exists($cache{$uid}) )
	{
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
		) = unpack($format, $cache{$uid});

		check_access($user, $url);

		if($user->{'disabled'})
		{
			warn timestamp, "Access denied for unknown user $uid\n";
		}
		elsif($user->{'timed'})
		{
			warn timestamp, "Access denied by worktime for $uid\n";
		}
		elsif($user->{'quoted'})
		{
			warn timestamp, "Access denied by quota for $uid\n";
		}
		elsif($user->{'filtered'})
		{
			warn timestamp, "Content $url filtered for $uid\n";
		}
		else
		{
			print "$url\n";
			next;
		}
	}

	untie %blacklist;
	untie %cache;

	print "$DEFAULT_URL\n";
}
