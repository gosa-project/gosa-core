#!/usr/bin/perl


# Copyright (C) 2005 Guillaume Delecourt <guillaume.delecourt@opensides.be>
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
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#

use Net::LDAP;
use Getopt::Std;
use Net::LDAP::Schema;
use Net::LDAP::LDIF;

# Variables a config
$admin="cn=ldapadmin,dc=example,dc=be";
$password="";
$peopleou="ou=People,dc=example,dc=be";
$base="dc=example,dc=be";
$scope="one"; # par defaut
$dump_file="myldaptree.ldif";
$server="localhost";


my %Options;

my $ok = getopts('?', \%Options);

#Verifying if help is needed
if ( (!$ok) || (@ARGV < 1) || ($Options{'?'}) ) {
	&help();
}

print "We backup the whole tree before every operation\n";
&dump();

$comm=$ARGV[0];

if($comm eq "del" && @ARGV >1 )
{	
	print "You asked to delete attribute : ";
	$i=1;
	while($ARGV[$i] ne "")
	{	
			print $ARGV[$i]." ";
			$i++;
	}
	print "\n";
	$ldap = Net::LDAP->new($server);
	$ldap->bind($admin,password=>$password);


	print "ldap connection" .$ldap;

	$mesg = $ldap->search(filter=>"(objectClass=*)",base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;

	foreach $entry (@entries) {
		$i=1;
		print $entry->dn()."\n";
		while($ARGV[$i] ne "")
		{	
			if($ARGV[$i] eq "obj"){$obj=1;$i++;next}
			if($obj==1)
			{
				$mesg = $ldap->modify($entry->dn(), delete => {"ObjectClass"=>"$ARGV[$i]"});
				print "\t objectClass: ".$ARGV[$i];
			}
			else
			{
				$mesg = $ldap->modify($entry->dn(), delete => [$ARGV[$i]]);
				print "\t attribut: ".$ARGV[$i];
			}
			$obj=0;
			$i++;
		}
		
		print "\n";
	}
	$ldap->unbind;
	exit(0);
}
elsif($comm eq "gosa" && @ARGV ==1)
{
	print "Add GOsa attribute for the following users\n";
	print "---------------------------------------------\n";
	$ldap = Net::LDAP->new($server);
	$ldap->bind($admin,password=>$password);
	$mesg = $ldap->search(filter=>"&(!(objectClass~=gosaAccount))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;

	foreach $entry (@entries) {
		$mesg = $ldap->modify($entry->dn(), add => { "ObjectClass" => "gosaAccount"});
		$mesg = $ldap->modify($entry->dn(), add => { "ObjectClass" => "organizationalPerson"});
		$mesg = $ldap->modify($entry->dn(), add => { "ObjectClass" => "Person"});
		print $entry->dn();
		print "\n";
	}
	$ldap->unbind;
	exit(0);
}
elsif($comm eq "modif" && @ARGV >1)
{
	print "Modifications asked\n";
	print "------------------------\n";
	$ldap = Net::LDAP->new($server);
	$ldap->bind($admin,password=>$password);

	$mesg = $ldap->search(filter=>"(objectClass=*)",base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$mesg = $ldap->modify($entry->dn(), replace => { "$ARGV[1]" => "$ARGV[2]" } );
	print $entry->dn()."\n\tattribut $ARGV[1] modifié avec la valeur $ARGV[2]\n";
	}
	$ldap->unbind;
	exit(0);
}
elsif($comm eq "dump" && @ARGV ==1)
{
	&dump();
}
else
{
	&help();
}

sub help()
{
    print_banner;
    print "Usage: $0 [-?] command\n";
    print "\t-?	show this help message\n";
    print "\tgosa -> add GOsa attributes for the whole the people branch !\n";
    print "\tdel attribut  -> Remove an attribute for the whole people branch !\n";
    print "\tmodif <attribute> <attribute value> -> to modify the attribute\n";
    print "\tdump to dump the whole ldap tree\n";
    exit (1);
}

sub dump()
{
    $ldap = Net::LDAP->new($server) or die "$@";
    $ldap->bind($admin,password=>$password);
    my $ldif = Net::LDAP::LDIF->new($dump_file,'w') ;
    $mesg = $ldap->search ( 
				base   => "$base",
				filter => "(objectclass=*)"
			);
    $ldif->write_entry($mesg->entries) ;
    $ldap->unbind;
}
