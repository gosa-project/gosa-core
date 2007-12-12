#!/usr/bin/perl

use strict;
use File::Path;
use File::Basename;

# Check for parameters
if ($ARGV[0] eq ""){
	die ("Usage: parse-pkg <config-file>\n");
}

# Generate cache
gen_cache($ARGV[0]);
exit 0;

#-----------------------------------------------------------------------------

sub gen_cache
{
	my ($conffile)= @_;
	my $line;

	print "Generating GOsa package cache - this may take some time\n";
	open(CONFIG, "<$conffile") or die("Failed to open '$conffile' - aborted\n");
	
	# Read lines
	while ($line = <CONFIG>){
		# Unify
		chop($line);
		$line =~ s/^\s+//;
		$line =~ s/^\s+/ /;

		# Strip comments
		$line =~ s/#.*$//g;

		# Skip empty lines
		if ($line =~ /^\s*$/){
			next;
		}

		# Interpret deb line
		if ($line =~ /^deb [^\s]+\s[^\s]+\s[^\s]+/){
			my ($baseurl)  = ($line =~ /^deb\s([^\s]+)/);
			my ($dist)     = ($line =~ /^deb\s[^\s]+\s([^\s]+)/);
			my ($sections) = ($line =~ /^deb\s[^\s]+\s[^\s]+\s(.*)$/);
			
			my $section;
			foreach $section (split(" ", $sections)){
				parse_package_info ("$baseurl", "$dist", "$section");
			}
		}
	}

	close (CONFIG);
}

#-----------------------------------------------------------------------------

sub parse_package_info
{
	my ($baseurl, $dist, $section)= @_;
	my ($package, $server);

	foreach $package ("Packages.gz"){
		print ("* trying to retrieve $baseurl/dists/$dist/$section/binary-i386/$package\n");
	
		($server)= ($baseurl =~ /^[^\/]+\/\/([^\/]+)\/.*$/);
		get_package("$baseurl/dists/$dist/$section/binary-i386/$package", "/etc/gosa/fai/$server/$dist/$section");
		parse_package("/etc/gosa/fai/$server/$dist/$section");
		last;
	}
}

#-----------------------------------------------------------------------------

sub get_package
{
	my ($url, $dest)= @_;

	# This is ugly, but I've no time to take a look at "how it works in perl"
	system("wget '$url' -O '$dest'");
	system("gzip -cd '$dest' > '$dest.in'");
	system("rm -f '$dest'");

	return 0;
}

#-----------------------------------------------------------------------------

sub parse_package
{
	my ($path)= @_;
	my ($name, $desc, $vers, $sect, $line);

	my $tpath= dirname($path);
	-d "$tpath" || mkpath "$tpath";

	open(PACKAGES, "<$path.in") or die("Failed to open '$path.in' - aborted\n");
	open(OUT, ">$path") or die("Failed to open '$path' - aborted\n");
	
	# Read lines
	while ($line = <PACKAGES>){
		# Unify
		chop($line);

		# Use empty lines as a trigger
		if ($line =~ /^\s*$/){
			print OUT "$name|$vers|$sect|$desc\n";
			next;
		}

		# Trigger for package name
		if ($line =~ /^Package:\s/){
			($name)= ($line =~ /^Package: (.*)$/);
			next;
		}

		# Trigger for version
		if ($line =~ /^Version:\s/){
			($vers)= ($line =~ /^Version: (.*)$/);
			next;
		}

		# Trigger for description
		if ($line =~ /^Description:\s/){
			($desc)= ($line =~ /^Description: (.*)$/);
			next;
		}

		# Trigger for description
		if ($line =~ /^Section:\s/){
			($sect)= ($line =~ /^Section: (.*)$/);
			next;
		}
	}

	close (OUT);
	close (PACKAGES);
}

