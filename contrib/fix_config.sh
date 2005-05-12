#!/bin/sh

CONF=/etc/gosa/gosa.conf
cat << EOF

This script will take your current $CONF and will update the menu and tab tags. It
will not touch the parts you have touched between the main tabs.  A backup copy is
created.

EOF
read -n1 -sp "Do you want to continue? (y/n)" answer
echo

if [ "$answer" != "y" -a "$answer" != "Y" ]; then
	echo "Aborted."
	echo
	exit 1
fi

if [ ! -f $CONF ]; then
	echo "Can't find $CONF Aborted."
	echo
	exit 1
fi

cp $CONF $CONF.orig

main=$(sed -n '/<main/,/\/main>/p' $CONF)

cat << EOF > $CONF
<?xml version="1.0"?>
<conf>
        <menu>
                <section name="My account">
                        <plugin acl="default" class="user" icon="personal.png"
                                path="plugins/personal/generic" />
                        <plugin acl="default" class="posixAccount" icon="posix.png"
                                path="plugins/personal/posix" />
                        <plugin acl="default" class="mailAccount" icon="email.png"
                                path="plugins/personal/mail" />
                        <plugin acl="default" class="sambaAccount" icon="samba.png"
                                path="plugins/personal/samba" />
                        <plugin acl="default" class="proxyAccount" icon="proxy.png"
                                path="plugins/personal/proxy" />
                        <plugin acl="default" class="pureftpdAccount" icon="ftp.png"
                                path="plugins/personal/pureftpd" />
                        <plugin acl="default" class="gofaxAccount" icon="fax.png"
                                path="plugins/gofax/faxaccount" />
                        <plugin acl="default" class="phoneAccount" icon="phone.png"
                                path="plugins/gofon/phoneaccount" />
                        <plugin acl="default" class="password" icon="password.png"
                                path="plugins/personal/password" />
                </section>

                <section name="Administration">
                        <plugin acl="user" class="userManagement" icon="user.png"
                                path="plugins/admin/users" />
                        <plugin acl="group" class="groupManagement" icon="group.png"
                                path="plugins/admin/groups" />
                        <plugin acl="ogroup" class="ogroupManagement" icon="ogroup.png"
                                path="plugins/admin/ogroups" />
                        <plugin acl="department" class="departmentManagement" icon="department.png"
                                path="plugins/admin/departments" />
                        <plugin acl="application" class="applicationManagement"
                                icon="application.png" path="plugins/admin/applications" />
                        <plugin acl="blocklists" class="blocklist" icon="blocklists.png"
                                path="plugins/gofax/blocklists" />
                        <plugin acl="system" class="systems" icon="system.png"
                                path="plugins/admin/systems" />
                </section>

                <section name="Addons">
                        <plugin acl="default" class="addressbook" icon="addressbook.png"
                                path="plugins/addons/addressbook" />
                        <plugin acl="default" class="faxreport"  icon="reports.png"
                                path="plugins/gofax/reports" />
                        <plugin acl="default" class="fonreport"  icon="phonereport.png"
                                path="plugins/gofon/reports" />
                        <plugin acl="logs" class="logview" icon="logview.png"
                                path="plugins/addons/logview" />
                        <plugin acl="ldif" class="export" icon="ldif.png"
                                path="plugins/addons/ldifexport" />
                </section>
        </menu>

        <usertabs>
                <tab class="user" name="Generic" />
                <tab class="posixAccount" name="Unix" />
                <tab class="mailAccount" name="Mail" />
                <tab class="sambaAccount" name="Samba" />
                <tab class="proxyAccount" name="Proxy" />
                <tab class="pureftpdAccount" name="FTP" />
                <tab class="gofaxAccount" name="Fax" />
                <tab class="phoneAccount" name="Phone" />
                <tab class="reference" name="References" />
        </usertabs>

        <grouptabs>
                <tab class="group" name="Generic" />
                <tab class="appgroup" name="Applications" />
                <tab class="mailgroup" name="Mail" />
                <tab class="acl" name="ACL" />
                <tab class="reference" name="References" />
        </grouptabs>

        <appstabs>
                <tab class="application" name="Generic" />
                <tab class="applicationParameters" name="Options" />
                <tab class="reference" name="References" />
        </appstabs>

        <termtabs>
                <tab class="termgeneric" name="Generic" />
                <tab class="termservice" name="Devices" />
                <tab class="termstartup" name="Startup" />
                <tab class="terminfo" name="Monitoring"
                        wakecmd="/usr/bin/sudo /usr/sbin/etherwake" />
                <tab class="reference" name="References" />
        </termtabs>

        <worktabs>
                <tab class="workgeneric" name="Generic" />
                <tab class="termservice" name="Devices" />
                <tab class="termstartup" name="Startup" />
                <tab class="terminfo" name="Monitoring"
                        wakecmd="/usr/bin/sudo /usr/sbin/etherwake" />
                <tab class="reference" name="References" />
        </worktabs>

        <printtabs>
                <tab class="printgeneric" name="Generic" />
                <tab class="reference" name="References" />
        </printtabs>

        <phonetabs>
                <tab class="phonegeneric" name="Generic" />
                <tab class="reference" name="References" />
        </phonetabs>

        <deptabs>
                <tab class="department" name="Generic" />
                <tab class="reference" name="References" />
        </deptabs>

        <ogrouptabs>
                <tab class="ogroup" name="Generic" />
                <tab class="reference" name="References" />
        </ogrouptabs>

$main

</conf>
EOF
