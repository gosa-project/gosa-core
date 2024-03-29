<?php
$class_mapping= array(
                "msgPool" => "include/utils/class_msgPool.inc",
                "xml" => "include/utils/class_xml.inc",
                "timezone" => "include/utils/class_timezone.inc",
                "tests" => "include/utils/class_tests.inc",
                "writeexcel_worksheet" => "include/utils/excel/class.writeexcel_worksheet.inc.php",
                "writeexcel_biffwriter" => "include/utils/excel/class.writeexcel_biffwriter.inc.php",
                "writeexcel_workbook" => "include/utils/excel/class.writeexcel_workbook.inc.php",
                "writeexcel_format" => "include/utils/excel/class.writeexcel_format.inc.php",
                "writeexcel_formula" => "include/utils/excel/class.writeexcel_formula.inc.php",
                "writeexcel_olewriter" => "include/utils/excel/class.writeexcel_olewriter.inc.php",
                "sortableListing" => "include/class_sortableListing.inc",
                "session" => "include/class_session.inc",
                "configRegistry" => "include/class_configRegistry.inc",
                "gosaProperty" => "include/class_configRegistry.inc",
                "baseSelector" => "include/class_baseSelector.inc",
                "Socket_Client" => "include/class_socketClient.inc",
                "tabs" => "include/class_tabs.inc",
                "GOsaRegistration" => "include/class_GOsaRegistration.inc",
                "pathNavigator" => "include/class_pathNavigator.inc",
                "CopyPasteHandler" => "include/class_CopyPasteHandler.inc",
                "ldapMultiplexer" => "include/class_ldapMultiplexer.inc",
                "ItemSelector" => "include/class_ItemSelector.inc",
                "log" => "include/class_log.inc",
                "management" => "include/class_management.inc",
                "msg_dialog" => "include/class_msg_dialog.inc",
                "userFilter" => "include/class_userFilter.inc",
                "listing" => "include/class_listing.inc",
                "filter" => "include/class_filter.inc",
                "passwordMethodsasl" => "include/password-methods/class_password-methods-sasl.inc",
                "passwordMethodsha" => "include/password-methods/class_password-methods-sha.inc",
                "passwordMethodRemote" => "include/password-methods/class_password-methods-remote.inc",
                "passwordMethod" => "include/password-methods/class_password-methods.inc",
                "passwordMethodssha" => "include/password-methods/class_password-methods-ssha.inc",
                "passwordMethodCrypt" => "include/password-methods/class_password-methods-crypt.inc",
                "passwordMethodsmd5" => "include/password-methods/class_password-methods-smd5.inc",
                "passwordMethodClear" => "include/password-methods/class_password-methods-clear.inc",
                "passwordMethodMd5" => "include/password-methods/class_password-methods-md5.inc",
                "jsonRPC" => "include/class_jsonRPC.inc",
                "stats" => "include/class_stats.inc",
                "filterLDAP" => "include/class_filterLDAP.inc",
                "releaseSelector" => "include/class_releaseSelector.inc",
                "config" => "include/class_config.inc",
                "acl" => "include/class_acl.inc",
                "parseXml" => "include/functions_helpviewer.inc",
                "remoteObject" => "include/class_remoteObject.inc",
                "userFilterEditor" => "include/class_userFilterEditor.inc",
                "userinfo" => "include/class_userinfo.inc",
                "pChartInclude" => "include/class_pChartInclude.inc",
                "certificate" => "include/class_certificate.inc",
                "pluglist" => "include/class_pluglist.inc",
                "filterNOACL" => "include/class_filterNOACL.inc",
                "LDAP" => "include/class_ldap.inc",
                "SnapShotDialog" => "include/class_SnapShotDialog.inc",
                "all" => "include/class_core.inc",
                "core" => "include/class_core.inc",
                "listingSortIterator" => "include/class_listingSortIterator.inc",
                "Print_a_class" => "include/functions_debug.inc",
                "multi_plug" => "include/class_multi_plug.inc",
                "csvExporter" => "include/exporter/class_csvExporter.inc",
                "pdfExporter" => "include/exporter/class_pdfExporter.inc",
                "SnapshotHandler" => "include/class_SnapshotHandler.inc",
                "departmentSortIterator" => "include/class_departmentSortIterator.inc",
                "plugin" => "include/class_plugin.inc",
                "Step_Migrate" => "setup/class_setupStep_Migrate.inc",
                "Step_Welcome" => "setup/class_setupStep_Welcome.inc",
                "setup" => "setup/class_setup.inc",
                "setup_step" => "setup/class_setupStep.inc",
                "Step_Ldap" => "setup/class_setupStep_Ldap.inc",
                "Step_Schema" => "setup/class_setupStep_Schema.inc",
                "Step_Checks" => "setup/class_setupStep_Checks.inc",
                "Step_Language" => "setup/class_setupStep_Language.inc",
                "Step_License" => "setup/class_setupStep_License.inc",
                "Step_Finish" => "setup/class_setupStep_Finish.inc",
                "trustModeDialog" => "plugins/personal/posix/trustModeDialog/class_trustModeDialog.inc",
                "filterLDAPBlacklist" => "plugins/personal/posix/groupSelect/class_filterLDAPBlacklist.inc",
                "groupSelect" => "plugins/personal/posix/groupSelect/class_groupSelect.inc",
                "posixAccount" => "plugins/personal/posix/class_posixAccount.inc",
                "trustSelect" => "plugins/personal/posix/trustSelect/class_trustSelect.inc",
                "MyAccountTabs" => "plugins/personal/myaccount/MyAccountTabs.inc",
                "MyAccount" => "plugins/personal/myaccount/class_MyAccount.inc",
                "user" => "plugins/personal/generic/class_user.inc",
                "password" => "plugins/personal/password/class_password.inc",
                "aclResolver" => "plugins/generic/references/class_aclResolver.inc",
                "reference" => "plugins/generic/references/class_reference.inc",
                "ldifViewer" => "plugins/generic/references/class_ldifViewer.inc",
                "infoPage" => "plugins/generic/infoPage/class_infoPage.inc",
                "welcome" => "plugins/generic/welcome/class_welcome.inc",
                "rssReader" => "plugins/generic/dashBoard/class_rssReader.inc",
                "dbInformation" => "plugins/generic/dashBoard/dbInformation/class_dbInformation.inc",
                "dbChannelStatus" => "plugins/generic/dashBoard/dbChannelStatus/class_dbChannelStatus.inc",
                "dbNotifications" => "plugins/generic/dashBoard/dbNotifications/class_dbNotifications.inc",
                "dbPluginStatus" => "plugins/generic/dashBoard/dbPluginStatus/class_dbPluginStatus.inc",
                "dashBoard" => "plugins/generic/dashBoard/class_dashBoard.inc",
                "RegistrationDialog" => "plugins/generic/dashBoard/Register/class_RegistrationDialog.inc",
                "pieChart2" => "plugins/generic/statistics/chartClasses/class_pieChart2.inc",
                "actionSelectChart" => "plugins/generic/statistics/chartClasses/class_actionSelectChart.inc",
                "statChart" => "plugins/generic/statistics/chartClasses/class_statChart.inc",
                "objectCountChart" => "plugins/generic/statistics/chartClasses/class_objectCountChart.inc",
                "categoryActionsOverTime" => "plugins/generic/statistics/chartClasses/class_categoryActionOverTime.inc",
                "renderTimeChart" => "plugins/generic/statistics/chartClasses/class_renderTimeChart.inc",
                "pieChart1" => "plugins/generic/statistics/chartClasses/class_pieChart1.inc",
                "cpuLoadChart" => "plugins/generic/statistics/chartClasses/class_cpuLoadChart.inc",
                "memoryUsageChart" => "plugins/generic/statistics/chartClasses/class_memoryUsageChart.inc",
                "durationTimeChart" => "plugins/generic/statistics/chartClasses/class_durationTimeChart.inc",
                "passwordChangeChart" => "plugins/generic/statistics/chartClasses/class_passwordChangeChart.inc",
                "statistics" => "plugins/generic/statistics/class_statistics.inc",
                "filterLDAPDepartmentBlacklist" => "plugins/admin/ogroups/objectSelect/class_filterLDAPDepartmentBlacklist.inc",
                "objectSelect" => "plugins/admin/ogroups/objectSelect/class_objectSelect.inc",
                "ogrouptabs" => "plugins/admin/ogroups/tabs_ogroups.inc",
                "_available" => "plugins/admin/ogroups/tabs_ogroups.inc",
                "migrate_ogroupRDN" => "plugins/admin/ogroups/migration/class_migrate_ogroupRDN.inc",
                "ogroupManagement" => "plugins/admin/ogroups/class_ogroupManagement.inc",
                "ogroup" => "plugins/admin/ogroups/class_ogroup.inc",
                "grouptabs" => "plugins/admin/groups/tabs_group.inc",
                "groupManagement" => "plugins/admin/groups/class_groupManagement.inc",
                "filterGroupLDAP" => "plugins/admin/groups/class_filterGroupLDAP.inc",
                "migrate_groupRDN" => "plugins/admin/groups/migration/class_migrate_groupRDN.inc",
                "group" => "plugins/admin/groups/class_group.inc",
                "userGroupSelect" => "plugins/admin/groups/userGroupSelect/class_userGroupSelect.inc",
                "userSelect" => "plugins/admin/groups/userSelect/class_userSelect.inc",
                "singleUserSelect" => "plugins/admin/groups/singleUserSelect/class_singleUserSelect.inc",
                "filterACL" => "plugins/admin/acl/class_filterACL.inc",
                "acltab" => "plugins/admin/acl/tabs_acl.inc",
                "migrate_aclRoleRDN" => "plugins/admin/acl/migration/class_migrate_aclRoleRDN.inc",
                "aclrole" => "plugins/admin/acl/class_aclRole.inc",
                "aclroletab" => "plugins/admin/acl/tabs_acl_role.inc",
                "aclManagement" => "plugins/admin/acl/class_aclManagement.inc",
                "usertabs" => "plugins/admin/users/tabs_user.inc",
                "userManagement" => "plugins/admin/users/class_userManagement.inc",
                "migrate_userRDN" => "plugins/admin/users/migration/class_migrate_userRDN.inc",
                "dcObject" => "plugins/admin/departments/class_dcObject.inc",
                "filterDEPARTMENT" => "plugins/admin/departments/class_filterDEPARTMENT.inc",
                "organization" => "plugins/admin/departments/class_organizationGeneric.inc",
                "country" => "plugins/admin/departments/class_countryGeneric.inc",
                "locality" => "plugins/admin/departments/class_localityGeneric.inc",
                "departmentManagement" => "plugins/admin/departments/class_departmentManagement.inc",
                "department" => "plugins/admin/departments/class_department.inc",
                "domain" => "plugins/admin/departments/class_domain.inc",
                "deptabs" => "plugins/admin/departments/tabs_department.inc",
                "DynamicLdapGroup" => "plugins/addons/dyngroup/class_DynamicLdapGroup.inc",
                "filterCONFIGPROPERTIES" => "plugins/addons/propertyEditor/class_filterProperties.inc",
                "migrateRDN" => "plugins/addons/propertyEditor/migration/class_migrateRDN.inc",
                "propertyEditor" => "plugins/addons/propertyEditor/class_propertyEditor.inc",
                "commandVerifier" => "plugins/addons/propertyEditor/class_commandVerifier.inc",
 );
