package ServerPackages;

use Exporter;
@ISA = ("Exporter");

# Each module has to have a function 'process_incoming_msg'. This function works as a interface to gosa-sd and recieves the msg hash from gosa-sd. 'process_incoming_function checks, wether it has a function to process the incoming msg and forward the msg to it. 


use strict;
use warnings;
use GosaSupportDaemon;

BEGIN{}

END {}


### START ##########



sub get_module_tags {
    
    # lese config file aus dort gibt es eine section Basic
    # dort stehen drei packettypen, für die sich das modul anmelden kann, gosa-admin-packages, 
    #   server-packages, client-packages
    my %tag_hash = (gosa_admin_packages => "yes", 
                    server_packages => "yes", 
                    client_packages => "yes",
                    );
    return \%tag_hash;
}


sub process_incoming_msg {
    my ($crypted_msg) = @_ ;
    if(not defined $crypted_msg) {
        &main::daemon_log("function 'process_incoming_msg': got no msg", 7);
    }
    $crypted_msg =~ /^([\s\S]*?)\.(\d{1,3}?)\.(\d{1,3}?)\.(\d{1,3}?)\.(\d{1,3}?)$/;
    $crypted_msg = $1;
    my $host = sprintf("%s.%s.%s.%s", $2, $3, $4, $5);

    # collect addresses from possible incoming clients
    my @valid_keys;
    my @host_keys = keys %$main::known_daemons;
    foreach my $host_key (@host_keys) {    
        if($host_key =~ "^$host") {
            push(@valid_keys, $host_key);
        }
    }
    my @client_keys = keys %$main::known_clients;
    foreach my $client_key (@client_keys) {
        if($client_key =~ "^$host"){
            push(@valid_keys, $client_key);
        }
    }
    push(@valid_keys, $main::server_address);
    
    my $l = @valid_keys;
    my $msg_hash;
    my $msg_flag = 0;    
    my $msg = "";

    # determine the correct passwd for deciphering of the incoming msgs
    foreach my $host_key (@valid_keys) {
        eval{
            &main::daemon_log("ServerPackage: host_key: $host_key", 7);
            my $key_passwd;
            if (exists $main::known_daemons->{$host_key}) {
                $key_passwd = $main::known_daemons->{$host_key}->{passwd};
            } elsif (exists $main::known_clients->{$host_key}) {
                $key_passwd = $main::known_clients->{$host_key}->{passwd};
            } elsif ($host_key eq $main::server_address) {
                $key_passwd = $main::server_passwd;
            } 
            &main::daemon_log("ServerPackage: key_passwd: $key_passwd", 7);
            my $key_cipher = &create_ciphering($key_passwd);
            $msg = &decrypt_msg($crypted_msg, $key_cipher);
            &main::daemon_log("ServerPackages: decrypted msg: $msg", 7);
            $msg_hash = $main::xml->XMLin($msg, ForceArray=>1);
            #my $tmp = printf Dumper $msg_hash;
            #&main::daemon_log("DEBUG: ServerPackages: xml hash: $tmp", 7);
        };
        if($@) {
            &main::daemon_log("ServerPackage: key raise error: $@", 7);
            $msg_flag += 1;
        } else {
            last;
        }
    } 
    
    if($msg_flag >= $l)  {
        &main::daemon_log("WARNING: ServerPackage do not understand the message:", 5);
        &main::daemon_log("$@", 7);
        return;
    }

    # process incoming msg
    my $header = @{$msg_hash->{header}}[0]; 
    my $source = @{$msg_hash->{source}}[0];

    &main::daemon_log("ServerPackages: msg from host:", 5);
    &main::daemon_log("\t$host", 5);
    &main::daemon_log("ServerPackages: header from msg:", 5);
    &main::daemon_log("\t$header", 5);
    &main::daemon_log("ServerPackages: msg to process:", 5);
    &main::daemon_log("\t$msg", 5);

    my @targets = @{$msg_hash->{target}};
    my $len_targets = @targets;
    if ($len_targets == 0){     
        &main::daemon_log("ERROR: ServerPackages: no target specified for msg $header", 1);

    }  elsif ($len_targets == 1){
        # we have only one target symbol

        my $target = $targets[0];
        &main::daemon_log("SeverPackages: msg is for:", 7);
        &main::daemon_log("\t$target", 7);

        if ($target eq $main::server_address) {
            # msg is for server
            if ($header eq 'new_passwd'){ &new_passwd($msg_hash)}
            elsif ($header eq 'here_i_am') { &here_i_am($msg_hash)}
            elsif ($header eq 'who_has') { &who_has($msg_hash) }
            elsif ($header eq 'who_has_i_do') { &who_has_i_do($msg_hash)}
            elsif ($header eq 'update_status') { &update_status($msg_hash) }
            elsif ($header eq 'got_ping') { &got_ping($msg_hash)}
            elsif ($header eq 'get_load') { &execute_actions($msg_hash)}
            else { &main::daemon_log("ERROR: ServerPackages: no function assigned to this msg", 5) }

        
       } elsif ($target eq "*") {
            # msg is for all clients

            my @target_addresses = keys(%$main::known_clients);
            foreach my $target_address (@target_addresses) {
                if ($target_address eq $source) { next; }
                $msg_hash->{target} = [$target_address];
                &send_msg_hash2address($msg_hash, $target_address);
            }           
        } else {
            # msg is for one host

            if (exists $main::known_clients->{$target}) {
                &send_msg_hash2address($msg_hash, $target);
            } elsif (exists $main::known_daemons->{$target}) {
                # target is known
                &send_msg_hash2address($msg_hash, $target);
            } else {
                # target is not known
                &main::daemon_log("ERROR: ServerPackages: target $target is not known neither in known_clients nor in known_daemons", 1);
            }
        }
    }

    return ;
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


#===  FUNCTION  ================================================================
#         NAME:  new_passwd
#   PARAMETERS:  msg_hash - ref - hash from function create_xml_hash
#      RETURNS:  nothing
#  DESCRIPTION:  process this incoming message
#===============================================================================
sub new_passwd {
    my ($msg_hash) = @_;

    my $source = @{$msg_hash->{source}}[0];
    my $passwd = @{$msg_hash->{new_passwd}}[0];

    if (exists $main::known_daemons->{$source}) {
        &main::add_content2known_daemons(hostname=>$source, status=>"new_passwd", passwd=>$passwd);
        my $hash = &create_xml_hash("confirm_new_passwd", $main::server_address, $source);
        &send_msg_hash2address($hash, $source);

    } elsif (exists $main::known_clients->{$source}) {
        &main::add_content2known_clients(hostname=>$source, status=>"new_passwd", passwd=>$passwd);

    } else {
        &main::daemon_log("ERROR: $source not known, neither in known_daemons nor in known_clients", 1)   
    }

    return;
}


#===  FUNCTION  ================================================================
#         NAME:  here_i_am
#   PARAMETERS:  msg_hash - hash - hash from function create_xml_hash
#      RETURNS:  nothing
#  DESCRIPTION:  process this incoming message
#===============================================================================
sub here_i_am {
    my ($msg_hash) = @_;

    my $source = @{$msg_hash->{source}}[0];
    my $mac_address = @{$msg_hash->{mac_address}}[0];
    my $out_hash;

    # number of known clients
    my $nu_clients = keys %$main::known_clients;

    # check wether client address or mac address is already known
    if (exists $main::known_clients->{$source}) {
        &main::daemon_log("WARNING: $source is already known as a client", 1);
        &main::daemon_log("WARNING: values for $source are being overwritten", 1);   
        $nu_clients --;
    }

    # number of actual activ clients
    my $act_nu_clients = $nu_clients;

    &main::daemon_log("number of actual activ clients: $act_nu_clients", 5);
    &main::daemon_log("number of maximal allowed clients: $main::max_clients", 5);

    if($main::max_clients <= $act_nu_clients) {
        my $out_hash = &create_xml_hash("denied", $main::server_address, $source);
        &add_content2xml_hash($out_hash, "denied", "I_cannot_take_any_more_clients!");
        my $passwd = @{$msg_hash->{new_passwd}}[0]; 
        &send_msg_hash2address($out_hash, $source, $passwd);
        return;
    }
    
    # new client accepted
    my $new_passwd = @{$msg_hash->{new_passwd}}[0];

    # create known_daemons entry
    my $events = @{$msg_hash->{events}}[0];
    &main::create_known_client($source);
    &main::add_content2known_clients(hostname=>$source, events=>$events, mac_address=>$mac_address, 
                                status=>"registered", passwd=>$new_passwd);

    # return acknowledgement to client
    $out_hash = &create_xml_hash("registered", $main::server_address, $source);
    &send_msg_hash2address($out_hash, $source);

    # notify registered client to bus
    $out_hash = &main::create_xml_hash("new_client", $main::server_address, $main::bus_address, $source);
    &main::send_msg_hash2bus($out_hash);

    # give the new client his ldap config
    &new_ldap_config($source);

    return;
}


#===  FUNCTION  ================================================================
#         NAME:  who_has
#   PARAMETERS:  msg_hash - hash - hash from function create_xml_hash
#      RETURNS:  nothing 
#  DESCRIPTION:  process this incoming message
#===============================================================================
sub who_has {
    my ($msg_hash) = @_ ;
    
    # what is your search pattern
    my $search_pattern = @{$msg_hash->{who_has}}[0];
    my $search_element = @{$msg_hash->{$search_pattern}}[0];
    &main::daemon_log("who_has-msg looking for $search_pattern $search_element", 7);

    # scanning known_clients for search_pattern
    my @host_addresses = keys %$main::known_clients;
    my $known_clients_entries = length @host_addresses;
    my $host_address;
    foreach my $host (@host_addresses) {
        my $client_element = $main::known_clients->{$host}->{$search_pattern};
        if ($search_element eq $client_element) {
            $host_address = $host;
            last;
        }
    }
        
    # search was successful
    if (defined $host_address) {
        my $source = @{$msg_hash->{source}}[0];
        my $out_msg = &main::create_xml_hash("who_has_i_do", $main::server_address, $source, "mac_address");
        &main::add_content2xml_hash($out_msg, "mac_address", $search_element);
        &main::send_msg_hash2address($out_msg, $main::bus_address);
    }
    return;
}


sub who_has_i_do {
    my ($msg_hash) = @_ ;
    my $header = @{$msg_hash->{header}}[0];
    my $source = @{$msg_hash->{source}}[0];
    my $search_param = @{$msg_hash->{$header}}[0];
    my $search_value = @{$msg_hash->{$search_param}}[0];
    print "\ngot msg $header:\nserver $source has client with $search_param $search_value\n";
}


#===  FUNCTION  ================================================================
#         NAME:  new_ldap_config
#   PARAMETERS:  address - string - ip address and port of a host
#      RETURNS:  nothing
#  DESCRIPTION:  send to address the ldap configuration found for dn gotoLdapServer
#===============================================================================
sub new_ldap_config {
    my ($address) = @_ ;
    
    if (not exists $main::known_clients->{$address}) {
        &main::daemon_log("ERROR: $address does not exist in known_clients, cannot send him his ldap config", 1);
        return;
    }
    
    my $mac_address = $main::known_clients->{$address}->{"mac_address"};
    if (not defined $mac_address) {
        &main::daemon_log("ERROR: no mac address found for client $address", 1);
        return;
    }

    # fetch dn
    my $goHard_cmd = "ldapsearch -x '(&(objectClass=goHard)(macAddress=00:11:22:33:44:57))' dn gotoLdapServer";
    my $dn;
    my @gotoLdapServer;
    open (PIPE, "$goHard_cmd 2>&1 |");
#    my $rbits = "";
#    vec($rbits, fileno PIPE, 1) = 1;
#    my $rout;
#    my $nf = select($rout=$rbits, undef, undef, $ldap_timeout);
    while(<PIPE>) {
        chomp $_;
        # If it's a comment, goto next
        if ($_ =~ m/^[#]/) { next;}
        if ($_ =~ m/^dn: ([\S]+?)$/) {
            $dn = $1;
        } elsif ($_ =~ m/^gotoLdapServer: ([\S]+?)$/) {
            push(@gotoLdapServer, $1);
        }
    }
    close(PIPE);
    
    # no dn found
    if (not defined $dn) {
        &main::daemon_log("ERROR: no dn arose from command: $goHard_cmd", 1);
        return;
    }
    
    # no gotoLdapServer found
    my $gosaGroupOfNames_cmd = "ldapsearch -x '(&(objectClass=gosaGroupOfNames)(member=$dn))' gotoLdapServer";
    if (@gotoLdapServer == 0) {
        open (PIPE, "$gosaGroupOfNames_cmd 2>&1 |");
        while(<PIPE>) {
            chomp $_;
            if ($_ =~ m/^[#]/) { next; }
            if ($_ =~ m/^gotoLdapServer: ([\S]+?)$/) {
                push(@gotoLdapServer, $1);
            }
        }
        close(PIPE);
    }

    # still no gotoLdapServer found
    if (@gotoLdapServer == 0) {
        &main::daemon_log("ERROR: cannot find gotoLdapServer entry in command: $gosaGroupOfNames_cmd", 1);
        return;
    }

    # sort @gotoLdapServer and then split of ranking
    my @sorted_gotoLdapServer = sort(@gotoLdapServer);
    @gotoLdapServer = reverse(@sorted_gotoLdapServer);
    foreach (@gotoLdapServer) {
        $_ =~ s/^\d://;
    }

    my $t = join(" ", @gotoLdapServer);
 
    my $out_hash = &main::create_xml_hash("new_ldap_config", $main::server_address, $address);
    map(&main::add_content2xml_hash($out_hash, "new_ldap_config", $_), @gotoLdapServer);
    &main::send_msg_hash2address($out_hash, $address);

    return;
}


#===  FUNCTION  ================================================================
#         NAME:  execute_actions
#   PARAMETERS:  msg_hash - hash - hash from function create_xml_hash
#      RETURNS:  nothing
#  DESCRIPTION:  invokes the script specified in msg_hash which is located under
#                /etc/gosad/actions
#===============================================================================
sub execute_actions {
    my ($msg_hash) = @_ ;
    my $configdir= '/etc/gosad/actions/';
    my $result;

    my $header = @{$msg_hash->{header}}[0];
    my $source = @{$msg_hash->{source}}[0];
    my $target = @{$msg_hash->{target}}[0];
 
    if((not defined $source)
            && (not defined $target)
            && (not defined $header)) {
        &main::daemon_log("ERROR: Entries missing in XML msg for gosad actions under /etc/gosad/actions");
    } else {
        my $parameters="";
        my @params = @{$msg_hash->{$header}};
        my $params = join(", ", @params);
        &main::daemon_log("execute_actions: got parameters: $params", 5);

        if (@params) {
            foreach my $param (@params) {
                my $param_value = (&get_content_from_xml_hash($msg_hash, $param))[0];
                &main::daemon_log("execute_actions: parameter -> value: $param -> $param_value", 7);
                $parameters.= " ".$param_value;
            }
        }

        my $cmd= $configdir.$header."$parameters";
        &main::daemon_log("execute_actions: executing cmd: $cmd", 7);
        $result= "";
        open(PIPE, "$cmd 2>&1 |");
        while(<PIPE>) {
            $result.=$_;
        }
        close(PIPE);
    }

    # process the event result


    return;
}

1;
