## gosa 2.8


### Bug Fixes

* Fixed delete functions in all table widgets.
* Fixed deprecated warnings for php v7.4.
* Fixed unexpected behavior when switching the list view multiple times.


### Features

* Add new GOsa Logo.
* Add simplified template system for plugins and HTML snippets.
* Added global function for handling HTML snippets that can be customized to the theme.
* Added new default theme for GOsa-core and all plugins. The old theme can be found under classic.
* Add account menu for the active user in the new default theme.
* Added new translations for the new theme.
* Change of password verification to give better feedback to the user.
* Modification of visible elements such as buttons etc to improve robustness.
* Merged .tpl files that were duplicated.

## gosa 2.7.5


### Bug Fixes

* Fixed problem that occured when certain POST variables get transferred in a wrong way.
* Fixed an issue with the LDAP special character detection that just worked with OpenLDAP.
* Fixed two false positive messages that occur when renaming groups.


### Features

* Made samba primary group selectable for users.

## gosa 2.7.4


### Bug Fixes

* Fixed problem in setup checks that seem to access a bool value by string index.
* Fixed problems with sieve login and TLS.
* Fixed ACL resolution for login restrictions.
* Fixed pronlems with unsaved user pictures.


### Features

* Robustness improvements for PHP 5.3/5.4 compatibility.
* Made asterisk delimiter configurable.
* Enabled template mode even when editing a template.
* Do not create primary groups for templates.
* Allow to modfiy the mail address when cyrus is used as mail method and accounts are identified by uid.
* Added SASL password method.

## gosa 2.7.3


### Bug Fixes

* Fixed some listing problems.
* Fixed problems with $ in samba-hash-, check-, pre- and posthooks.
* Fixed problem with password changes beeing forced on login for newly created user.
* This fixed a bug where properties stored in gosa.conf did not take effect.
* Fixed group membership adaption when creating a user using a template.
* Fixed removal of FAI profiles, hooks, scripts.
* Fixed progressbar reload in fai installation status plugin.
* Fixed several opsi plugin bugs.


### Features

* Made DHCP plugin overview show all hosts.
* Added checks to ensure that valid hostnames/fqdns are created.
* Re-added statusbar to fai management plugin.

## gosa 2.7.2

### Bug Fixes

* Fixed problem with mail-method parameters that were read from ldap.
* Fixed removal of FAI-template and profile entries.


### Features

* Updated bundeled smarty to 3.1.4.
* Introduced sortable listing to ACL and posix dialogs.
* Added flag to allow modification of generated uid proposals.
* Improved dyngroup to not write dynamic values back to the ldap.
* Hardening: Replaced in_array calls with a method that uses a strict matching.
* Updated samba password hashing and its error handling.
* Updated Kolab tab, to use a sortable list for mynetworks entries.
* Updated cleansing of FAI object in ldap, thanks to psc.

## gosa 2.7.1


### Bug Fixes

* Fixed acl resolution to be case insensitive.
* Fixed mail address check for user templates.


### Features

* Updated passwordHook behaviour
* Readded sambaKickoffTime to samba tab.
* Updated account expiration checks.
* Updated integrated smarty to 3.0.7

## gosa 2.7


### Bug Fixes

_None documented_


### Features

* Updated design. CSS3, mostly w3c conform.
* Updated and fixed english wording.
* Rewrote filtering methods, allows to define self-made filters.
* Rewrote reference tabs, it now resolve the users acls.
* Replaced all lists by either listing or sortable_listing-
* Added dynamic groups to core functionality. Thanks to <Thomas Chemineau>
* Added property editor plugin, including property validation and migration.
* Rewrote schema check, more flexible and only checks for required classes.
* Stripped setup to minimum requirements. (Use property editor form fine-tuning).
* Introduced dynamic path-menu, replaces the old 'MyAccount' section.
* Added premodify, precreate and preremove events to be able to act before things get saved.
* Hardened handling of sepcial chars in user inputs.
* Added a wildcard ACL to allow to given ACLs to 'everyone'
* Disallow UTF-8 characters in passwords.
* Switch to the new version of the smarty template engine, version 3.
* Updated the ldap export plugins to use the cli, which is much faster.
* Intruduced a new image rendering based on sprites, increases performance.
* Added automatic logout, for security reasons.
* Allow to use replacements like {%var[n-m]} in pre and post events and user templates.

## gosa 2.6.9


### Bug Fixes

* Fixed problem with initial phone accounts, sip type was NULL
* Fixed TLS issues in the setup
* Fixed locale issues in the setup
* Fixed delete of DHCP statements
* Fixed DHCP authoritative flag


### Features

* Modified sieve behaviour to use sieve-discard for "drop_own_mails"
* Modified filter descriptions to behave more like former 2.6 releases

## gosa 2.6.8


### Bug Fixes

* Fixed gosa.conf system generation
* Fixed phone filters


### Features

* Base selector displays descriptions and icons now
* Base selector autocomplete
* Added wildcard ACLs
* Improved ACL handling for new lists

## gosa 2.6.7


### Bug Fixes

* Fixed generation of kerberos host keys when DNS is enabled
* Fixed template error message when no sshkey plugin is enabled
* Fixed problem where arrays got shortened by one in copy and paste
* Fixed javascript for IE
* Fixed problems deprecated functions in PHP > 5.3
* Fixed issues with templates and password methods that got introduced in 2.6.6 user list migration


### Features

* Added more information to DHCP service dialog (thanks to Mathieu)
* Added more checks for DNS zone records
* Added IMAP folder autocreation
* Added printer css
* Speed up OPSI operations by large factors
* Speed up group and user removal by large factors
* Unified logging with rSyslog plugin and service
* Converted to new configurable list view
* Save list positions
* Updated integrated smarty to version 3.0b7 to avoid broken templates during further development

## gosa 2.6.6


### Bug Fixes

* Fixed display of opsi hosts in the system list
* Fixed usage of the sambe "base" for every list page
* Fixed automatic base settings for new systems based on the ogroup base
* Fixed setup issues with LDAP inspection
* Fixed setup issues with sub dialogues
* Fixed next group ID generation
* Fixed "suggest IP" function to return unique IP
* Fixed multiple password decryption
* Fixed issues with empty ACL definitions
* Fixed lost attributes when using copy and paste with objectgroups
* Various minor bugfixes


### Features

* Added ssh public key management
* Added LVM support to FAI modules using setup-storage
* Added DHCP/DNS options to the OPSI hosts
* Added GOsa login restrictions
* Added "Domain" to department management
* Added organizational roles management
* Added option to configure user DNs
* Added OPSI license management
* Added possibility to add FAI packages without the SI service (thanks to GLG)
* Added IMAP timeout option
* Added pool method for uidNumber/gidNumber using sambaUnixIdPool
* Added workaround for non closable message dialogues in Konqueror 3.x
* Updated locales
* Updated gosa.conf manual page
* Updated DNS settings dialoges
* Moved to unified datepicker for all dates
* Enhanced addressbook plugin ACL
* Removed outdated samba 2 mode

## gosa 2.6.5


### Bug Fixes

* Fixed config generation with sambaMachineAccountRDN
* Various minor bugfixes


### Features

* Updated locales
* Improved PPD handling
* Added more sanity checks in group management
* DHCP now allows multiple values for options and statements containing the same keyword
* Added per entry SOA record to the DNS plugin
* Improved grouping of FAI classes
* Add free-text input to FAI packages
* PPD upload allows special chars now
* Added compression robustness check for snapshots
* Added brasilian portuguese
* Updated logging on debian systems

## gosa 2.6.4


### Bug Fixes

* Fixed issue with the FAI log viewer
* Correct issues with newly created servers and DNS zones
* Corrected js based disabling of widgets in Samba settings
* Fixed issue with wrong set mail attributes in the setup
* Fixed uid autogeneration which contained curly braces
* Fixed FAI package list acl mapping
* Fixed ACL resolution for small ldap bases


### Features

* Added missing images
* Made DNS TTL settings optional
* Added setup check for gosaDepartments at the base object
* Added missing existance checks for group mail addresses
* Moved essential schema files back to the core package
* Updated integrated smarty
* Added imagick dependency to the setup
* Added phoneNumber limitations to match asterisk realtime extension tables
* Updated addressbook permissions

## gosa 2.6.3


### Bug Fixes

* Fixed several issues, occurred when using user templates
* Fixed problem with department creation, since GOsa 2.6.1
* Fixed exponential escaping of special chars in dns record entries


### Features

* Added samba domain information to samba tab
* Removed readonly attributes from samba tab

## gosa 2.6.2


### Bug Fixes

* Removed obsoleted GOlab mail method


### Features

* Updated german locales
* Updated french locales
* Updated spanish locales
* Improved sieve handling, added more detailed error messages
* Mail methods cleanup for kolab 2.2 / IMAP acls
* Optimized system to group assignments when activating new devices
* Add a choice for OPSI clients when activating new devices
* Setup cleanups
* Edit locking improvements, code cleanup

## gosa 2.6.1


### Bug Fixes

* Make ACL editor more robust
* Fix problem with self modify ACL detection
* Fixed problems with sieveManagement
* Fixed issue with department saving and structuralObjectClass errors
* Fixed problem with sudoers creation
* Fixed accidentally remove of non GOsa user accounts
* Fixed detection issues with password methods
* Fixed issues with user templates and storing password methods


### Features

* Build seperate packages for plugin schema files
* Increased all schema version numbers to make setup check work again
* Added admin account migration to setup
* Session expired dialog is now translated
* Added local delivery flag to group mail
* Simplified FAI management
* Removed forced dependency on GOsa-SI

## gosa 2.6


### Bug Fixes

_None documented_


### Features

* Redesign of ACL handling
  * Roles
  * Fine grained read, write, change controls down to the attributes
  * Self service
* Separate password changer module
* Major dialog- and interface redesign
* Major performance improvements
* New backend daemon
  * MIT kerberos support
  * Mail queue-support
  * System depolyment queue support
  * OPSI support
  * GOto support
  * DAK keyring support
  * Caching of slow data sources
* Queue support for system deployment
* Improved FAI integration
  * Queues
  * Copy on write storage for subreleases
  * Logviewer
* OPSI integration
* SUDO support
* Split into GOsa core and GOsa plugins
* Edit multiple users/groups at a time
* Apply user templates afterwards
* Objects snapshots
* Configurable object RDNs
* Added manual page for gosa.conf
* More comprehensive keywords in gosa.conf
* Merging "main" and "location" attributes to allow site wide defaults
* Automatic configuration reloading
* Vietnamese translation
* New manageable objects country, locality, organization and dcObject

## gosa 2.5.16


### Bug Fixes

* Fixed problem with undefined ridbase in domain objects
* Fixed postremove/-create/-modify parameter expansion
* Fixed department tagging
* Fixed DNS record problem where nSRecords were not kept


### Features

* Updated integrated smarty to 2.6.19
* Updated saving of ppds in printer setttings
* Updated shared folder acls, keep manually set acls
* Remove DNS entries for removed hosts
* Increased setup performance
* Added more secure way to save passwords in gosa.conf

## gosa 2.5.15


### Bug Fixes

* Fixed problem with saving vacation message start/stop
* Fixed sorting of releases
* Fixed storage of (IMAP) ACLs in group/mail settings
* Fixed issues with sieve script management
* Fixed issue with undefined index and copy 'n paste in samba plugin
* Fixed problem with the references/ogroup handling with special character DNs
* Fixed renaming of printers which was broken in special cases


### Features

* Changed order of sys-action commandline parameters
* Changed sorting of management plugins to natural sorting
* Updated translations (de/fr/pl)
* General code cleanup
* Optimized way of writing unit tags
* Online manual updates / helpviewer update
* DNS plugin updates
* DHCP plugin updates

## gosa 2.5.14


### Bug Fixes

* Old phone numbers will be removed correctly from asterisk extensions table.
* Fixed saving of A/B networks reverse zone entries
* Fixed problem with non loaded class_dhcpPlugin in some cases.
* Fixed non ISO display of IMAP folder names
* Fixed URL encoding in addressbook
* Fixed issues with IE and browser language detection
* Fixed removing of application categories
* Fixed saving of USB devices
* Fixed saving of IMAP acl's for groups


### Features

* Phone conference language is now selectable.
* Added fglrx driver to driver list
* Added hook to specify custom X drivers
* Added check for used hardware adresses
* Allow special characters in share names
* PPD handling revised
* Added support for new devices detected by the arp monitor
* Grey out non used options for better usability in the server and workstation tabs
* Made SNMP community configurable
* Added login attributes uid/mail, you can log in via your mail address, too
* Removed PHP5 dependency in branch 2.5

## gosa 2.5.13


### Bug Fixes

* Fixed autouid problem with slashes


### Features

* Re-added ISC DHCP support
* Fixes for the mail based bugtracker
* Added list sorting for FAI script lists
* Added copy'n paste for mimetypes
* Cut'n paste objects are now greyed out
* Added swedish locale
* Improved language detection
* Added a statistic footer to lists
* Added the ssh plugin
* Layout fixes

## gosa 2.5.12


### Bug Fixes

* Fixed problems with automatic reverse zones
* Fixed several IE6 related Java-Script problems
* FAI partition ordering fixed, partition sizes fixed
* Fixed installations that fail the schema check
* Made gidNumber be the current in posix check hook
* Removed inconsistency in gosa/gosa+samba3 schema
* Fixed multiple saving of "My account" data
* Don't allow moving of objects from administrative units to other administrative units where ACL's permit it. Objects "seemed" to disapear because the tagging changes.


### Features

* Removed png.js by default. Looks ugly, but performs. Take a look at the FAQ on how to re-enable it for IE.
* Added non-login password change dialog
* Various spelling fixes
* Added some extra robustness to the PPD reader code
* FAI release management updates
* Updated error messages to fade out the interface
* Repository cleanup
* Added feedback link to easily report PHP errors
* Added more content sorting where needed
* Added gosa-desktop package to be able to start it by link
* Added method to highlight tabs
* Generel translation update for de, es, fr, it, nl, pl, ru, zh

## gosa 2.5.11a


### Bug Fixes

* Fixed language detection and removed line wraps in tab headers
* Fixed french translation


### Features

* Added chinese translation

## gosa 2.5.11


### Bug Fixes

* Add workaround for failing is_php4() when using PHP5 with "zend.ze1_compatibility_mode" set to "On"
* Fixed double loaded pages in gecko based browsers when js is activated


### Features

* Backported new sieve filter editor from trunk
* Backported new setup from trunk
* Replaced a set of PHP <? short tag occurences
* Updated locales (de/fr)

## gosa 2.5.10


### Bug Fixes

* Fixed non-saved Samba-Domain changes in groups
* Fixed problem with removing commata based DN's
* Corrected setup generated perl mkntpasswd string
* Fixed month listing in fax reports - february was march
* Fixed acl's for saving printers
* Fixed saving of disabled samba acl's


### Features

* Included hook to make use of dynamic uid-bases
* Included vacation date range specification
* Freezed application parameters are not editable anymore
* Enabled 9 digits for gid-/uidNumbers
* Added support for rfc2307bis compliant groups

## gosa 2.5.9


### Bug Fixes

* Fixed ldap tls connections when schema check was being used
* Fixed a problem with cut'n paste for groups and ogroups
* Fixed object tagging for workstation/printer assignement
* Fixed undefined index in FAI package lists
* Fixed copy'n paste for groups and object groups
* Fixed "none" permissions in IMAP shared folders to be assignable


### Features

* Updated italian translation
* Added the possiblility to choose a RDN style for DN's when a CN is already in use
* Added new mail method "golab" which has some tweaks against the standard kolab mode
* Enabled non ASCII characters in vacation messages

## gosa 2.5.8


### Bug Fixes

* Fixed problem with winstations shown in user list.
* Fixed date of birth and shadow expire in template adaption
* Security fix, that allows non priviledged users to change several settings - including admin passwords


### Features

* Allow basic regex ().*^$ in fax blocklist numbers.
* Updated user mail account to search for CYRUSUNIXSTYLE in all relevant sections of the config file
* Added support for sambaLogonHours

## gosa 2.5.7


### Bug Fixes

* Fixed login.tpl to display error msgs in the middle of the screen
* Fixed some error outputs in login.php to not break the screen
* Fixed problems with workstations when fai.schema was not included
* Fixed saving of terminals printer service attribute
* Fixed a ACL naming bug, that avoids that an admin with non "all" ACLs can edit specified objects.
* Fixed simultaneous move + rename for deparments


### Features

* Added auto scroll function to FAI-Create-Branch and Department tagging
* Made gid-/uidNumbers 32 bit aware
* Replaced hardcoded config path /etc/gosa with CONFIG_DIR constant
* Included personal title in DN
* Added function to remove PPD's from disc
* Removed old cups dependencies
* Internally updated to smarty 2.6.16
* Removed asterisk status view in user display. This was too slow in bigger installations.
* Re-enabled phone queues. They got optimized for invisibility.

## gosa 2.5.6


### Bug Fixes

* Fixed malformed usage of $this>var in samba class.
* Fixed checkbox selection in samba class.
* Fixes various issues with setup.php
* Activated missing checks for IP and MAC
* Fixed copy'n paste errors for netatalk
* Various W3C fixes
* Fixed "My Account" mode, where buttons disappear after saving
* Fixed day of birth problem in M$ IE
* Fixed setting of Kerberos passwords


### Features

* Copy & paste implemented into FAI
* Added setup fix to support GraphicsMagick
* Added several fixes for all user plugins to support Copy & Paste.
* Connectivity netatalk: Moved plugin intialization from execute() to contructor().
* Avoid tab lables to have line feeds
* Avoid removal of shares while they are used by users
* Added finer grained ACL settings for mail accounts

## gosa 2.5.5


### Bug Fixes

* Resolved problem with infinite loop while storing sieve scripts
* Fixed "missing PPD" configuration error, for newly created printer
* Corrected problem where the object base was sometimes broken when saving object groups
* Fixed saving of terminal attribute gotoLpdEnable to contain "yes" instead of "1"
* Fixed a set of W3C problems
* Fixed multiple savings in addressbook (Closes: #23)
* Fixed shadow expire when using templates (Closes: #20)


### Features

* Added remove method for shared folder in kolab mode
* Added checkbox to decide if the shared folder should be deleted from IMAP if the mail extension is removed from group mail account
* Updated request method for mail folders
* Added subsearch checkbox to object group "add items" filter
* Avoid reset of several attributes from workstations when not inherited from object groups
* Show error messages from password dialog
* Made %uid, %sn, etc. available in templates using gosaMailAlternateAddress

## gosa 2.5.4


### Bug Fixes

* Applied fixes for logviewer done by Mario Minati
* Updated locales, fixed a set of missing strings
* Fixed problems in FAI list handling
* Fixed saving of user logon scripts
* Fixed non-FAI application mode
* Bug while saving FAI partitions fixed
* Don't save PPD if none is not selected bug fixed
* Saving of non revisioned applications fixed


### Features

* Included patch to choose the addressbook base
* Added "uid" to personal plugins for replacement in post events
* More speed fixes applied, especially for users, objectgroups and generic plugin loading

## gosa 2.5.3


### Bug Fixes

* Fixed problem in reloading departments when we've PHP4
* Fixed gotoPrinter membership problem.
* Fixed environment shares, only available shares will be displayed (gosaUnitTag was ignored)
* Fixed saving of inherited workstation settings
* Removed error when no FAI repositories were present
* Fixed posix group add dialog, filter wasn't working.
* Fixed get_printer_list undefined index warnings while editing a user.
* Fixed ogroup non-static method error
* Fixed user membership for gotoPrinter, if membership was edited via user environemnt, some numeric values were stored too
* Fixed mail account, mail server string possibly was an array
* Fixed typos
* Fixed upper/lowcase ou's for groups/people when using an unclean LDAP database
* Fixed ACL handling to *not* show the admin user dialog when configured for self modify only
* Fixed problem when changing passwords via "My account"


### Features

* Added more information to hotplug devices.

## gosa 2.5.2


### Bug Fixes

* Fixed current main base not beeing set when editing non tabbed plugins
* Fixed filtering for divlists
* Fixed deletion of shares in environment tabs
* Fixed display of FAI partitions
* Removed Quota warnings for existing accounts without quota limits
* Worked around PHP4 session problems when creating new departments
* Fixed problems when moving around departments including a comma
* Avoid the try of creating already existing ou's
* Fixed non working printer removal


### Features

* Updated french online help
* Updated german online help
* Unified bool values in gosa.conf. true/yes and false/no are valid now in upper and lower case.

## gosa 2.5.1


### Bug Fixes

* Fixed problems with NFS shares and terminals
* Fixed problem with compressed gosa.conf in the debian package


### Features

* Finalized polish translations

## gosa 2.5


### Bug Fixes

* Improved robustness while operating whith the LDAP
* Several Kolab related fixes
* Fixed PPD parsing for several commercial PPD's
* Several css fixes
* Fixed series of bugs that lead to not shown groups


### Features

* Improved FAI support
  * Server and workstations are treated the same way
  * Destination selector for new devices
  * Summary tab introduced
* Tagging of departments introduced
* Global check hooks allow user defined testing of single plugins
* Major speedups with large databases
* Added english and french online help
* Unified plugin "head" selectors, (re-)added subtree support
* Tune LDAP error messages
* Moved from "guru mediation style" to div-popups

## gosa 2.4


### Bug Fixes

* Fixed templating for samba and unix users
* Applied hundreds of smaller bugfixes


### Features

* Updated layout to work cleanly with IE6+, Firefox 1.0.4+, khtml 3.4+
* Added FAI (Fully Automatted Installation) support
* Added mail queue management
* Added many missing acl informations
* Added help browser and initial french help
* Improved speed by switching to directory style dialogs and performing sub searches.
* Per user language selector in generic tab
* New connectivity plugins (PHPscheduleit/PPTP/glpi)

## gosa 2.4beta3


### Bug Fixes

* Fixed application removal
* Several kolab related fixes for server objects
* Corrected contributed slapd.conf
* Fixed kolab mode where GOsa saves KB quotas, interprets quotas as kolab MB
* Fixed IE issues with W3C compatibilty where IE posts disabled fields
* Fixed problems with existing samba accounts and password changed fields
* Removed login problems with undefined ldap_conf variable
* Fixed problems where the GECOS field is not written correctly


### Features

* Updated layout
* Improved accessibility for disabled persons
* Added intranet account to list of connectivity plugins
* Increased robustnes for non set fields

## gosa 2.4beta2


### Bug Fixes

* Fixed error handler to be PHP 4.x compatible
* Fixed PHP compatibility problem in setup.php, using ini_get() instead of ini_get_all()
* Fixed cases where ipHostAddress is required but not checked by GOsa
* Fixed group dialog filters
* Fixed problems in setup which showed up with white pages if PHP has been compiled without mbstring support
* Fixed layout if the rendered page does not cover 100% of the browser window


### Features

* Improved phone plugin to respect IAX, CAPI and SIP phone attributes automatically if the revision changes
* Improved W3C compatibility
* Added checks that remove the contents of /var/spool/gosa/*
* Added postmodify for password change operations

## gosa 2.4beta1


### Bug Fixes

* Don't shred samba group ID's if they are not present in the combobox
* Samba3 bugfixes for munged dial handling
* Fixed index ruler for long lists


### Features

* Override automatically detected user bases if they don't exist
* Updated smarty to version 2.6.9
* Updated GOfon support to handle new features
* Replacement of most external programm calls
* Updated LDIF export
* Improved setup checks to find more possible errors
* Completed system creation for servers, phones and misc components
* Added support for kolab users and kolab server settings
* Added server settings
* Added LDIF import
* Added CSV import
* Added italian translation (thanks to Alessandro Amici)
* Added subtree search checkbox in lists with potential higher usage
* Added version indicator to make support more easy
* Added sample databases for fax, phone and system logging
* Added error handler for normal PHP errors

## gosa 2.3


### Bug Fixes

* Fixed french translation
* Fixed error in shadowExpire attribute
* Fixed sample configuration files to be unproblematic when used in conjunction with OpenLDAP 2.2
* Worked around a possible problem with sizelimit in php-ldap
* Various smaller fixes
* Fixed SELECT queries to be mysql 3.x _and_ 4.x compatible


### Features

* Updated smarty to version 2.6.7
* Added dutch translations (thanks to Niels Klomp)
* Added webdav and phpgroupware accounts
* Unified all filters in dialogs to use the internationalized choosers
* Added option to do non subtree searches with filters
* Added experimental support for editing LDAP trees that contain referrals
* Updated Altlinux contributions, including themes and scripts
* Improved big ldap support by size limits and non sub searches
* Added global TLS switch for LDAP connections
* Made departments movable

## gosa 2.2


### Bug Fixes

* Told IMAP plugin to remove mail accounts when the user is deleted


### Features

* Removed DHCP/DNS plugins, they will be replaced by the terminal/server/workstation plugins.
* Added case sensitivity check for login names
* Made bases set to users "home" department when creating new objects
* Moved sieve-*.txt config files to /etc/gosa
* Interface cleanups
* Added simple log file viewer
* Added support for asterisk
* Included javascript magic to improve usability (doubleclicks in lists, disabling of fields, warning messages, etc.)
* More filtering and sizelimits for speed optimizations
* Mail handling is now pluggable
* Added possibility to bundle objects to object groups
* Added a reference tab to track relation ships of different objects
* Improved samba 3 support (terminal server support)
* Updated translations and added a french one

## gosa 2.1.3


### Bug Fixes

* Fixed problem with initial password setting
* Add a workaround to fix problem with groups not beeing displayed with openldap. Here the server reacts with empty results if searching for non existing objectClass "sambaGroupMapping" in case of using samba2
* Fix the homeDirectory check which is a bit too harsh with templates


### Features

* Increase number in version.inc

## gosa 2.1.2


### Bug Fixes

* Fixed problem with uppercase login names
* Fixed gettext problem on older installations
* Corrected sieve login which was broken due to a library switch
* Made in_array act case insensitive for is_account check
* Fixed location of DMODE and HASH in config file
* Fixed general problems with password hash generation if not specified
* Filtering fixes


### Features

* Extensive speed increasements in ldap searches
* Complete move to unicode which removes all active encoding/decoding of contents from GOsa itself
* Made GOsa run smooth on PHP 5
* Added complete russian translation contributed by Igor Muratov
* Migrated phone list to (global) addressbook

## gosa 2.1.1


### Bug Fixes

* Fixed upper/lower case problem in mail templates
* Fixed typo in generic plugin error message
* Made template dialog work again
* Fixed headpage for application management which tends to do no proper display of used applications
* Fixed possible case problem with is_account
* Made base selector contain newly added departments in department dialog


### Features

* Enabled mail-account-less fax accounts
* Added command line interface to use GOsa without web interface
* Updated debian control to be aware of apache2 based installations
* Transferd tab variables in group dialog, so the primary mail address can be checked

## gosa 2.1


Bugfix release

### Bug Fixes

* size of homeDirectory attribute increased
* FAQ/README/INSTALL updated
* spec file updated

## gosa 2.1rc2


Bugfix release

### Bug Fixes

* Fixed memory usage check
* Fixed size of alternate mail address field
* Fixed sorting of group in posix tab
* Made GOsa keep group membership even if user has no posix account
* Fixed typo in blocklist spelling
* Fixed error message when trying to filter users without a valid uid
* Made posix account visible, even if there are no shadow attributes inside this entry


### Features

* Made user dn configurable
* Included setup
* Translation updates

## gosa 2.1rc1


Bugfix release

### Bug Fixes

* Fixed annoying ACL bug in template mode
* Fixed possible privilege escalation problem in password routine (thanks to Henning Schmiedehausen)
* Removed password storage from user info class (thanks to Rainer Herbst)


### Features

* Various interface cleanups
* Templatization finished
* Reworked user headpage
* Made GOsa more robust in detecting errors in config
* Added additional error messages reported by LDAP server
* Added schmemacheck hook
* Started with setup implementation

## gosa 2.1beta3


Bugfix release

### Bug Fixes

* Made template mode remember the templates primary group
* Fixed user/group lists to react on filter changes


### Features

* Templatized posix plugin
* Added option to disable strict checking of uid/gid names
* Massive samba3 updates
* Made ou=people and ou=groups configurable

## gosa 2.1beta2


Bugfix and feature enhancement release.

### Bug Fixes

* Made GOsa remove object locks when changing plugins during edit process.
* Fixed handling for multiple ACL's per base
* Fixed listboxes to unify output / sort output
* Fixed annoying bug in tab_groups.inc when removing the mailtab
* Bases did not get set in template mode
* Fixed user part


### Features

* Added DHCP plugin
* Gerneral speed tunig, reduced the number of unessasary ldap accesses
* Added syslog output for actions "save" and "remove"
* Templatized faxaccount/pureftpd/samba and mail plugins
* Included calendar.js functionality in samba plugin

## gosa 2.1beta1


This release has some feature enhancements and contains many bugfixes and design cleanups

### Bug Fixes

* Fixed many HTML related things. Pages are now perfectly validated as html 4.01 transitional.
* Added dn cleaner to getDN() in order to fix problems with "broken" ldap databases.


### Features

* Added schemata for iplanet, checked if it works.
* Rewrote phonelist, added vcard export.
* Added filters to allmost all plugins.
* Added DNS plugin.
* Generic userinterface cleanups, everything is a template now and can be redesigned/stripped.
* Improved translations, added missing ones.
* Added choosable templates for mail vacation messages.
* Improved templating stuff to generate user defined auto uids.
* Made user interface more comprehensive, so its important for you to start with a clean gosa.conf from contrib.
* Added external password change hook, so that its possible to synchronize with a non samba PDC via scripts. (Some organizations tend to keep a readable copy of their users password which possible now, too.)
* Updated FAQ

## gosa 2.0.1


This release doesn't have feature enhancements (nearly), only bugfixes reported by users are incorporated.

### Bug Fixes

* Fixed oblivious fields when changing to subdialogs. All user dialogs were affected
* Made facsimileTelephoneNumber beeing saved without the need of a fax account
* Fixed printer sorting which destroyed the array index
* Removed redundant fields in terminal configuration
* Made terminal plugin save the terminal hardware information
* Added missing </html> tags to index.php/main.php
* Fixed debian debconf script not to touch uidbase/ridbase values in gosa.conf
* Fixed "Force ID", which creates a group for the posix user with forced ID.
* Finetuning in login window behaviour
* Code cleanup and templatized two more plugins
* Added 'dn cleaner' for the acl list. So syntactically problematic dn's with strange commata get fixed.


### Features

* As requested by some users, you can now advise GOsa not to create a group for the user, but take an existing group as primary one.

## gosa 2.0 final


### Bug Fixes

* Made samba3 support work
* Fixed several small bugs with the templating stuff
* Fixed problem with shared folders, added missing attribute gosaSharedFolderTarget needed in some setups


### Features

* Updated icons
* Renamed icons to have more logical names

## gosa 2.0rc2


### Bug Fixes

* Corrected mistakenly copied ui object in functions.inc
* Fixed errors when activating new terminals
* Removed krb warnings in class_user.inc
* Plugins user, apps, groups and departments didn't check for already present entries. Now they do.
* Removed problem in terminal dialog where checkboxes are not saved
* Fixed ACL handling for users primary group
* Fixed mail group handling


### Features

* Replaced own template class by smarty, since only two files were affected by this
* Changed basic layout to seperate public readable files from templates
* Added FAQ, update TODO for next versions
* Made accounts movable between departments
* Added partial spanish translations

## gosa 2.0rc1


### Bug Fixes

* Fixed samba2 rid generation (btw. still missing is sid support for samba3. But this will go into the final.)
* Fixed many minor bugs


### Features

* Switched to XML based gosa.conf
* Cleaned all plugins, moved to children of plugin.conf
* Moved back to gettext for translations
* Added hooks for pre-/post-install scripts
* Cleaned LDAP class
* Added workarounds for MS-IE (>5.5) to render transparent PNGs in a correct way
* Redesigned login screen / some plugins
* Added hooks for eGOsa, which is a java applet based browsing tool
* Switched from user based ACLs to group based ACLs, removed standalone ACL plugin in favor of new group tab.
* Introduced simple theming support
* Added 'dn'-renaming for accounts

---

*Changelog starts with latest Beta 1.99.97*