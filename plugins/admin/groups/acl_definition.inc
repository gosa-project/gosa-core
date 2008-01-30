<?php
# ACLD definitions go here

$ACLD['application']=array("cn" => "LDAP name",
			"create" => "Create",
			"delete" => "Delete",
			"description" => "Description",
			"gosaApplicationExecute" => "Execute",
			"gosaApplicationFlags" => "Modify flags",
			"gotoLogonScript" => "Script",
			"gosaApplicationIcon" => "Icon",
			"gosaApplicationName" => "Visible name");
$ACLD['mimetype']=array("cn" 				=> "LDAP name",
			"create" 						=> "Create",
			"delete" 						=> "Delete",
			"description" 					=> "Description",
			"gotoMimeIcon" 					=> "Icon",
			"gotoMimeApplication" 			=> "Modify flags",
			"gotoMimeEmbeddedApplication" 	=> "Embedded applications",
			"gotoMimeLeftClickAction" 		=> "Left click actions",
			"gotoMimeFilePattern" 			=> "File patterns",
			"gotoMimeGroup" 				=> "Mime group",
			"description"	 				=> "Description");
$ACLD['department']= array(
			"businessCategory",
			"description",
			"facsimileTelephoneNumber",
			"l",
			"ou",
			"create",
			"delete",
			"base",
			"postalAddress",
			"st",
			"telephoneNumber");
$ACLD['gofax']=      array("cn",
			"create",
			"description",
			"facsimileAlternateTelephoneNumber",
			"facsimileTelephoneNumber",
			"goFaxBlocklist",
			"goFaxDeliveryMode",
			"goFaxFormat",
			"goFaxIsEnabled",
			"goFaxLanguage",
			"goFaxPrinter",
			"goFaxRBlockgroups",
			"goFaxSBlockgroups",
			"gosaMailForwardingAddress");
$ACLD['blocklists']= array("cn",
			"create",
			"delete",
			"description",
			"goFaxBlocklist");
$ACLD['ogroup']=   array("ogroup","create","delete","cn", "description", "gosaGroupObjects","base");
$ACLD['group']=      array("cn",
			"memberUid",
			"create",
			"delete",
			"description",
			"force_gid",
			"password",
			"gidNumber");
$ACLD['appgroup']=   array("create");
$ACLD['all']=        array();
$ACLD['acl']=   array('acl');
$ACLD['mailgroup']=  array(
			"create",
			"default_permission",
			"imap_perms",
			"member_permissions",
			"mail",
			"gosaMailAlternateAddress",
			"gosaMailForwardingAddress",
			"gosaMailQuota",
			"gosaMailServer");
$ACLD['terminal']=   array("gotoAdaptPath",
			"gotoAutoFs",
			"gotoBootKernel",
			"gotoFilesystem",
			"gotoFontPath",
			"gotoKernelParameters",
			"gotoLdapServer",
			"gotoLpdEnable",
			"gotoLpdServer",
			"gotoMode",
			"gotoModule",
			"gotoModules",
			"gotoNtpServer",
			"gotoScannerEnable",
			"gotoSwapServer",
			"gotoSyslogServer",
			"gotoTerminalPath",
			"gotoXColordepth",
			"gotoXDriver",
			"gotoXHsync",
			"gotoXKbLayout",
			"gotoXKbModel",
			"gotoXKbVariant",
			"gotoXMethod",
			"gotoXMouseButtons",
			"gotoXMousePort",
			"gotoXMouseType",
			"gotoXResolution",
			"gotoXVsync",
			"gotoXdmcpServer",
			"login",
			"password");
$ACLD['workgeneric']=   array("base",
			"gotoMode",
			"gotoSyslogServer",
			"gotoNtpServer",
			"FAIstate",
			"action",
			"password");
$ACLD['workstartup']=   array("gotoBootKernel",
			"gotoKernelParameters",
			"FAIdebianMirror",
			"FAIclass",
			"gotoModules",
			"gotoShare");
$ACLD['workservice']=   array("gotoXKbModel",
			"gotoXKbLayout",
			"gotoXKbVariant",
			"gotoXMouseType",
			"gotoXMouseport",
			"gotoXResolution",
			"gotoXColordepth",
			"AutoSync",
			"gotoXHsync",
			"gotoVHsync",
			"gotoScannerEnable",
			"gotoXDriver");
$ACLD['printgeneric']=   array("cn",
			"base",
			"labeledURI",
			"gotoPrinterPPD",
			"gotoUserPrinter",
			"macAddress",
			"gotoUserAdminPrinter",
			"gotoGroupAdminPrinter",
			"gotoUserPrinter",
			"gotoAdminPrinter",
			"description");
$ACLD['servgeneric']=   array("cn",
			"base",
			"create",
			"gotoMode",
			"FAIstate",
			"description");
$ACLD['servdb']=   array("goKrbServer",
			"goKrbRealm",
			"goKrbAdmin",
			"goFaxServer",
			"goFaxAdmin",
			"goFaxPassword",
			"goFonServer",
			"goFonAdmin",
			"goFonPassword",
			"goFonCountryCode",
			"goFonAreaCode",
			"goImapServer",
			"goImapPassword",
			"goImapName",
			"goImapConnect",
			"goImapAdmin",
			"goImapPassword",
			"goImapSievePort",
			"goLogDBServer",
			"goLogAdmin",
			"goLogPassword",
			"goGlpiServer",
			"goGlpiAdmin",
			"goGlpiPassword",
			"goGlpiDatabase",
			"goKrbPassword");
$ACLD['servkolab']=   array("postfix_mydomain",
			"postfix_mydestination",
			"cyrus_admins",
			"cyrus_pop3s",
			"cyrus_imap",
			"cyrus_imaps",
			"cyrus_sieve",
			"proftpd_ftp",
			"apache_http",
			"postfix_enable_virus_scan",
			"apache_allow_unauthenticated_fb",
			"postfix_mynetworks",
			"postfix_relayhost",
			"postfix_allow_unauthenticated",
			"cyrus_pop3");
$ACLD['servservice']=   array("goExportEntry",
			"goNtpServer",
			"goLdapBase",
			"goTerminalServer",
			"goXdmcpIsEnabled",
			"goSyslogServer",
			"goCupsServer",
			"goMailServer",
			"goLdapServer");
$ACLD['servrepository']=   array("base",
			"gotoMode",
			"description");
$ACLD['user']=       array("academicTitle",
			"create",
			"delete",
			"certificateSerialNumber",
			"certificates",
			"departmentNumber",
			"description",
			"employeeNumber",
			"employeeType",
			"facsimileTelephoneNumber",
			"functionalTitle",
			"givenName",
			"gouvernmentOrganizationalPersonLocality",
			"gouvernmentOrganizationalUnit",
			"gouvernmentOrganizationalUnitDescription",
			"gouvernmentOrganizationalUnitSubjectArea",
			"homePhone",
			"homePostalAddress",
			"houseIdentifier",
			"ivbbLastDeliveryCollective",
			"jpegPhoto",
			"kerberos",
			"l",
			"labeledURI",
			"mobile",
			"o",
			"ou",
			"pager",
			"password",
			"personalTitle",
			"postalAddress",
			"postalCode",
			"passwordStorage",
			"publicVisible",
			"role",
			"roomNumber",
			"sn",
			"st",
			"street",
			"telephoneNumber",
			"uid",
			"vocation");
$ACLD['mailAccount']=       array(
			"create",
			"custom_sieve",
			"gosaMailAlternateAddress",
#			"gosaMailDeliveryMode",
			"gosaMailForwardingAddress",
			"gosaMailMaxSize",
			"gosaMailQuota",
			"gosaMailServer",
			"gosaSpamMailbox",
			"gosaSpamSortLevel",
			"use_vacation" 			=> _("Enable vacation message"),
			"gosaVacationMessage"	=> _("Vacation message"),
			"gosaVacationTimer"		=> _("Vacation start/stop"),
			"sieveManagement",
                        "drop_own_mails",
			"mail",
			"only_local");
$ACLD['posixAccount']=      
		array(  "create",	
			"allowedHosts",
			"force_ids",
			"gidNumber",
			"gosaDefaultLanguage",
			"gosaDefaultPrinter",
			"gosaHostACLD",
			"gosaHostAcl",
			"groupMembership",
			"homeDirectory",
			"kerberos",
			"loginShell",
			"memberUid",
			"shadow",
			"shadowExpire",
			"shadowInactive",
			"shadowMax",
			"shadowMin",
			"shadowWarning",
			"uidNumber");
$ACLD['sambaAccount']=      
		array(
			"create",
			"acctFlags" 		=> "samba2:acctFlags", 
			"homeDrive" 		=> "samba2:homeDrive",
			"profilePath"		=> "samba2:profilePath",
			"scriptPath" 		=> "samba2:scriptPath",
			"smbHome" 			=> "samba2:smbHome",
			"sambaHomePath"		,//=> _("Home path"), 
			"sambaHomeDrive"	,//=> _("Home drive"), 
			"sambaDomainName"	,//=> _("Samba domain name"),
			"sambaLogonScript"	,//=> _("Logon script"),
			"sambaProfilePath"	,//=> _("Profile path"),	 

			"sambaLogonHours",

			"sambaLogonTime",//	=> _("Limit Logon Time"),
			"sambaLogoffTime",// 	=> _("Limit Logoff Time"), 
			"sambaKickoffTime",// 	=> _("Account expiry"), 	
			"sambaPwdCanChange",//	=> _("Can change password"),
			"sambaPwdMustChange",//=> _("Password expiry"), 
			"no_password_required",// => _("Login from windows client requires no password"),
			"temporary_disable",// => _("Lock account"),
			"sambaUserWorkstation",//=> _("Allow connection from these workstations only"),

			"connectclientdrives",//	=> _("Connect client drives at logon"),
			"connectclientprinters",// => _("Connect client printers at logon"),
			"defaultprinter",//	 	=> _("Default to main client printer"),

			"shadow",//				=> _("Shadowing"),
			"brokenconn",//			=> _("On broken or timed out"),
			"reconn",//				=> _("Reconnect if disconnected"),

			"inherit",//				=> _("Inherit client config")
			"tslogin",//				=> _("Inherit client config")

			"CtxWFHomeDir",
			"CtxWFHomeDirDrive",
			"CtxWFProfilePath",

			"CtxMaxConnectionTime",
			"CtxMaxDisconnectionTime",
			"CtxMaxIdleTime",

			"CtxInitialProgram",
			"CtxWorkDirectory"
			) ;

$ACLD['netatalk']=	array(
			"create",
			"netatalkShare",
			"netatalkUserHomepath");


$ACLD['conference']= array(
			"goFonConference",
			"goFonPIN",
			"base",
			"delete",
			"cn",
			"description",
			"telephoneNumber",			
			"goFonConferenceOption",
			"goFonConferenceOptionLifetime",
			"goFonConferenceOptionFormat",
			"goFonConferenceOption_s",
			"goFonConferenceOption_i",
			"goFonConferenceOption_r",
			"goFonConferenceOption_d",
			"goFonConferenceOption_D",
			"goFonConferenceOption_P",
			"goFonConferenceOption_M",
			"goFonConferenceOption_c",
			"goFonConferenceOwner");

$ACLD['environment']=array(
			"create",
			"gotoPrinter",
			"gotoPrinterToggle",
			"gotoEnvironment",
			"gotoShare",
			"gotoProfileServer",
			"gotoProfileFlags",
			"gotoXResolution",
			"gotoLogonScript",
			"gotoKioskProfile",
			"gotoProfileQuota",
			"gotoHotplugDevice",
			"gotoShareAppleMounts"
			);
/* Connectivity accounts */
$ACLD['phpgwAccount']			=array("create");
$ACLD['webdavAccount']			=array("create");
$ACLD['intranetAccount']		=array("create");
$ACLD['phpscheduleitAccount']		=array("create");
$ACLD['pptpAccount']			=array("create");
$ACLD['proxyAccount']			=array("create","proxyAccount","gosaProxyAcctFlags","gosaProxyQuota",
						"gosaProxyWorkingStart","gosaProxyWorkingStop");
$ACLD['pureftpdAccount']		=array("create","FTPUploadBandwidth","FTPStatus",
						"FTPDownloadBandwidth","FTPQuotaFiles","FTPQuotaMBytes",
						"FTPUploadRatio","FTPDownloadRatio");
$ACLD['kolabAccount']			=array("create","kolabFreeBusyFuture", "unrestrictedMailSize", "calFBURL","kolabDelegate");
$ACLD['opengwAccount']			=array("create"	=>"Enable/disable account",	
		"LocationTeam"	=>"Location team",
		"TemplateUser"	=>"Template user",
		"is_locked"		=>"Lock status",
		"TeamIDs"		=>"Teams");


$ACLD['systems']			=array("systems" 		);
$ACLD['FAIclass']			=array("FAIclass" 		);

$ACLD['phoneAccount'] = array("create","goFonDeliveryMode","goFonHomeServer",
      "goFonHardware","goFonPIN","goFonVoicemailPIN","telephoneNumber", "goFonMacro");

$ACLD['nagiosAccount'] = array(
		"create",
		"NagiosAlias",
		 "NagiosPager",
		 "NagiosMail",
		 "HostNotificationPeriod",
		 "ServiceNotificationPeriod",
		 "HostNotificationCommands",
		 "ServiceNotificationCommands",
		 "HostNotificationOptions",
		 "ServiceNotificationOptions",
		 "AuthorizedAllHosts",
		 "AuthorizedAllServices",
		 "AuthorizedSystemCommands",
		 "AuthorizedAllHostCommands",
		 "AuthorizedAllServiceCommands",
		 "AuthorizedConfigurationInformation",
		 "AuthorizedSystemInformation");

$ACLD['addressbook']=  array("addressbook","add", "delete",
	  "sn", "givenName", "mail", "title",
      "initials", "homePostalAddress", "displayName",
      "homePhone", "mobile", "o", "postalAddress", "l",
      "postalCode", "st", "ou", "telephoneNumber",
      "facsimileTelephoneNumber", "pager");
$ACLD['fonreport']	=  array("fonreport", "src", "dst", "channel", "lastapp", "disposition", "duration");
$ACLD['faxreport']	=  array("faxreport");
$ACLD['connectivity']=  array("connectivity");
$ACLD['logview']	=  array("logview");
$ACLD['mailqueue']	=  array("mailqueue","unhold_all","hold_all","del_all","requeue_all","unhold","hold","del","requeue","query","header");
$ACLD['ldapmanager']=  array("ldapmanager","import","export","xlsexport","csvimport");

$ACLD['FAIclass']	= 	array(
		// Allow displaying of FAI menu element
		"FAIclass"	=> "FAI management enabled");

$ACLD['phonequeue'] = array( "create","goFonTimeOut","goFonMaxLen","goFonAnnounceFrequency","goFonDialOption_t","goFonDialOption_T",
      "goFonDialOption_h","goFonDialOption_r","cn","goFonHomeServer",
      "goFonDialOption_H","goFonMusiconHold","goFonWelcomeMusic","goFonQueueReportHold","goFonQueueYouAreNext",
      "goFonQueueThereAre","goFonQueueCallsWaiting","goFonQueueThankYou","goFonQueueMinutes","goFonQueueSeconds","goFonQueueLessThan",
      "telephoneNumber","goFonQueueLanguage","goFonQueueStrategy","goFonQueueAnnounceHoldtime","goFonQueueAnnounce","goFonDialOption","goFonQueueRetry");					
$ACLD['goFonMacro']	=array("goFonMacro","edit","delete");
$ACLD['mailogroup']	= array("create");
$ACLD['scalix']	= array(
		"scalixMailnode"			=> _("Scalix Mail node"),
		"scalixMailboxClass"		=> _("Mailbox class"),
		"scalixServerLanguage"		=> _("Server language"),
		"scalixAdministrator"		=> _("Administrator"),
		"scalixMailboxAdministrator"=> _("Mailbox administrator"),
		"scalixHideUserEntry"		=> _("Hide user entry in Scalix"),
		"scalixLimitMailboxSize"	=> _("Mailbox size limitations"),
		"scalixLimitOutboundMail"	=> _("Limit outbound"),
		"scalixLimitInboundMail"	=> _("Limit inbound"),
		"scalixLimitNotifyUser" 	=> _("Notify user"),
		"scalixEmailAddress"		=> _("Scalix email addresses"));
$ACLD['gotomasses']=array("create" => _("Create/edit new task"),"remove" => _("Remove tasks"));
$ACLD['mimetype'] = array("create","remove","cn","gotoMimeApplication","gotoMimeLeftClickAction","gotoMimeIcon","description","gotoMimeFilePattern","gotoMimeEmbeddedApplication","gotoMimeGroup","gotoMimeFilePattern","gotoMimeApplication","gotoMimeEmbeddedApplication");
?>
