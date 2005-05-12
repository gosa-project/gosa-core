#!/usr/bin/perl

use strict;
use DB_File;

my $db = "/var/spool/squid/domains.db";
my %db;

tie(%db, 'DB_File', $db);

while(<>)
{
	chomp;
	unless(exists($db{$_}))
	{
		$db{$_} = 1;
	}
}

untie %db;
