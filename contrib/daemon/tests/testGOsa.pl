#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  testGosa.pl
#
#        USAGE:  ./testGosa.pl 
#
#  DESCRIPTION:  
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:   (), <>
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  06.12.2007 14:31:37 CET
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;
use IO::Socket::INET;
use Digest::MD5  qw(md5 md5_hex md5_base64);
use Crypt::Rijndael;
use MIME::Base64;

sub create_ciphering {
    my ($passwd) = @_;

    $passwd = substr(md5_hex("$passwd") x 32, 0, 32);
    my $iv = substr(md5_hex('GONICUS GmbH'),0, 16);
    print "iv: $iv\n";
    print "key: $passwd\n";

    my $my_cipher = Crypt::Rijndael->new($passwd ,Crypt::Rijndael::MODE_CBC() );
    $my_cipher->set_iv($iv);
    return $my_cipher;
}

sub decrypt_msg {
    my ($crypted_msg, $my_cipher) = @_ ;
    $crypted_msg = &decode_base64($crypted_msg);
    my $msg = $my_cipher->decrypt($crypted_msg); 
    return $msg;
}

sub encrypt_msg {
    my ($msg, $my_cipher) = @_;
    if(not defined $my_cipher) { print "no cipher object\n"; }
    $msg = "\0"x(16-length($msg)%16).$msg;
    my $crypted_msg = $my_cipher->encrypt($msg);
    chomp($crypted_msg = &encode_base64($crypted_msg));
    return $crypted_msg;
}



my $gosa_server = IO::Socket::INET->new(LocalPort => "9999",
        Type => SOCK_STREAM,
        Reuse => 1,
        Listen => 1,
        );





my $client = $gosa_server->accept();
my $other_end = getpeername($client);
if(not defined $other_end) {
    print "client cannot be identified:";
} else {
    my ($port, $iaddr) = unpack_sockaddr_in($other_end);
    my $actual_ip = inet_ntoa($iaddr);
    print "accept client at gosa socket from $actual_ip\n";
    chomp(my $crypted_msg = <$client>);
    print "crypted msg: <<<$crypted_msg<<<\n";

    my $cipher = &create_ciphering("ferdinand_frost");

    my $msg = &decrypt_msg($crypted_msg, $cipher);
    print "msg: <<<$msg<<<\n";

    print "\n#################################\n\n";

    my $answer = "gosa answer: $msg";

    print "answer: $answer\n";
    
    my $out_cipher = &create_ciphering("ferdinand_frost");
    my $crypted_answer = &encrypt_msg($answer, $out_cipher);
    
    print $client $crypted_answer."\n";

}

sleep(3);
close($client);








