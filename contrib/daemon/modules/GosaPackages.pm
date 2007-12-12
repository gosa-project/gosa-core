package GosaPackages;

use Exporter;
@ISA = ("Exporter");

# Each module has to have a function 'process_incoming_msg'. This function works as a interface to gosa-sd and recieves the msg hash from gosa-sd. 'process_incoming_function checks, wether it has a function to process the incoming msg and forward the msg to it. 


use strict;
use warnings;
use GosaSupportDaemon;

BEGIN{}

END{}


### START ##########################

# create general settings for this module
my $gosa_cipher = &create_ciphering($main::gosa_passwd);

sub get_module_tags {
    
    # dort stehen drei packettypen, für die sich das modul anmelden kann, gosa-admin-packages, 
    #   server-packages, client-packages
    my %tag_hash = (gosa_admin_packages => "yes", 
                    server_packages => "no", 
                    client_packages => "no");
    return \%tag_hash;
}


sub process_incoming_msg {
    my ($crypted_msg) = @_ ;
    if(not defined $crypted_msg) {
        &main::daemon_log("function 'process_incoming_msg': got no msg", 7);
    }
    &main::daemon_log("GosaPackages: crypted_msg:$crypted_msg", 7);
    &main::daemon_log("GosaPackages: crypted_msg len:".length($crypted_msg), 7);

    $crypted_msg =~ /^([\s\S]*?)\.(\d{1,3}?)\.(\d{1,3}?)\.(\d{1,3}?)\.(\d{1,3}?)$/;
    $crypted_msg = $1;
    my $host = sprintf("%s.%s.%s.%s", $2, $3, $4, $5);
 
    &main::daemon_log("GosaPackages: crypted_msg:$crypted_msg", 7);
    &main::daemon_log("GosaPackages: crypted_msg len:".length($crypted_msg), 7);


    # collect addresses from possible incoming clients
    # only gosa is allowd as incoming client
    &main::daemon_log("GosaPackages: host_key: $host", 7);
    &main::daemon_log("GosaPackages: key_passwd: $main::gosa_passwd", 7);

    $gosa_cipher = &main::create_ciphering($main::gosa_passwd);
    # determine the correct passwd for deciphering of the incoming msgs
    my $msg = "";
    my $msg_hash;
    eval{
        $msg = &main::decrypt_msg($crypted_msg, $gosa_cipher);
        &main::daemon_log("GosaPackages: decrypted_msg: $msg", 7);

        $msg_hash = $main::xml->XMLin($msg, ForceArray=>1);
    };
    if($@) {
        &main::daemon_log("WARNING: GosaPackages do not understand the message:", 5);
        &main::daemon_log("$@", 7);
        return;
    }

    &main::daemon_log("GosaPackages: msg for daemon from host:", 1);
    &main::daemon_log("\t$host", 1);
    &main::daemon_log("GosaPackages: msg to process:", 5);
    &main::daemon_log("\t$msg", 5);
    
    $msg = "gosaPackages hat was bekommen";
    
    my $out_cipher = &main::create_ciphering($main::gosa_passwd);
    my $out_msg = &main::encrypt_msg($msg, $out_cipher);
    return $out_msg;

}


#===  FUNCTION  ================================================================
#         NAME:  got_ping
#   PARAMETERS:  msg_hash - hash - hash from function create_xml_hash
#      RETURNS:  nothing
#  DESCRIPTION:  process this incoming message
#===============================================================================
sub got_ping {
    my ($msg_hash) = @_;
    
    my $source = @{$msg_hash->{source}}[0];
    my $target = @{$msg_hash->{target}}[0];
    my $header = @{$msg_hash->{header}}[0];
    
    if(exists $main::known_daemons->{$source}) {
        &main::add_content2known_daemons(hostname=>$source, status=>$header);
    } else {
        &main::add_content2known_clients(hostname=>$source, status=>$header);
    }
    
    return;
}


1;
