#!/usr/bin/perl
#
# Igor Muratov <migor@altlinux.org>
#
# Find changes at LDAP and put this to filesystem
#
#
# Igor Muratov <migor@altlinux.org>
# 20041004
# - Added rebuildVirtual function
# 
# Igor Muratov <migor@altlinux.org>
# 20040617:
# - Changed search fiter to exclude gosaUserTemplate entries
#
# Simon Liebold <s.liebold@gmx.de>:
# 20040617:
# - Changed $TS_FILE-location
#
# $Id: goAgent.pl,v 1.4 2004/11/19 21:46:56 migor-guest Exp $ 
#

use strict;
use Net::LDAP;

my $LDAP_HOST='localhost';
my $LDAP_PORT='389';
my $LDAP_BASE='dc=example,dc=com';
#my $LDAP_USER='cn=admin,dc=example,dc=com';
#my $LDAP_PASS='secret';

my $HOME_DIR='/home';
my $TS_FILE='/tmp/gosa_timestamp';
my $KEYS_DIR='/etc/openssh/authorized_keys2';
my $MAIL_DIR='/var/spool/mail';
my $VLOCAL='/etc/postfix/virtual_local';
my $VFORWARD='/etc/postfix/virtual_forward';
my ($ldap, $mesg, $entry);
my $virtuals = 0;

# Anonymous bind to LDAP
sub anonBind
{
	my $ldap = Net::LDAP->new( $LDAP_HOST, port => $LDAP_PORT );
	my $mesg = $ldap->bind();
	$mesg->code && die $mesg->error;
	return $ldap;
}

# Bind as LDAP user
#sub userBind
#{
#	my $ldap = Net::LDAP->new( $LDAP_HOST, port => $LDAP_PORT );
#	my $mesg = $ldap->bind($LDAP_USER, password=>$LDAP_PASS);
#	$mesg->code && die $mesg->error;
#	return $ldap;
#}

# Read timestamp
sub getTS
{
	open(F, "< $TS_FILE");
	my $ts = <F>;
	chop $ts;
	$ts ||= "19700101000000Z";
	return $ts;
}

# save timestamp
sub putTS
{
	my $ts = `date -u '+%Y%m%d%H%M%SZ'`;
	open(F, "> $TS_FILE");
	print F $ts;
}

sub rebuildVirtuals
{
	print "Rebuild virtuals table for postfix\n";
	$mesg = $ldap->search(
		base => $LDAP_BASE,
		filter => "(&(objectClass=gosaMailAccount)(gosaMailDeliveryMode=[*L*])(|(mail=*)(gosaMailAlternateAddress=*)))",
		attrs =>  [
			'mail',
			'uid',
			'gosaMailForwardingAddress',
			'memberUid'
		],
	);

	# Work if changes is present
	open(VIRT, "> $VLOCAL");
	foreach my $entry ($mesg->all_entries)
	{
		foreach my $addr ($entry->get_value('mail'))
		{
			print VIRT "$addr\t";
			print VIRT join(",", (
				$entry->get_value("uid"),
				$entry->get_value("gosaMailForwardingAddress"),
				$entry->get_value("memberUid"),
			));
			print VIRT "\n";
		}
	}
	close(VIRT);
	`postmap $VLOCAL`;

	$mesg = $ldap->search(
		base => $LDAP_BASE,
		filter => "(&(objectClass=gosaMailAccount)(!(gosaMailDeliveryMode=[*L*]))(|(mail=*)(gosaMailAlternateAddress=*)))",
		attrs =>  [
			'gosaMailForwardingAddress',
		],
	);

	# Work if changes is present
	open(VIRT, "> $VFORWARD");
	foreach my $entry ($mesg->all_entries)
	{
		foreach my $addr ($entry->get_value('mail'))
		{
			print VIRT "$addr\t";
			print VIRT join(",", (
				$entry->get_value("gosaMailForwardingAddress"),
			));
			print VIRT "\n";
		}
	}
	close(VIRT);
	`postmap $VFORWARD`;
}

sub posixAccount
{
	my $entry = shift;
	my $uid = ($entry->get_value('uid'))[0];
	my $home = ($entry->get_value('homeDirectory'))[0];
	my $uidNumber = ($entry->get_value('uidNumber'))[0];
	my $gidNumber = ($entry->get_value('gidNumber'))[0];

	print "Update posixAccount: $uid\n";
	`install -dD -m0701 -o$uidNumber:$gidNumber $home`;
	#`install -d -m0700 -o$uidNumber:$gidNumber $home/.ssh`;
	#`install -d -m0751 -o$uidNumber:$gidNumber $home/.public_html`;
	print "\tEntry ".$entry->dn()." updated\n";
}

# Get ssh keys and place to system directory
sub strongAuthenticationUser
{
	my $entry = shift;
	my $uid = ($entry->get_value('uid'))[0];
	open(KEYS, "> $KEYS_DIR/$uid");
	print KEYS $_ foreach ($entry->get_value('userCertificate;binary'));
}

# Create mailbox if need
sub inetLocalMailRecipient
{
	my $entry = shift;
	my $uid = ($entry->get_value('uid'))[0];
	my $mail = ($entry->get_value('mailLocalAddress'))[0];
	my $addr = ($entry->get_value('mailRoutingAddress'))[0];
	my $uidNumber = ($entry->get_value('uidNumber'))[0];
	my $mailbox = "$MAIL_DIR/$uid";

	print "Update inetLocalMailRecipient: $mail\n";
	if( $uid eq $addr )
	{
		if( -f "$mailbox" )
		{
			print "Warning: mailbox $mailbox alredy exists. No changes.\n";
		} else {
			`install -m660 -o$uidNumber -gmail /dev/null $mailbox`;
		}
	}
	print "\tEntry ".$entry->dn()." updated\n";
}

sub disassemble
{
	my $entry = shift;

	foreach my $attr ($entry->get_value('objectClass'))
	{
		if( $attr eq "posixAccount" ) {
			posixAccount($entry);
		} elsif( $attr eq "inetLocalMailRecipient" ) {
			inetLocalMailRecipient($entry);
		} elsif( $attr eq "strongAuthenticationUser" ) {
			strongAuthenticationUser($entry);
		} elsif( $attr eq "gosaMailAccount" ) {
			$virtuals++;
		}
	}
}

#
# Start main process
#

# Read timestamp from file
my $ts = getTS;

$ldap = anonBind;
$mesg = $ldap->search(
	base => $LDAP_BASE,
	filter => "(&(modifyTimestamp>=$ts)(!(objectClass=gosaUserTemplate)))"
);

# Put timestamp to file
putTS;

# Work if changes is present
if($mesg->count > 0)
{
	print "Processing records modified after $ts\n\n";

	foreach my $entry ($mesg->all_entries)
	{
		disassemble($entry);
	}
	rebuildVirtuals if $virtuals;
}
