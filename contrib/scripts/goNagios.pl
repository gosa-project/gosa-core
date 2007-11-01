#!/usr/bin/perl -w


# Copyright (C) 2005 Guillaume Delecourt <guillaume.delecourt@opensides.be>
# Copyright (C) 2005 Vincent Senave <vincent.senave@opensides.be>
# Copyright (C) 2007 Benoit Mortier <benoit.mortier@opensides.be>
#
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
use MIME::Lite;
use Sys::Syslog;
use Switch;
use strict;

# Default config variables

my $nb_user=0;
my $nb_group=0;

my ($i,$file,$ldap,@nagiosmail,
	$line,$text,$mesg,$entry,$userlist1,$userlist2,$userlist3,$userlist4,
	$userlist5,$userlist6,$userlist7,$msg,@groupname,@groupmembers,@contactlias,
	@groupdescription,@servicenotificationoptions,@servicenotificationperiod,
	@hostnotificationoptions,@hostnotificationperiod,$stdout,
	$usercontact,$members,@contactname,@nagiosalias,$j,@entries
);

my $error_open = "Problem opening file ";

# Where are the config stored
my $gosa_bind_conf="ldap2nagios_bind.conf";
my $gosa_ldap_conf="ldap2nagios.conf";

# Read the config files
my %config_bind = &read_conf($gosa_bind_conf);
my %config = &read_conf($gosa_ldap_conf);

# Parameters in ldap2nagios.conf
my $peopleou=$config{peopleou};
my $groupeou=$config{groupeou};
my $base=$config{base};
my $scope=$config{scope};# par defaut
my $server=$config{server};
my $logs=$config{logs};
my $logfile=$config{logfile};
my $email=$config{email};
my $email_from=$config{email_from};
my $nagiosadmin=$config{nagiosadmin};
my $cgi_file=$config{cgi_file};
my $contacts_file=$config{contacts_file};
my $contacts_groups_file=$config{contacts_groups_file};

# Parameters in ldap2nagios_bind.conf
my $admin=$config_bind{masterDN};
my $password=$config_bind{masterPw};


	$ldap = Net::LDAP->new($server);
	$mesg = $ldap->bind($admin,password=>$password) or syslog('error',$mesg->err) && print $mesg->code && die $mesg->error;

	#Searching the objectClass nagiosContact
	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosContact))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	$i=0;
	foreach $entry (@entries) {
		$contactname[$i]=$entry->get_value('uid');
		$nagiosmail[$i]=$entry->get_value('NagiosMail');
		$nagiosalias[$i]=$entry->get_value('NagiosAlias');
		$hostnotificationperiod[$i]=$entry->get_value('HostNotificationPeriod');
		$servicenotificationperiod[$i]=$entry->get_value('ServiceNotificationPeriod');
		$hostnotificationoptions[$i]=$entry->get_value('HostNotificationOptions');
		$servicenotificationoptions[$i]=$entry->get_value('ServiceNotificationOptions');
		$usercontact.=$entry->get_value('uid').",";
		
		$i++;
	}
	$nb_user=$i;
		
	#Searching the objectClass nagiosContactGroup
	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosContactGroup))", base=>$groupeou,scope=>$scope);
	@entries = $mesg->entries;
	$i=0;
	foreach $entry (@entries) {
		$groupname[$i]=$entry->get_value('cn');

		$groupdescription[$i]=$entry->get_value('description');

		if(!$groupdescription[$i]){$groupdescription[$i]=$groupname[$i]};

		$j=0;
		
			foreach $members($entry->get_value('memberUid')) {
				$groupmembers[$i][$j]=$members;
				$j++;
			}
		$i++;
	}
	$nb_group=$i;

	#Searching for the objectClass nagiosAuth
	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedSystemInformation~=checked))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist1.=$entry->get_value('uid').",";
	}
	$userlist1.=$nagiosadmin."\n\r";

	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedConfigurationInformation~=checked))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist2.=$entry->get_value('uid').",";
	}
	$userlist2.=$nagiosadmin."\n\r";

	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedSystemCommands~=checked))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist3.=$entry->get_value('uid').",";
	}
	$userlist3.=$nagiosadmin."\n\r";

	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedAllServices~=checked))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist4.=$entry->get_value('uid').",";
	}
	$userlist4.=$nagiosadmin."\n\r";

	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedAllHosts~=checked))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist5.=$entry->get_value('uid').",";
	}
	$userlist5.=$nagiosadmin."\n\r";


	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedAllServiceCommands~=checked))", base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist6.=$entry->get_value('uid').",";
	}
	$userlist6.=$nagiosadmin."\n\r";

	$mesg = $ldap->search(filter=>"(&(objectClass~=nagiosAuth)(AuthorizedAllHostCommands~=checked))",base=>$peopleou,scope=>$scope);
	@entries = $mesg->entries;
	foreach $entry (@entries) {
	$userlist7.=$entry->get_value('uid').",";
	}
	$userlist7.=$nagiosadmin."\n\r";

	&modiffile_cgi($cgi_file);
	&modiffile_contact($contacts_file);
	&modiffile_group($contacts_groups_file);
	
	$ldap->unbind;
	switch($logs)
	{
	case "mail"	{&mail()}
	case "log"	{&writelog()}
	case "terminal"	{&write_stdout()}

	}
	exit(0);

sub modiffile_contact()
{
	$file=$_[0];
	my $text="";
	open(FH,"$file") || die "$error_open $file";

	for($i=0;$i<$nb_user;$i++)
	{
		$text.="\n\ndefine contact{\n";
		$text.="\n\tcontact_name \t\t\t".$contactname[$i];
		$text.="\n\talias \t\t\t\t".$nagiosalias[$i];
		$text.="\n\thost_notification_period \t".$hostnotificationperiod[$i];
		$text.="\n\thost_notification_options \t".$hostnotificationoptions[$i];
		$text.="\n\tservice_notification_period \t".$servicenotificationperiod[$i];
		$text.="\n\tservice_notification_options \t".$servicenotificationoptions[$i];
		$text.="\n\temail \t\t\t\t".$nagiosmail[$i];
		$text.="\n}\n\n";
	}
	close(FH);
	open(FH,"> $file") || die "$error_open $file";
	print  FH "$text";
	close(FH);
	
}

sub modiffile_group()
{
	$file=$_[0];
	$text="";
	$j=0;
	$i=0;
	open(FH,"$file") || die "$error_open $file";

	for($i=0;$i<$nb_group;$i++)
	{
		$text.="\n\ndefine contactgroup{\n";
		$text.="\n\tcontactgroup_name \t".$groupname[$i];
		$text.="\n\talias \t\t\t".$groupdescription[$i];
		$text.="\n\tmembers \t\t";
		while(defined($groupmembers[$i][$j]))
		{
			$text.=$groupmembers[$i][$j]." ";
 			$j++;
		}
		$text.="\n}\n\n";
	}
	
	close(FH);
	open(FH,"> $file") || die "$error_open $file";
	print FH "$text";
	close(FH);
	
}

sub modiffile_cgi()
{
	$file=$_[0];
	$text="";
	open(FH,"$file") || die "$error_open $file";
	while(<FH>)
	{	
		$line=$_;
		if($line =~ /^authorized_for_system_information=*/i){$text.="authorized_for_system_information=".$userlist1}
		elsif($line =~ /^authorized_for_configuration_information=*/i){$text.="authorized_for_configuration_information=".$userlist2}
		elsif($line =~ /^authorized_for_system_commands=*/i){$text.="authorized_for_system_commands=".$userlist3}
		elsif($line =~ /^authorized_for_all_services=*/i){$text.="authorized_for_all_services=".$userlist4}
		elsif($line =~ /^authorized_for_all_hosts=*/i){$text.="authorized_for_all_hosts=".$userlist5}
		elsif($line =~ /^authorized_for_all_service_commands=*/i){$text.="authorized_for_all_host_commands=".$userlist6}
		elsif($line =~ /^authorized_for_all_host_commands=*/i){$text.="authorized_for_all_service_commands=".$userlist7}
		else {$text.=$line};
	}
	close(FH);
	open(FH,"> $file") || die "$error_open $file";
	print FH "$text";
	close(FH);
	
}

sub read_conf()
{
        my %conf;
        open (CONFIGFILE, "$_[0]") || die "Unable to open $_[0] for reading !\n";
        while (<CONFIGFILE>) {
                chomp($_);
                ## throw away comments
                next if ( /^\s*#/ || /^\s*$/ || /^\s*\;/);
                ## check for a param = value
                my ($parameter,$value)=read_parameter($_);
                $value = &subst_configvar($value,\%conf);
                $conf{$parameter}=$value;
          }
        close (CONFIGFILE);
        return(%conf);
}


sub read_parameter
{
        my $line=shift;
        ## check for a param = value
        if ($_=~/=/) {
          my ($param,$val);
          if ($_=~/"/) {
                #my ($param,$val) = ($_=~/(.*)\s*=\s*"(.*)"/);
                ($param,$val) = /\s*(.*?)\s*=\s*"(.*)"/;
          } elsif ($_=~/'/) {
                ($param,$val) = /\s*(.*?)\s*=\s*'(.*)'/;
          } else {
                ($param,$val) = /\s*(.*?)\s*=\s*(.*)/;
          }
          return ($param,$val);
        }
}

 
sub subst_configvar
{
        my $value = shift;
        my $vars = shift;

        $value =~ s/\$\{([^}]+)\}/$vars->{$1} ? $vars->{$1} : $1/eg;
        return $value;
}


sub mail
{

if($email eq ""){$email="root"}

$msg = MIME::Lite->new(
             From     => $config{email_from},
             To       => $config{email},
             Subject  => "Ldap2nagios Gosa plugin",
             Data     => $stdout
             );


$msg->send;
}


sub writelog
{
	open(F, "> $logfile");
	print F $stdout;
	close(F);
}

sub write_stdout
{
	$j=0;

	$stdout.="\n\nSearch for Nagios attribute in user and group list\n";
	$stdout.="-"x55;$stdout.="\n";

	for($i=0;$i<$nb_user;$i++) {
		$stdout.="\nContact $i : \n\tName\t\t\t $contactname[$i]";
		$stdout.="\n\n\tmail:\t\t\t\t $nagiosmail[$i]";
		$stdout.="\n\talias:\t\t\t\t $nagiosalias[$i]";
		$stdout.="\n\tHostNotificationPeriod:\t\t $hostnotificationperiod[$i]";
		$stdout.="\n\tServiceNotificationPeriod:\t $servicenotificationperiod[$i]";
		$stdout.="\n\tHostNotificationOptions:\t $hostnotificationoptions[$i]";
		$stdout.="\n\tServiceNotificationOptions:\t $servicenotificationoptions[$i]";
		$stdout.="\n"." "x15;$stdout.="-"x20;$stdout.=" "x 15;
	}
	
	for($i=0;$i<$nb_group;$i++) {
		$stdout.="\nGroupe $i : \n\tName\t\t $groupname[$i]";
		$stdout.="\n\talias \t\t $groupdescription[$i]";
		$stdout.="\n\tmembers \t";
		while(defined($groupmembers[$i][$j]))
		{
			$stdout.=$groupmembers[$i][$j]. " ";
 			$j++;
		}
	}
	
	$stdout.="\n\n\n\n\nAuthorization for the various Informations in Nagios\n"."-" x 53;$stdout.="\n";
	
	$stdout.="\nSystem infos :\t\t $userlist1";
	$stdout.="\nConfiguration infos :\t $userlist2";
	$stdout.="\nSystem commands : \t $userlist3";
	$stdout.="\nAll services :\t\t $userlist4";
	$stdout.="\nAll hosts :\t\t $userlist5";
	$stdout.="\nAll services commands :\t $userlist6";
	$stdout.="\nAll host commands :\t $userlist7";
	
	$stdout.="\n\n"; $stdout.=" "x10;$stdout.="-"x25;$stdout.=" "x10;
	$stdout.="\n$nb_user user(s) added in the file $file\n";
	$stdout.="\n$nb_group group(s) added in the file $file\n";
	
	print $stdout;

}
