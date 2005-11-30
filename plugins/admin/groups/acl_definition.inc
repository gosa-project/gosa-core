<?php
# ACLD definitions go here

$ACLD['application']=array("cn" => "LDAP name",
			"create" => "Create",
			"delete" => "Delete",
			"description" => "Description",
			"gosaApplicationExecute" => "Execute",
			"gosaApplicationFlags" => "Modify flags",
			"gosaApplicationName" => "Visible name");
$ACLD['department']= array("businessCategory",
			"description",
			"facsimileTelephoneNumber",
			"l",
			"ou",
			"postalAddress",
			"st",
			"telephoneNumber");
$ACLD['gofax']=      array("cn",
			"create",
			"description",
			"facsimileAlternateTelephoneNumber",
			"facsimileTelephoneNumber",
			"faxReport",
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
			"description",
			"goFaxBlocklist");
$ACLD['group']=      array("cn",
			"create",
			"delete",
			"description",
			"force_gid",
			"password",
			"gidNumber");
$ACLD['appgroup']=   array();
$ACLD['all']=        array();
$ACLD['mailgroup']=  array("default_permission",
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
			"publicVisible",
			"role",
			"roomNumber",
			"sn",
			"st",
			"street",
			"telephoneNumber",
			"uid",
			"vocation");
$ACLD['mail']=       array("custom_sieve",
			"gosaMailAlternateAddress",
			"gosaMailDeliveryMode",
			"gosaMailForwardingAddress",
			"gosaMailMaxSize",
			"gosaMailQuota",
			"gosaMailServer",
			"gosaSpamMailbox",
			"gosaSpamSortLevel",
			"gosaVacationMessage",
			"mail",
			"only_local");
$ACLD['posix']=      array("allowedHosts",
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
$ACLD['samba']=      array("acctFlags",
			"allow_pwchange",
			"homeDrive",
			"no_password_required",
			"profilePath",
			"scriptPath",
			"smbHome",
			"temporary_disable");

$ACLD['samba'] = array (
						"sambaLogonTime",//	=> _("Limit Logon Time"),
						"sambaLogoffTime",// 	=> _("Limit Logoff Time"), 
						"sambaKickoffTime",// 	=> _("Account expiry"), 	
						"sambaPwdCanChange",//	=> _("Can change password"),
						"sambaPwdMustChange",//=> _("Password expiry"), 
			 	        "sambaHomePath"		,//=> _("Home path"), 
						"sambaHomeDrive"	,//=> _("Home drive"), 
						"sambaLogonScript"	,//=> _("Logon script"),
				        "sambaProfilePath"	,//=> _("Profile path"),	 
						"sambaDomainName"	,//=> _("Samba domain name"),
						"allow_pwchange"	,//=> _("Allow password change"),
						"no_password_required",// => _("Login from windows client requires no password"),
						"temporary_disable",// => _("Lock account"),
						"sambaUserWorkstation",//=> _("Allow connection from these workstations only"),
						"connectclientdrives",//	=> _("Connect client drives at logon"),
						"connectclientprinters",// => _("Connect client printers at logon"),
						"defaultprinter",//	 	=> _("Default to main client printer"),
						"shadow",//				=> _("Shadowing"),
						"brokenconn",//			=> _("On broken or timed out"),
						"reconn",//				=> _("Reconnect if disconnected"),
						"inherit"//				=> _("Inherit client config")
						) ;


$ACLD['proxyAccount']=      array(
			"proxyAccount",
			"gosaProxyAcctFlags",
			"gosaProxyQuota",
			"gosaProxyWorkingStart",
			"gosaProxyWorkingStop");

$ACLD['pureftpdAccount']=   array(
			"pureftpd",	
			"FTPUploadBandwidth",
			"FTPStatus",
			"FTPDownloadBandwidth",
			"FTPQuotaFiles",
			"FTPQuotaMBytes",
			"FTPUploadRatio",
			"FTPDownloadRatio");
$ACLD['goFonConference']= array(
			"goFonConference",
			"goFonPIN",
			"base",
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

$ACLD['gotoEnvironment']=array(
					"gotoPrinter",
					"gotoEnvironment",
					"gotoShare",
					"gotoProfileServer",
					"gotoProfileFlags",
					"gotoXResolution",
					"gotoLogonScript",
					"gotoKioskProfile",
					"gotoProfileQuota",
					"gotoHotplugDevice"
					);

$ACLD['phpgwAccount']			=array("phpgwAccount" 			);
$ACLD['webdavAccount']			=array("webdavAccount" 			);
$ACLD['intranetAccount']		=array("intranetAccount" 		);
$ACLD['pptpAccount']			=array("gosapptp" 	  			);
$ACLD['phpscheduleitAccount']	=array("gosaphpscheduleit" 	  	);

$ACLD['phoneAccount'] = array("goFonDeliveryMode",
      "goFonHardware","goFonPIN","goFonVoicemailPIN","telephoneNumber", "goFonMacro");

$ACLD['nagiosAccount'] = array("NagiosAlias"
, "NagiosPager"
, "NagiosMail"
, "HostNotificationPeriod"
, "ServiceNotificationPeriod"
, "HostNotificationCommands"
, "ServiceNotificationCommands"
, "HostNotificationOptions"
, "ServiceNotificationOptions"
, "AuthorizedAllHosts"
, "AuthorizedAllServices"
, "AuthorizedSystemCommands"
, "AuthorizedAllHostCommands"
, "AuthorizedAllServiceCommands"
, "AuthorizedConfigurationInformation"
, "AuthorizedSystemInformation");

$ACLD['global-addressbook']=  array("create", "delete");
?>
