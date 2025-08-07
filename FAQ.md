# GOsa Frequently asked questions

### Q:  When creating many users for one department, I need to fill somefields again and again. Is there a shortcut for that?

A:  Just create a user template and pre-fill all values you need. You
    can use dynamic content, too: uid, sn and givenName will be replaced.
    i.E. an entry `/home/{%uid}` in homeDirectory will be replaced by the
    real uid of the user you're creating, `{%sn[0-4]}.{%givenName}@yourdomain.com`
    creates proper email addresses, etc. Templates include group membership.

---

### Q:  I can see passwords in my logs and in my process list while executing commands, such as postcreate/passwordHook/aso.

A:  The best way to execute scripts with sensitive data is to use envrionmental variables in your scripts, like shown here.
    
An example snippet from the gosa.conf:
    
    
    <plugin name='User password' class='password'
        postmodify="NEWPASSWORD=%new_password /usr/bin/sudo /scripts/myScript.sh"
    
Another example for the password check hook (The passwordHook is deprecated since GOsa 2.7):

    <location
        passwordHook="CURRENTPWD=%current_password NEWPWD=%new_password /usr/bin/sudo /scripts/myScript.sh"

You can then use the variables like this:

    #!/bin/sh
    echo $CURRENTPWD
    echo $NEWPWD

---

### Q:  Can GOsa execute commands 'BEFORE' creating/editing/removing users,departments, etc. 

A:  Yes. Edit /etc/gosa/gosa.conf's menu section. 
    Each plugin may has an entry "preremove", "premodify" and "precreate".  
    You can use ldap attributes as command line options.

i.E. 

    precreate="/usr/bin/sudo /usr/local/sbin/ftp.setperms %uid %givenName"

---

### Q:  Can GOsa execute commands 'AFTER' creating/editing/removing users,departments, etc. 

A:  Yes. Edit /etc/gosa/gosa.conf's menu section. 
    Each plugin may has an entry "postremove", "postmodify" and "postcreate".  
    You can use ldap attributes as command line options.

i.E. 
    
    postcreate="/usr/bin/sudo /usr/local/sbin/ftp.setperms %uid %givenName"

---

### Q:  I'd like to modify the look of GOsa to fit our CI. How can I create anown theme? 

A:  Themes are splitted into two parts. ihtml/ contains templates which
    generate the ui, html/ contains all parts that must be readable from
    clients. GOsa first looks for predefined files in the directory indirectly
    defined via the "theme" parameter in /etc/gosa/gosa.conf. If it can't
    find them here, it'll use the default one.

So start over by copying html/themes/default to html/themes/yourtheme
and ihtml/themes/default to ihtml/themes/yourtheme. Change gosa.conf to
contain theme="yourtheme" in section main. Here are some files to edit:

    * login.tpl         -> login screen
    * framework.tpl     -> page contents
    * style.css         -> stylesheets used by GOsa

Plugins and HTML snippets that produce visual output.

To apply your theme also in the plugins you need to create a new folder
with the name of your theme. Then copy all the content from the default 
folder. It is important that you copy existing .xml files. Now you can 
modify the .tpl files according to your wishes.

To customize code snippets that produce visual output you simple using 
switch statements. 

If there is no switch at the desired position yet.

- Declare a variable for the theme name in your class or in scope of the 
    function if nessecary. 
- Initialize it with the function `getThemeName()` from the include/functions.inc. 
- Move the current code snippet to the default statement and add a statement 
    with the name of your theme.

Otherwise, use the existing switch statement and just add your new statement.

---

### Q:  How can I let a person do administrative tasks under a specific department? 

A:  Since GOsa 2.6, there is a flexible but complex ACL management for managing access 
    rights.

---

### Q:  What about applications? 

A:  GOsa can manage desktop applications in ldap. Create a group and put all users
    in there, which have common desktop settings. Go to the "Application" tab and
    add all applications common to this group. Applications can be created from the
    application plugin.
    The idea behind this feature is a script running on the terminal-servers/
    workstation which check for applications on login (or on a regular basis using
    timestamps). This one will create the corresponding icons on your KDE or GNOME
    desktop.

---

### Q:  What's this terminal stuff? 

A:  GOto is - similar to LTSP - a ldap based diskless client system. It is available
    from our projects page.

---

### Q:  I can't select any mailservers. What's wrong? 

A:  It seems that a mail server is missing in your configuration.
    Create a new server, go to the services tab and add a mailserver 
    service and/or the imap service.

---

### Q:  Can I specify some kind of password policies? 

A:  You can place the keywords "passwordMinLength" and "passwordMinDiffer" in the main
    section of your gosa.conf. "passwordMinLength" specifies how many characters a
    password must have to be accepted. "passwordMinDiffer" contains the number of
    characters that must be different from the previous password.

> Note that these only affect passwords that are set by the user, not by the admins.

---

### Q:  I've to update passwords on external windows PDCs. Can I add a command to letsynchronize these for me? 

A:  There's the possibility to add a hooks in gosa.conf's plugin tags 
    using the "premodify/postmodify" keywords. The specified command 
    will be executed with these additional parameters: 

    * current_password 
    * new_password 
    * userPassword

    <plugin acl="users/password:self" class="password"
     premodify="/scripts/prepareForPasswordChange %current_password %new_password %uid"
     check="/scripts/checkPasswordHash %userPassword %dn"
     postmodify="/scripts/changePassword %dn %uid %userPassword %current_password %new_password %userPassword"
    >

For further information about pre- and post hooks search for the premodify and postmodify statements.

So you can call i.e. smbpasswd to handle your password change on the PDC.

---

### Q:  What about templates for vacation messages? 

A:  Create a directory to keep a set of vacation messages which are readable by the
    user that runs your apache. In this example I'll use /etc/gosa/vacation for that.

Put your vacation files in there containing a "DESC:some descriptive text" as the
first line followed by the normal vacation text. You can use all attributes from
the generic tab. I.e.:

    /etc/gosa/vacation/business.txt 
    
        DESC:Away from desk
        Hi, I'm currently away from my desk. You can contact me on
        my cell phone via %mobile.

     
        Greetings,
        %givenName %sn
     
Place the config option vacationTemplateDirectory="/etc/gosa/vacation" in the location found in
gosa.conf and a template box is show in the vacation mail tab.

---

### Q:  How can I generate automatic ID's for user templates? 

A:  Add an entry describing your id policy in gosa.conf, location section:


1) Using attributes
You can specify LDAP attributes (currently only sn and givenName) in braces {}
and add a percent sign befor it. Optionally you can strip it down to a number
of characters, specified in []. I.e.

        idGenerator="{%sn}-{%givenName[2-4]}"

    will generate an ID using the full surename, adding a dash, and adding at least
    the first two characters of givenName. If this ID is used, it'll use up to four
    characters. If no automatic generation is possible, a input box is shown.

2) using automatic id's I.e. specifying

        idGenerator="acct{id:3}"

    will generate a three digits id with the next free entry appended to "acct".

        idGenerator="ext{id#3}"

    will generate a three digits random number appended to "ext".

---

### Q:  I'm migrating from the current LDAP, now GOsa does not allow uid's and groupwith upper/lower case and spaces. What can I do? 

A:  Include the strictNamingRules="no" keyword in your gosa.conf's location section.
    
> WARNING: using strictNamingRules="no" will cause problems with cyrus/postfix!!

---

### Q:  I'd like to place my users under ou=staff, not under ou=people. Can I changethis? 

Yes. You can change the people and group locations by adding the following
statements to your location sections:

    userRDN="ou=staff"
    groupRDN="ou=crowds"

After logging in again, people and groups are created in the configured places.
As a side note, you can leave these strings blank for flat structures, too.

---

### Q:  I really don't want dn's containing the CN for user accounts because I don't want to support anonymous binds for uid resolution. Is it possible to have dn'scontaining the uid instead? 

A:  Yes. Placing the accountPrimaryAttribute="uid" keyword in your gosa.conf's location
    section will solve your problem.

---

### Q:  Hey, I've installed GOsa, but it claims something about "SID and / or RIDBASE are missing in your configuration". What's wrong? 

A:  You've configured GOsa to use samba3, but your LDAP has no samba domain object
    inside. Either log into samba for the first time to let it create that object,
    or supply the sid and ridbase for your domain in your gosa.conf's location, i.e.:

    <location name=...>
        ...
        sambaRidBase="1000"
        sambaSID="0-815-4711" \>

Remember to fill in your real domain sid which is retrievable by the command
"net getlocalsid".

---

### Q:  We have massive performance problems with using samba as a member server. 

A:  This is a known issue. We're working around this by putting

    <location name=...>
        ...
        sambaIdMapping="true"
        ... \>

into the configuration. GOsa will write the additional objectClass sambaIdmapEntry
to the group and user objects.

---

### Q:  I get 'The value specified as GID/UID number is too small' when forcing IDs. Why? 

A:  This is an additional security feature, so that no one can fall back to uid 0. The
    default minimum ID is 100. You can set it to every value you like by specifying

    <location name=...>
        ...
        minId="40"
        ... \>

in your configuration. In this example 40 will be the smallest ID you can enter.

---

### Q:  Aahhrg. I've updated to a new version and my gosa.conf seems to be broken. 

A:  Some parameters may have changed. Please move your gosa.conf away and re-run the setup.

---

### Q:  I've saved my windows workstations in other locations like GOsa is doing it for decades. Is there a way to change this? 

A:  Yes. Use the sambaMachineAccountRDN parameter in your location section: 

    <location name=...>
        ...
        sambaMachineAccountRDN="ou=machineaccounts"
        ... \>

---

### Q:  I'd like to have TLS based LDAP connections from within GOsa. Is this possible? 

A:  Yes, add

    <location ...>
        ...
              ldapTLS="true"
        ... \>

to the location section of GOsa. This switch affects LDAP connections for a single location only.

---

### Q:  Cyrus folder get created in the style user.username. I prefer the unix hirachystyle user/username. Is it possible to change this? 

A:  Yes, add

    <location
        cyrusUseSlashes="true"

to the location or main section of GOsa and the folders are created in unix style.

---

### Q:  I've a cyrus installation with customized user and folder prefixes. 
    How can I tell GOsa to use the prefixes I prefer? 

A:  Simply set the following attributes in the location tag of your gosa.conf:  

    <location 
        mailUserCreation="myprefix/%mail%"
        mailFolderCreation="myfolder\\.%cn%@%domain%"

The dot in the above example is escaped to prevent it from replacing with '/' if 
cyrusUseSlashes is set to true.


You can use the following replacements:

    %cn%       - The groups cn.
    %uid%      - The users uid.
    %prefix%   - The default prefix used by the mailmethod.
    %uattrib%  - The configured mailAttribute="" (gosa.conf).
    %domain%   - The domain part of the given mail address. (user@domain.com = domain.com)
    %mailpart% - The user part of the mail address. (user@domain.com = user)
    %mail%     - The complete mail address. 

---

### Q:  I want to use cyrus for multiple mail domains, but GOsa uses the 'uid' attribute for account namens, how do I change this to 'mail'? 

A:  Just add/modify the following line to/in your gosa.conf:

    <location 
        ...
        mailAttribute="mail"

---

### Q:  I'd like to do special checks for several plugin parameters. How can I modify GOsa to take care of these checks? 

A:  No need to modify anything. Just add a hook the the plugin you'd like to
    check:

    check="/your/command/binary"

This binary will get an ldif to STDIN for analysis and may write an error message
to STDOUT. Note, that the supplied ldif may NOT be the original target ldif due
to technical reasons.

---

### Q:  Is there a way to use ACL independet filtering when using administrative units? 

A:  Yes. Set "honourUnitTags" to "true" in your gosa.conf's location section.

---

### Q:  How can i active the account expiration code for the gosa interface? 

A:  Yes. Just set "handleExpiredAccounts" to "true" in your gosa.conf's main section.

---

### Q:  What is the correct connection string for a Kolab server in GOsa? 

A:  Try `{localhost:143/novalidate-cert}`.

---

### Q:  Sieve is not working from GOsa - there are authentication problems with this service, IMAP/POP is working. What's wrong? 

A:  Verify that the paramater sasl_auto_transition: no is not present in your imap.conf

---

### Q:  I have a SIEVE problem - "Can't log into SIEVE server. Server  says //. - 

A:  Probably something is wrong with the authentification which is used by timesieved.

    * Check if you can login with "sieveshell" on this host.
    * Also check "telnet localhost 2000" - Is there any output about "Plain Login"?

Please verify the ldap imap attributes, like goImapSieveServer and goImapSievePort. 
These value can be modified using the server->services tab in GOsa 2.6.

Here is an older, but maybe helpful solution for Cyrus-Imapd 2.1.5 on SuSE 9.0:

- Install the "cyrus-sasl-plain" rpm from the distro-cd (This packet contains "sasl2/libplain" library).
- Modify your /etc/imap.conf:

    ```
    sasl_pwcheck_method: saslauthd
    sasl_mech_list: plain login
    ```

- Modify your /etc/sysconfig/saslauthd:

    ```
     SASLAUTHD_AUTHMECH=pam
    ```

---

### Q:  Slapd does not start after adding or changing schema files to the slapd config. What can I do? 

A:  Check the order of how slapd loads the schema files. 
    Order of schema loading matters, because some schemas depend on other
    schemas being already loaded.

---

### Q:  Slapd does not start with kolab2.schema included. It claims that the definition of calFBURL is missing. What can I do? 

A:  For Kolab to work correctly you have to include the `rfc2739.schema`
    in your slapd.conf. Insert it before the `kolab2.schema`.

---

### Q:  New implementations of OpenLDAP seem to require `{sasl}` instead of `{kerberos}` in password hashes. GOsa writes the wrong string. What can I do? 

A:  You can set "useSaslForKerberos" to "true" in your gosa.conf's main section.

---

### Q:  Is there a way to add the personalTitle attribute the the users dn? 

A:  Just add this line into the location section of your gosa.conf.

    <location
    ... 
    personalTitleInDN="true"

---

### Q:  I'd like to assign different uid bases for certain user/group objects.How can this be achieved? 

A:  Use the `baseIdHook` in your gosa.conf's location section to specify a script
    which handles the ID generation externaly. It get's called with the "dn"
    and the attribute to be ID'd. It should return an integer value.

---

### Q:  I'd like to use `rfc2307bis` compliant groups. Is this possible? 

A:  Yes - place the `rfc2307bis="true"` inside of the location section of
    your gosa.conf. Remember, that you can't create empty groups in this mode.

---

### Q:  GOsa sessions expire too quick. Is there a way to change this? 

A:  Yes. Set "sessionLifetime" to the number of seconds of inactivity. 7200
    (60x60x2) would be for two hours. Place this option inside the main
    section of your gosa.conf.

---

### Q:  Is there a way to let users change passwords without logging into GOsa? 

A:  Yes. Browse to "password.php". You can preset a couple of things i.e.:

    http://your.admin.server/password.php?uid=yourUid&method=md5&directory=yourDirectory

---

### Q:  GOsa only shows 300 entries at a time. Is this normal? 

A:  There's a default sizelimit. You can set the "ldapSizelimit" option in your
    gosa.conf's  location section to a higher value to get rid of it.

---

### Q:  I have problems with my ldap server when I open groups with a huge amount of members, what can I do? 

A:  You can set a nesting limit which ensures that the user names will not be 
    resolved if the amount of members reaches this limit.

    <location
        ...
        ldapFilterNestingLimit="100"

---

### Q:  I want to disable the "Is the configuration file up to date?" check when logging in.
    How can I disable this check? 

A:  Just set the configVersion attribute to an empty value:

    <conf configVersion="" >

---

### Q:  I've shredded my access control and am not able to do anything from now on. 
    Is there a way to override the ACL? 

A:  Yes. Insert the following statement in the location section of your gosa.conf:

    ignoreAcl="your user's dn"

---

### Q:  I can't logon as Administration, what is wrong? 

A:  It looks like you are missing an administrativ account. 
    In newer versions of GOsa you can simply re-run the setup and create 
    an admin account on the migration page. 

> Additionally you can set ignoreACL in GOsa 2.6, just search the FAQ.

---

### Q:  The Unix's user's shell list is empty (unconfigured) 

A:  Just copy or link your /etc/shell in /etc/gosa.

---

### Q:  After upgrading GOsa, the setup.php doesn't work or looks broken.  

A:  You should delete all files in /var/spool/gosa

    # cd /var/spool/gosa
    # rm -rf *

---

### Q:  After installing GOsa using an existing LDAP tree, my user accounts are not listed. 

A:  You need to add the following objectClasses to your accounts:

    objectClass: person
    objectClass: organizationalPerson

The setup will automatically migrate those accounts, see migration step in GOsa setup!

---

### Q:  Is it possible to login with the users mail address too?  

A:  Yes, just add the following line to your gosa.conf:

    <location 
        ...
        loginAttribute="mail"

or for both, uid and mail:

    <location 
        ...
        loginAttribute="uid,mail"

---

### Q:  GOsa doesn't seem to follow my referrals. What can I do? 

A:  Place the option `ldapFollowReferrals = "true"` inside your locations definition
    and you should be fine.

---

### Q:  GOsa is not in my native language, can I translate it to my language? 

A:  Yes. Just go to the locale directory and copy the messages.po file somewhere
    else. (Translation is splitted into core and plugin translation files). 
    Edit the copy and put your translations into the msgstr lines. To be
    included in next GOsa releases, you may want to send it to the GOsa maintainer.
    Finally you need to create a directory with your language code. (i.e. de for
    german) containing the LC_MESSAGES directory. Move your messages.po file there
    and run 'msgfmt messages.po' in that directory. That's it.

You may need to restart apache, depending on your setup. On Debian, be sure
to have your locale generated (dpkg-reconfigure locales) before.

---

### Q:  I've problems with a user being a member in more than 16 groups when accessing files on NFS mounted volumes. What's wrong?

A:  Make sure you're using `"--manage-gids"` to start the rpc.mountd.

---