create database glpi;
use glpi;

CREATE TABLE `glpi_cartridges` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_glpi_cartridges_type` int(11) NOT NULL default '0',
  `FK_glpi_printers` int(11) NOT NULL default '0',
  `date_in` date default NULL,
  `date_use` date default NULL,
  `date_out` date default NULL,
  `pages` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_cartridges_type` (`FK_glpi_cartridges_type`),
  KEY `FK_glpi_printers` (`FK_glpi_printers`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;

INSERT INTO `glpi_cartridges` (`ID`, `FK_glpi_cartridges_type`, `FK_glpi_printers`, `date_in`, `date_use`, `date_out`, `pages`) VALUES 
(5, 5, 5, '2006-09-29', '2006-09-29', NULL, 0),
(4, 5, 5, '2006-09-29', '2006-09-29', NULL, 0);


CREATE TABLE `glpi_cartridges_assoc` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_glpi_cartridges_type` int(11) NOT NULL default '0',
  `FK_glpi_type_printer` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_glpi_type_printer` (`FK_glpi_type_printer`,`FK_glpi_cartridges_type`),
  KEY `FK_glpi_cartridges_type` (`FK_glpi_cartridges_type`),
  KEY `FK_glpi_type_printer_2` (`FK_glpi_type_printer`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=13 ;


INSERT INTO `glpi_cartridges_assoc` (`ID`, `FK_glpi_cartridges_type`, `FK_glpi_type_printer`) VALUES 
(1, 1, 1),
(2, 2, 5),
(3, 2, 1),
(4, 3, 13),
(5, 4, 13),
(6, 5, 11),
(7, 5, 10),
(8, 5, 5),
(9, 5, 12),
(10, 5, 2),
(11, 5, 14),
(12, 5, 3);



CREATE TABLE `glpi_cartridges_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `ref` varchar(255) NOT NULL default '',
  `location` int(11) NOT NULL default '0',
  `type` tinyint(4) NOT NULL default '0',
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `tech_num` int(11) default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `comments` text NOT NULL,
  `alarm` tinyint(4) NOT NULL default '10',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `tech_num` (`tech_num`),
  KEY `deleted` (`deleted`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;


INSERT INTO `glpi_cartridges_type` (`ID`, `name`, `ref`, `location`, `type`, `FK_glpi_enterprise`, `tech_num`, `deleted`, `comments`, `alarm`) VALUES 
(1, 'TestPatrone', '', 0, 1, 8, 0, '', '', 0),
(2, 'HP_deskjet_7100C', '', 0, 4, 8, 0, '', 'None', 0),
(3, 'teseter', '', 0, 3, 12, 0, '', '', 0),
(4, 'teseter2', '', 0, 3, 12, 0, '', '', 0),
(5, 'Epson Stylus Nuclear Color', '', 0, 3, 12, 0, '', 'Uranium green.\r\n', 0);



CREATE TABLE `glpi_computer_device` (
  `ID` int(11) NOT NULL auto_increment,
  `specificity` varchar(250) NOT NULL default '',
  `device_type` tinyint(4) NOT NULL default '0',
  `FK_device` int(11) NOT NULL default '0',
  `FK_computers` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `device_type` (`device_type`),
  KEY `device_type_2` (`device_type`,`FK_device`),
  KEY `FK_computers` (`FK_computers`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=136 ;


INSERT INTO `glpi_computer_device` (`ID`, `specificity`, `device_type`, `FK_device`, `FK_computers`) VALUES 
(87, '', 1, 1, 1),
(92, '', 1, 1, 13),
(133, '', 1, 1, 17),
(120, '', 1, 1, 23),
(122, '', 1, 1, 24),
(135, '', 1, 1, 19),
(132, '', 1, 1, 29);



CREATE TABLE `glpi_computers` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(200) NOT NULL default '',
  `serial` varchar(200) NOT NULL default '',
  `otherserial` varchar(200) NOT NULL default '',
  `contact` varchar(90) NOT NULL default '',
  `contact_num` varchar(90) NOT NULL default '',
  `tech_num` int(11) NOT NULL default '0',
  `comments` text NOT NULL,
  `date_mod` datetime default NULL,
  `os` int(11) default NULL,
  `location` int(11) default NULL,
  `domain` int(11) NOT NULL default '0',
  `network` int(11) NOT NULL default '0',
  `model` int(11) default NULL,
  `type` int(11) default NULL,
  `is_template` enum('0','1') NOT NULL default '0',
  `tplname` varchar(200) default NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  PRIMARY KEY  (`ID`),
  KEY `location` (`location`),
  KEY `os` (`os`),
  KEY `type` (`model`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `deleted` (`deleted`),
  KEY `is_template` (`is_template`),
  KEY `date_mod` (`date_mod`),
  KEY `tech_num` (`tech_num`),
  KEY `type_2` (`type`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=41 ;


INSERT INTO `glpi_computers` (`ID`, `name`, `serial`, `otherserial`, `contact`, `contact_num`, `tech_num`, `comments`, `date_mod`, `os`, `location`, `domain`, `network`, `model`, `type`, `is_template`, `tplname`, `FK_glpi_enterprise`, `deleted`) VALUES 
(1, 'cn=asterisk,ou=servers,ou=systems,dc=gonicus,dc=de', '', '', '', '1', 1, '', '2006-03-08 12:12:04', 5, 0, 0, 0, 0, 27, '0', NULL, 8, 'N'),
(4, 'cn=pyramid,ou=terminals,ou=systems,dc=gonicus,dc=de', '', '', '', '', 0, '', '2006-03-08 07:05:07', 5, 0, 0, 0, 0, 21, '0', NULL, 8, 'N'),
(5, 'cn=ctu03,ou=phones,ou=systems,dc=gonicus,dc=de', '', '', '', '', 0, '', '2006-02-10 08:58:15', 6, 0, 0, 0, 0, 21, '0', NULL, 8, 'N'),
(6, 'cn=fax,ou=servers,ou=systems,dc=gonicus,dc=de', '', '', '', '', 3, '', '2006-03-01 10:04:06', 6, 0, 0, 0, 0, 21, '0', NULL, 12, 'N'),
(19, 'cn=cl1--6665,ou=workstations,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '18', 0, '', '2007-01-30 08:03:00', 5, 0, 0, 0, 0, 27, '0', NULL, 16, 'N'),
(11, 'cn=vserver-04f,ou=servers,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, '', '2006-05-12 07:51:32', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(13, 'cn=GetraenkeHalter,ou=netdevices,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '6', 7, 'tester', '2006-06-21 12:34:54', 7, 0, 0, 0, 0, 28, '0', NULL, 16, 'N'),
(18, 'cn=cl1--151,ou=workstations,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'tester', '2007-01-30 07:55:33', 18, 0, 0, 0, 0, 18, '0', NULL, 16, 'N'),
(16, 'cn=ctu153,ou=phones,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '8', 9, 'Tester fÃ¼r den Comment \\\\\\n<br>\r\ntester', '2006-06-20 09:24:35', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(17, 'cn=cl1--157,ou=workstations,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'asdfasdf \n\r\nasdf', '2007-01-30 07:01:41', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(20, 'cn=vserver-01.intranet.gonicus.de,ou=servers,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'sda', '2007-05-21 06:53:29', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(21, 'cn=vserver-01.intranet.gonicus.de,ou=servers,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'sda', '2007-05-21 06:53:29', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(23, 'cn=kohlenhydrate,ou=servers,ou=systems,ou=keks,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '11', 11, 'eg', '2006-09-29 09:37:30', 7, 0, 0, 0, 0, 26, '0', NULL, 8, 'N'),
(24, 'cn=Mineralstoffe,ou=workstations,ou=systems,ou=keks,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'dfsaaaaaaaaaaaaaaaaaaaaaaaaaaa', '2006-10-18 11:21:03', 7, 0, 0, 0, 0, 27, '0', NULL, 16, 'N'),
(25, 'cn=Ammoniumcarbonat,ou=phones,ou=systems,ou=keks,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '11', 11, 'asdf', '2006-09-29 10:34:32', 7, 0, 0, 0, 0, 21, '0', NULL, 16, 'N'),
(26, 'cn=Kakaomasse,ou=netdevices,ou=systems,ou=keks,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '11', 11, 'ikk', '2006-09-29 10:41:12', 7, 0, 0, 0, 0, 27, '0', NULL, 8, 'N'),
(27, 'cn=afs-22,ou=servers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, '-Ã¶', '2006-11-30 13:31:50', 7, 0, 0, 0, 0, 26, '0', NULL, 16, 'N'),
(28, 'cn=Netzwerkkomponente,ou=netdevices,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '11', 0, '', '2006-11-29 14:43:00', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(29, 'cn=vserver-04.intranet.gonicus.de,ou=servers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'asdfasdf', '2006-11-30 12:48:22', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(30, 'cn=okulele,ou=servers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '21', 21, 'No comment', '2007-01-23 14:20:48', 11, 0, 0, 0, 0, 21, '0', NULL, 16, 'N'),
(31, 'cn=keksesindlecker45,ou=workstations,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, 'sdfsdfaasdf', '2007-01-16 08:26:35', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N'),
(32, 'cn=phoneTesterAparatz,ou=phones,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, '', '2007-01-15 08:21:39', 5, 0, 0, 0, 0, 18, '0', NULL, 12, 'N'),
(39, 'cn=terminal,ou=terminals,ou=systems,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '', 0, '', '2007-05-21 07:02:42', 6, 0, 0, 0, 0, 28, '0', NULL, 12, 'N');



CREATE TABLE `glpi_config` (
  `ID` int(11) NOT NULL auto_increment,
  `ldap_port` varchar(10) NOT NULL default '389',
  `num_of_events` varchar(200) NOT NULL default '',
  `jobs_at_login` varchar(200) NOT NULL default '',
  `sendexpire` varchar(200) NOT NULL default '',
  `cut` varchar(200) NOT NULL default '',
  `expire_events` varchar(200) NOT NULL default '',
  `list_limit` varchar(200) NOT NULL default '',
  `version` varchar(200) NOT NULL default '',
  `logotxt` varchar(200) NOT NULL default '',
  `root_doc` varchar(200) NOT NULL default '',
  `event_loglevel` varchar(200) NOT NULL default '',
  `mailing` varchar(200) NOT NULL default '',
  `imap_auth_server` varchar(200) NOT NULL default '',
  `imap_host` varchar(200) NOT NULL default '',
  `ldap_host` varchar(200) NOT NULL default '',
  `ldap_basedn` varchar(200) NOT NULL default '',
  `ldap_rootdn` varchar(200) NOT NULL default '',
  `ldap_pass` varchar(200) NOT NULL default '',
  `admin_email` varchar(200) NOT NULL default '',
  `mailing_resa_all_admin` varchar(200) NOT NULL default '0',
  `mailing_resa_user` varchar(200) NOT NULL default '1',
  `mailing_resa_admin` varchar(200) NOT NULL default '1',
  `mailing_signature` varchar(200) NOT NULL default '',
  `mailing_new_admin` varchar(200) NOT NULL default '',
  `mailing_followup_admin` varchar(200) NOT NULL default '',
  `mailing_finish_admin` varchar(200) NOT NULL default '',
  `mailing_new_all_admin` varchar(200) NOT NULL default '',
  `mailing_followup_all_admin` varchar(200) NOT NULL default '',
  `mailing_finish_all_admin` varchar(200) NOT NULL default '',
  `mailing_new_all_normal` varchar(200) NOT NULL default '',
  `mailing_followup_all_normal` varchar(200) NOT NULL default '',
  `mailing_finish_all_normal` varchar(200) NOT NULL default '',
  `mailing_new_attrib` varchar(200) NOT NULL default '',
  `mailing_followup_attrib` varchar(200) NOT NULL default '',
  `mailing_finish_attrib` varchar(200) NOT NULL default '',
  `mailing_new_user` varchar(200) NOT NULL default '',
  `mailing_followup_user` varchar(200) NOT NULL default '',
  `mailing_finish_user` varchar(200) NOT NULL default '',
  `ldap_field_name` varchar(200) NOT NULL default '',
  `ldap_field_email` varchar(200) NOT NULL default '',
  `ldap_field_location` varchar(200) NOT NULL default '',
  `ldap_field_realname` varchar(200) NOT NULL default '',
  `ldap_field_phone` varchar(200) NOT NULL default '',
  `ldap_condition` varchar(255) NOT NULL default '',
  `permit_helpdesk` varchar(200) NOT NULL default '',
  `default_language` varchar(255) NOT NULL default 'french',
  `priority_1` varchar(200) NOT NULL default '#fff2f2',
  `priority_2` varchar(200) NOT NULL default '#ffe0e0',
  `priority_3` varchar(200) NOT NULL default '#ffcece',
  `priority_4` varchar(200) NOT NULL default '#ffbfbf',
  `priority_5` varchar(200) NOT NULL default '#ffadad',
  `date_fiscale` date NOT NULL default '2005-12-31',
  `cartridges_alarm` int(11) NOT NULL default '10',
  `cas_host` varchar(255) NOT NULL default '',
  `cas_port` varchar(255) NOT NULL default '',
  `cas_uri` varchar(255) NOT NULL default '',
  `planning_begin` time NOT NULL default '08:00:00',
  `planning_end` time NOT NULL default '20:00:00',
  `utf8_conv` int(11) NOT NULL default '0',
  `auto_assign` enum('0','1') NOT NULL default '0',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;


INSERT INTO `glpi_config` (`ID`, `ldap_port`, `num_of_events`, `jobs_at_login`, `sendexpire`, `cut`, `expire_events`, `list_limit`, `version`, `logotxt`, `root_doc`, `event_loglevel`, `mailing`, `imap_auth_server`, `imap_host`, `ldap_host`, `ldap_basedn`, `ldap_rootdn`, `ldap_pass`, `admin_email`, `mailing_resa_all_admin`, `mailing_resa_user`, `mailing_resa_admin`, `mailing_signature`, `mailing_new_admin`, `mailing_followup_admin`, `mailing_finish_admin`, `mailing_new_all_admin`, `mailing_followup_all_admin`, `mailing_finish_all_admin`, `mailing_new_all_normal`, `mailing_followup_all_normal`, `mailing_finish_all_normal`, `mailing_new_attrib`, `mailing_followup_attrib`, `mailing_finish_attrib`, `mailing_new_user`, `mailing_followup_user`, `mailing_finish_user`, `ldap_field_name`, `ldap_field_email`, `ldap_field_location`, `ldap_field_realname`, `ldap_field_phone`, `ldap_condition`, `permit_helpdesk`, `default_language`, `priority_1`, `priority_2`, `priority_3`, `priority_4`, `priority_5`, `date_fiscale`, `cartridges_alarm`, `cas_host`, `cas_port`, `cas_uri`, `planning_begin`, `planning_end`, `utf8_conv`, `auto_assign`) VALUES 
(1, '389', '10', '1', '1', '80', '30', '15', ' 0.6', 'GLPI powered by indepnet', '/glpi', '5', '0', '', '', '', '', '', '', 'admsys@xxxxx.fr', '0', '1', '1', 'SIGNATURE', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', 'uid', 'mail', 'physicaldeliveryofficename', 'cn', 'telephonenumber', '', '', 'english', '#fff2f2', '#ffe0e0', '#ffcece', '#ffbfbf', '#ffadad', '2005-12-31', 10, '', '', '', '08:00:00', '20:00:00', 0, '0');



CREATE TABLE `glpi_connect_wire` (
  `ID` int(11) NOT NULL auto_increment,
  `end1` int(11) NOT NULL default '0',
  `end2` int(11) NOT NULL default '0',
  `type` tinyint(4) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `end1_1` (`end1`,`end2`,`type`),
  KEY `end1` (`end1`),
  KEY `end2` (`end2`),
  KEY `type` (`type`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=10 ;


INSERT INTO `glpi_connect_wire` (`ID`, `end1`, `end2`, `type`) VALUES 
(1, 1, 1, 4),
(5, 2, 17, 4),
(4, 2, 13, 4),
(7, 2, 23, 4),
(8, 2, 24, 4),
(9, 2, 29, 4);



CREATE TABLE `glpi_consumables` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_glpi_consumables_type` int(11) default NULL,
  `date_in` date default NULL,
  `date_out` date default NULL,
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_cartridges_type` (`FK_glpi_consumables_type`),
  KEY `date_in` (`date_in`),
  KEY `date_out` (`date_out`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_consumables_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `ref` varchar(255) NOT NULL default '',
  `location` int(11) NOT NULL default '0',
  `type` tinyint(4) NOT NULL default '0',
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `tech_num` int(11) default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `comments` text NOT NULL,
  `alarm` tinyint(4) NOT NULL default '10',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `tech_num` (`tech_num`),
  KEY `deleted` (`deleted`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_contact_enterprise` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_enterprise` int(11) NOT NULL default '0',
  `FK_contact` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_enterprise` (`FK_enterprise`,`FK_contact`),
  KEY `FK_enterprise_2` (`FK_enterprise`),
  KEY `FK_contact` (`FK_contact`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_contacts` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `phone` varchar(30) NOT NULL default '',
  `phone2` varchar(30) NOT NULL default '',
  `fax` varchar(30) NOT NULL default '',
  `email` varchar(255) NOT NULL default '',
  `type` tinyint(4) NOT NULL default '1',
  `comments` text NOT NULL,
  `deleted` enum('Y','N') NOT NULL default 'N',
  PRIMARY KEY  (`ID`),
  KEY `deleted` (`deleted`),
  KEY `type` (`type`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_contract_device` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_contract` int(11) NOT NULL default '0',
  `FK_device` int(11) NOT NULL default '0',
  `device_type` tinyint(4) NOT NULL default '0',
  `is_template` enum('0','1') NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_contract` (`FK_contract`,`FK_device`,`device_type`),
  KEY `FK_contract_2` (`FK_contract`),
  KEY `FK_device` (`FK_device`,`device_type`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_contract_enterprise` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_enterprise` int(11) NOT NULL default '0',
  `FK_contract` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_enterprise` (`FK_enterprise`,`FK_contract`),
  KEY `FK_enterprise_2` (`FK_enterprise`),
  KEY `FK_contract` (`FK_contract`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_contracts` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `num` varchar(255) NOT NULL default '',
  `cost` float NOT NULL default '0',
  `contract_type` int(11) NOT NULL default '0',
  `begin_date` date default NULL,
  `duration` tinyint(4) NOT NULL default '0',
  `notice` tinyint(4) NOT NULL default '0',
  `periodicity` tinyint(4) NOT NULL default '0',
  `facturation` tinyint(4) NOT NULL default '0',
  `bill_type` int(11) NOT NULL default '0',
  `comments` text NOT NULL,
  `compta_num` varchar(255) NOT NULL default '',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `week_begin_hour` time NOT NULL default '00:00:00',
  `week_end_hour` time NOT NULL default '00:00:00',
  `saturday_begin_hour` time NOT NULL default '00:00:00',
  `saturday_end_hour` time NOT NULL default '00:00:00',
  `saturday` enum('Y','N') NOT NULL default 'N',
  `monday_begin_hour` time NOT NULL default '00:00:00',
  `monday_end_hour` time NOT NULL default '00:00:00',
  `monday` enum('Y','N') NOT NULL default 'N',
  `device_countmax` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `contract_type` (`contract_type`),
  KEY `begin_date` (`begin_date`),
  KEY `bill_type` (`bill_type`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_device_case` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(255) NOT NULL default '',
  `format` enum('Grand','Moyen','Micro') NOT NULL default 'Moyen',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=5 ;





CREATE TABLE `glpi_device_control` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(255) NOT NULL default '',
  `interface` enum('IDE','SATA','SCSI','USB') NOT NULL default 'IDE',
  `raid` enum('Y','N') NOT NULL default 'Y',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;





CREATE TABLE `glpi_device_drive` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(255) NOT NULL default '',
  `is_writer` enum('Y','N') NOT NULL default 'Y',
  `speed` varchar(30) NOT NULL default '',
  `interface` enum('IDE','SATA','SCSI') NOT NULL default 'IDE',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;





CREATE TABLE `glpi_device_gfxcard` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(120) NOT NULL default '',
  `ram` varchar(10) NOT NULL default '',
  `interface` enum('AGP','PCI','PCI-X','Other') NOT NULL default 'AGP',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;





CREATE TABLE `glpi_device_hdd` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(100) NOT NULL default '',
  `rpm` varchar(20) NOT NULL default '',
  `interface` int(11) NOT NULL default '0',
  `cache` varchar(20) NOT NULL default '',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;





CREATE TABLE `glpi_device_iface` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(120) NOT NULL default '',
  `bandwidth` varchar(20) NOT NULL default '',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;





CREATE TABLE `glpi_device_moboard` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(100) NOT NULL default '',
  `chipset` varchar(120) NOT NULL default '',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;


INSERT INTO `glpi_device_moboard` (`ID`, `designation`, `chipset`, `comment`, `FK_glpi_enterprise`, `specif_default`) VALUES 
(1, 'NVidia Nforce 9', 'Nforce', 'kein', 2, '');



CREATE TABLE `glpi_device_pci` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(255) NOT NULL default '',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;





CREATE TABLE `glpi_device_power` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(255) NOT NULL default '',
  `power` varchar(20) NOT NULL default '',
  `atx` enum('Y','N') NOT NULL default 'Y',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;





CREATE TABLE `glpi_device_processor` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(120) NOT NULL default '',
  `frequence` int(11) NOT NULL default '0',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;





CREATE TABLE `glpi_device_ram` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(100) NOT NULL default '',
  `frequence` varchar(8) NOT NULL default '',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  `type` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;





CREATE TABLE `glpi_device_sndcard` (
  `ID` int(11) NOT NULL auto_increment,
  `designation` varchar(120) NOT NULL default '',
  `type` varchar(100) NOT NULL default '',
  `comment` text NOT NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `specif_default` varchar(250) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;





CREATE TABLE `glpi_doc_device` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_doc` int(11) NOT NULL default '0',
  `FK_device` int(11) NOT NULL default '0',
  `device_type` tinyint(4) NOT NULL default '0',
  `is_template` enum('0','1') NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_doc` (`FK_doc`,`FK_device`,`device_type`),
  KEY `FK_doc_2` (`FK_doc`),
  KEY `FK_device` (`FK_device`,`device_type`),
  KEY `is_template` (`is_template`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=55 ;


INSERT INTO `glpi_doc_device` (`ID`, `FK_doc`, `FK_device`, `device_type`, `is_template`) VALUES 
(17, 1, 1, 1, '0'),
(8, 1, 2, 1, '0'),
(50, 1, 1, 3, '0'),
(51, 4, 5, 3, '0'),
(52, 1, 7, 3, '0'),
(53, 4, 7, 3, '0'),
(54, 1, 8, 3, '0');



CREATE TABLE `glpi_docs` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `filename` varchar(255) NOT NULL default '',
  `rubrique` int(11) NOT NULL default '0',
  `mime` varchar(30) NOT NULL default '',
  `date_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `comment` text NOT NULL,
  `deleted` enum('Y','N') NOT NULL default 'N',
  `link` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `rubrique` (`rubrique`),
  KEY `deleted` (`deleted`),
  KEY `date_mod` (`date_mod`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=6 ;


INSERT INTO `glpi_docs` (`ID`, `name`, `filename`, `rubrique`, `mime`, `date_mod`, `comment`, `deleted`, `link`) VALUES 
(1, 'Anleitung', 'class_mail-methods-kolab.inc', 0, 'application/octet-stream', '2006-01-25 11:23:16', 'leeer', 'N', ''),
(4, 'Hardware handbuch', 'sieve-php.inc', 0, 'application/octet-stream', '2006-09-29 11:01:52', 'Steht alles drin.\r\n42.342 Seiten', 'N', '');



CREATE TABLE `glpi_dropdown_cartridge_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=14 ;


INSERT INTO `glpi_dropdown_cartridge_type` (`ID`, `name`) VALUES 
(3, 'Color blue'),
(4, 'Color combination RGB'),
(6, 'Laser Cartridge Red'),
(8, 'Laser Cartridge Green'),
(9, 'Laser Cartridge combination RGB'),
(10, 'Color Black'),
(11, 'Color Black /& Rgb');



CREATE TABLE `glpi_dropdown_consumable_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_contact_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_contract_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=8 ;


INSERT INTO `glpi_dropdown_contract_type` (`ID`, `name`) VALUES 
(1, 'Pr&#234;t'),
(2, 'Location'),
(3, 'Leasing'),
(4, 'Assurances'),
(5, 'Maintenance Hardware'),
(6, 'Maintenance Software'),
(7, 'Prestation');



CREATE TABLE `glpi_dropdown_domain` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_enttype` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_firmware` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_hdd_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;


INSERT INTO `glpi_dropdown_hdd_type` (`ID`, `name`) VALUES 
(1, 'IDE'),
(2, 'SATA'),
(3, 'SCSI');



CREATE TABLE `glpi_dropdown_iface` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_kbcategories` (
  `ID` int(11) NOT NULL auto_increment,
  `parentID` int(11) NOT NULL default '0',
  `name` varchar(255) NOT NULL default '',
  `completename` text NOT NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `parentID_2` (`parentID`,`name`),
  KEY `parentID` (`parentID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_locations` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `parentID` int(11) NOT NULL default '0',
  `completename` text NOT NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `name` (`name`,`parentID`),
  KEY `parentID` (`parentID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


INSERT INTO `glpi_dropdown_locations` (`ID`, `name`, `parentID`, `completename`) VALUES 
(1, 'lala', 0, 'chk'),
(2, 'ldala', 1, 'chacka');



CREATE TABLE `glpi_dropdown_model` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_netpoint` (
  `ID` int(11) NOT NULL auto_increment,
  `location` int(11) NOT NULL default '0',
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `location` (`location`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_network` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_os` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=21 ;


INSERT INTO `glpi_dropdown_os` (`ID`, `name`) VALUES 
(5, 'Windows 3.1'),
(6, 'Debian Sarge'),
(7, 'Debian Woody'),
(11, 'Windows 98 se'),
(10, 'Debian SID'),
(14, 'Windows Server 2003'),
(18, 'Windows 99');



CREATE TABLE `glpi_dropdown_ram_type` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=5 ;


INSERT INTO `glpi_dropdown_ram_type` (`ID`, `name`) VALUES 
(1, 'EDO'),
(2, 'DDR'),
(3, 'SDRAM'),
(4, 'SDRAM-2');



CREATE TABLE `glpi_dropdown_rubdocs` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_state` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


INSERT INTO `glpi_dropdown_state` (`ID`, `name`) VALUES 
(1, 'asdf'),
(2, 'asdfasdf');



CREATE TABLE `glpi_dropdown_tracking_category` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_dropdown_vlan` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_enterprises` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(50) NOT NULL default '',
  `type` int(11) NOT NULL default '0',
  `address` text NOT NULL,
  `website` varchar(100) NOT NULL default '',
  `phonenumber` varchar(20) NOT NULL default '',
  `comments` text NOT NULL,
  `deleted` enum('Y','N') NOT NULL default 'N',
  `fax` varchar(255) NOT NULL default '',
  `email` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `deleted` (`deleted`),
  KEY `type` (`type`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=18 ;


INSERT INTO `glpi_enterprises` (`ID`, `name`, `type`, `address`, `website`, `phonenumber`, `comments`, `deleted`, `fax`, `email`) VALUES 
(8, 'Microsoft', 0, 'Neverlandranche', 'www.google.de', '0001', 'No comment', '', '02', '02@01.00'),
(12, 'Cherry', 0, '3 Tasten hÃ¶her als ''n''   4 te links.', 'http://cherry_oder_so_aehnlich.com', 'N/A', 'N/A', '', 'N/A', 'N/A@N/A.N/A'),
(16, 'GONICUS GmbH', 0, '', 'http://www.gonicus.de', '', 'fg', '', '', '');



CREATE TABLE `glpi_event_log` (
  `ID` int(11) NOT NULL auto_increment,
  `item` int(11) NOT NULL default '0',
  `itemtype` varchar(20) NOT NULL default '',
  `date` datetime NOT NULL default '0000-00-00 00:00:00',
  `service` varchar(20) default NULL,
  `level` tinyint(4) NOT NULL default '0',
  `message` text NOT NULL,
  PRIMARY KEY  (`ID`),
  KEY `comp` (`item`),
  KEY `date` (`date`),
  KEY `itemtype` (`itemtype`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=105 ;


INSERT INTO `glpi_event_log` (`ID`, `item`, `itemtype`, `date`, `service`, `level`, `message`) VALUES 
(90, 0, 'dropdowns', '2006-01-18 14:58:40', 'setup', 5, 'glpi added a value to a dropdown.'),
(89, 0, 'dropdowns', '2006-01-18 14:58:31', 'setup', 5, 'glpi added a value to a dropdown.'),
(88, 0, 'dropdowns', '2006-01-18 14:58:22', 'setup', 5, 'glpi added a value to a dropdown.'),
(87, 0, 'dropdowns', '2006-01-18 14:58:07', 'setup', 5, 'glpi added a value to a dropdown.'),
(86, 0, 'dropdowns', '2006-01-18 14:56:55', 'setup', 5, 'glpi added a value to a dropdown.'),
(85, 0, 'dropdowns', '2006-01-18 14:56:40', 'setup', 5, 'glpi added a value to a dropdown.'),
(84, 0, 'dropdowns', '2006-01-18 14:56:28', 'setup', 5, 'glpi added a value to a dropdown.'),
(83, 0, 'dropdowns', '2006-01-18 14:56:12', 'setup', 5, 'glpi added a value to a dropdown.'),
(82, 1, 'cartridges', '2006-01-18 14:55:35', 'inventory', 4, 'glpi added item HP Deskjet 80000 Green.'),
(81, -1, 'system', '2006-01-18 14:49:46', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(80, -1, 'system', '2006-01-18 14:47:02', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(79, -1, 'system', '2006-01-17 12:23:40', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(78, 4, 'documents', '2006-01-17 09:51:47', 'document', 4, 'glpi associate device.'),
(77, 4, 'documents', '2006-01-17 09:51:38', 'document', 4, 'glpi associate device.'),
(76, 4, 'printers', '2006-01-17 09:41:51', 'inventory', 4, 'glpi updated item.'),
(75, 0, 'dropdowns', '2006-01-17 09:41:28', 'setup', 5, 'glpi added a value to a dropdown.'),
(74, 0, 'dropdowns', '2006-01-17 09:41:14', 'setup', 5, 'glpi added a value to a dropdown.'),
(73, 0, 'dropdowns', '2006-01-17 09:41:03', 'setup', 5, 'glpi added a value to a dropdown.'),
(72, 4, 'printers', '2006-01-17 08:56:20', 'inventory', 4, 'glpi added Name.'),
(71, -1, 'system', '2006-01-17 08:48:04', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(31, -1, 'system', '2005-12-23 09:38:42', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(32, 26, 'computers', '2005-12-23 09:39:05', 'inventory', 4, 'glpi deleted item.'),
(33, 0, 'Devices', '2005-12-23 14:53:31', 'inventory', 4, 'glpi added gzt.'),
(34, -1, 'system', '2006-01-03 07:49:38', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(35, -1, 'system', '2006-01-03 08:16:04', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(36, 11, 'networking', '2006-01-03 08:30:08', 'inventory', 4, 'glpi added item name .'),
(37, 2, 'Devices', '2006-01-03 12:07:52', 'inventory', 4, 'glpi updated item.'),
(38, 0, 'Devices', '2006-01-03 13:12:53', 'inventory', 4, 'glpi added asdfasdf.'),
(39, 0, 'Devices', '2006-01-03 13:27:36', 'inventory', 4, 'glpi added amd.'),
(40, 0, 'Devices', '2006-01-03 13:33:00', 'inventory', 4, 'glpi added Duschaube.'),
(41, 0, 'Devices', '2006-01-03 13:46:11', 'inventory', 4, 'glpi added name.'),
(42, 0, 'Devices', '2006-01-03 14:03:42', 'inventory', 4, 'glpi added Hdd.'),
(43, 0, 'Devices', '2006-01-03 14:42:23', 'inventory', 4, 'glpi added name.'),
(44, 0, 'Devices', '2006-01-03 14:59:12', 'inventory', 4, 'glpi added Controllers.'),
(45, 0, 'Devices', '2006-01-03 15:06:59', 'inventory', 4, 'glpi added gfxcard.'),
(46, 0, 'Devices', '2006-01-03 15:18:33', 'inventory', 4, 'glpi added power.'),
(47, 0, 'Devices', '2006-01-03 15:22:32', 'inventory', 4, 'glpi added pheripgherals.'),
(48, -1, 'system', '2006-01-04 08:15:26', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(49, 2, 'Devices', '2006-01-04 08:16:05', 'inventory', 4, 'glpi updated item.'),
(50, 26, 'computers', '2006-01-04 08:22:28', 'inventory', 4, 'glpi Unlinked a device from computer 26.'),
(51, -1, 'system', '2006-01-04 11:23:06', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(52, 26, 'documents', '2006-01-04 11:23:47', 'document', 4, 'glpi associate device.'),
(53, 2, 'documents', '2006-01-04 11:25:40', 'document', 4, 'glpi added item tester.'),
(54, 3, 'documents', '2006-01-04 11:26:09', 'document', 4, 'glpi added item .'),
(55, 4, 'documents', '2006-01-04 11:26:29', 'document', 4, 'glpi added item .'),
(56, -1, 'system', '2006-01-04 15:22:51', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(57, 3, 'documents', '2006-01-04 15:41:50', 'document', 4, 'glpi updated item.'),
(58, -1, 'system', '2006-01-05 10:17:59', 'login', 3, 'glpi  logged in from 10.3.64.43.'),
(59, 26, 'documents', '2006-01-05 10:18:52', 'document', 4, 'glpi associate device.'),
(60, -1, 'system', '2006-01-06 08:24:59', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(61, -1, 'system', '2006-01-06 09:21:47', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(62, -1, 'system', '2006-01-06 10:03:43', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(63, 0, 'dropdowns', '2006-01-06 10:33:17', 'setup', 5, 'glpi added a value to a dropdown.'),
(64, 0, 'dropdowns', '2006-01-06 10:41:57', 'setup', 5, 'glpi added a value to a dropdown.'),
(65, 0, 'dropdowns', '2006-01-06 10:42:12', 'setup', 5, 'glpi added a value to a dropdown.'),
(66, 5, 'monitors', '2006-01-06 14:38:56', 'inventory', 4, 'glpi added asdfasdf.'),
(67, 27, 'computers', '2006-01-06 14:39:39', 'inventory', 5, 'glpi connected item.'),
(68, -1, 'system', '2006-01-16 12:08:10', 'login', 1, 'failed login: root'),
(69, -1, 'system', '2006-01-16 12:08:23', 'login', 1, 'failed login: admin'),
(70, -1, 'system', '2006-01-16 12:08:35', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(91, 0, 'dropdowns', '2006-01-18 14:58:57', 'setup', 5, 'glpi added a value to a dropdown.'),
(92, 0, 'dropdowns', '2006-01-18 14:59:08', 'setup', 5, 'glpi added a value to a dropdown.'),
(93, 0, 'dropdowns', '2006-01-18 14:59:21', 'setup', 5, 'glpi added a value to a dropdown.'),
(94, 1, 'cartridges', '2006-01-18 15:00:07', 'inventory', 4, 'glpi updated item.'),
(95, 1, 'cartridges', '2006-01-18 15:02:10', 'inventory', 4, 'glpi added a cartridge.'),
(96, 1, 'cartridges', '2006-01-18 15:03:19', 'inventory', 4, 'glpi added a cartridge.'),
(97, 1, 'cartridges', '2006-01-18 15:31:39', 'inventory', 5, 'glpi installed cartridge.'),
(98, -1, 'system', '2006-01-19 08:33:27', 'login', 3, 'glpi logged in from 10.3.64.43.'),
(99, 1, 'cartridges', '2006-01-19 08:34:32', 'inventory', 4, 'glpi added 17 cartridge.'),
(100, 0, 'infocom', '2006-01-19 08:34:54', 'financial', 4, 'glpi added infocoms.'),
(101, 1, 'cartridges', '2006-01-19 10:21:27', 'inventory', 4, 'glpi updated item.'),
(102, 1, 'cartridges', '2006-01-19 10:21:33', 'inventory', 4, 'glpi added 1 cartridge.'),
(103, 2, 'cartridges', '2006-01-19 10:22:45', 'inventory', 4, 'glpi added item Laserjet3.'),
(104, 2, 'cartridges', '2006-01-19 10:23:16', 'inventory', 4, 'glpi updated item.');



CREATE TABLE `glpi_followups` (
  `ID` int(11) NOT NULL auto_increment,
  `tracking` int(11) default NULL,
  `date` datetime default NULL,
  `author` int(11) NOT NULL default '0',
  `contents` text,
  PRIMARY KEY  (`ID`),
  KEY `tracking` (`tracking`),
  KEY `author` (`author`),
  KEY `date` (`date`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_infocoms` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_device` int(11) NOT NULL default '0',
  `device_type` tinyint(4) NOT NULL default '0',
  `buy_date` date NOT NULL default '0000-00-00',
  `use_date` date NOT NULL default '0000-00-00',
  `warranty_duration` tinyint(4) NOT NULL default '0',
  `warranty_info` varchar(255) NOT NULL default '',
  `FK_enterprise` int(11) default NULL,
  `num_commande` varchar(50) NOT NULL default '',
  `bon_livraison` varchar(50) NOT NULL default '',
  `num_immo` varchar(50) NOT NULL default '',
  `value` float NOT NULL default '0',
  `warranty_value` float default NULL,
  `amort_time` tinyint(4) NOT NULL default '0',
  `amort_type` varchar(20) NOT NULL default '',
  `amort_coeff` float NOT NULL default '0',
  `comments` text NOT NULL,
  `facture` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_device` (`FK_device`,`device_type`),
  KEY `FK_enterprise` (`FK_enterprise`),
  KEY `buy_date` (`buy_date`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;


INSERT INTO `glpi_infocoms` (`ID`, `FK_device`, `device_type`, `buy_date`, `use_date`, `warranty_duration`, `warranty_info`, `FK_enterprise`, `num_commande`, `bon_livraison`, `num_immo`, `value`, `warranty_value`, `amort_time`, `amort_type`, `amort_coeff`, `comments`, `facture`) VALUES 
(1, 10, 19, '0000-00-00', '0000-00-00', 0, '', NULL, '', '', '', 0, NULL, 0, '', 0, '', '');



CREATE TABLE `glpi_inst_software` (
  `ID` int(11) NOT NULL auto_increment,
  `cID` int(11) NOT NULL default '0',
  `license` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `cID` (`cID`),
  KEY `sID` (`license`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_kbitems` (
  `ID` int(11) NOT NULL auto_increment,
  `categoryID` int(11) NOT NULL default '0',
  `question` text NOT NULL,
  `answer` text NOT NULL,
  `faq` enum('yes','no') NOT NULL default 'no',
  PRIMARY KEY  (`ID`),
  KEY `categoryID` (`categoryID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_licenses` (
  `ID` int(15) NOT NULL auto_increment,
  `sID` int(15) NOT NULL default '0',
  `serial` varchar(255) NOT NULL default '',
  `expire` date default NULL,
  `oem` enum('N','Y') NOT NULL default 'N',
  `oem_computer` int(11) NOT NULL default '0',
  `buy` enum('Y','N') NOT NULL default 'Y',
  PRIMARY KEY  (`ID`),
  KEY `sID` (`sID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_links` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_links_device` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_links` int(11) NOT NULL default '0',
  `device_type` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `device_type_2` (`device_type`,`FK_links`),
  KEY `device_type` (`device_type`),
  KEY `FK_links` (`FK_links`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_monitors` (
  `ID` int(10) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `date_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `contact` varchar(255) NOT NULL default '',
  `contact_num` varchar(255) NOT NULL default '',
  `tech_num` int(11) NOT NULL default '0',
  `comments` text NOT NULL,
  `serial` varchar(255) NOT NULL default '',
  `otherserial` varchar(255) NOT NULL default '',
  `size` int(3) NOT NULL default '0',
  `flags_micro` tinyint(4) NOT NULL default '0',
  `flags_speaker` tinyint(4) NOT NULL default '0',
  `flags_subd` tinyint(4) NOT NULL default '0',
  `flags_bnc` tinyint(4) NOT NULL default '0',
  `location` int(11) default NULL,
  `type` int(11) default NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `is_global` enum('0','1') NOT NULL default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `is_template` enum('0','1') NOT NULL default '0',
  `tplname` varchar(255) default NULL,
  PRIMARY KEY  (`ID`),
  KEY `ID` (`ID`),
  KEY `type` (`type`),
  KEY `location` (`location`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `deleted` (`deleted`),
  KEY `is_template` (`is_template`),
  KEY `tech_num` (`tech_num`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


INSERT INTO `glpi_monitors` (`ID`, `name`, `date_mod`, `contact`, `contact_num`, `tech_num`, `comments`, `serial`, `otherserial`, `size`, `flags_micro`, `flags_speaker`, `flags_subd`, `flags_bnc`, `location`, `type`, `FK_glpi_enterprise`, `is_global`, `deleted`, `is_template`, `tplname`) VALUES 
(2, 'asdfasdf', '0000-00-00 00:00:00', '', '', 0, '', '', '', 0, 0, 0, 0, 0, 0, 0, 0, '', '', '', '');



CREATE TABLE `glpi_networking` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(30) NOT NULL default '',
  `ram` varchar(10) NOT NULL default '',
  `serial` varchar(50) NOT NULL default '',
  `otherserial` varchar(50) NOT NULL default '',
  `contact` varchar(30) NOT NULL default '',
  `contact_num` varchar(30) NOT NULL default '',
  `tech_num` int(11) NOT NULL default '0',
  `date_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `comments` text NOT NULL,
  `location` int(11) default NULL,
  `domain` int(11) NOT NULL default '0',
  `network` int(11) NOT NULL default '0',
  `type` int(11) default NULL,
  `firmware` int(11) default NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `is_template` enum('0','1') NOT NULL default '0',
  `tplname` varchar(255) default NULL,
  `ifmac` varchar(30) NOT NULL default '',
  `ifaddr` varchar(30) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `location` (`location`),
  KEY `type` (`type`),
  KEY `firmware` (`firmware`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `deleted` (`deleted`),
  KEY `is_template` (`is_template`),
  KEY `tech_num` (`tech_num`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=12 ;


INSERT INTO `glpi_networking` (`ID`, `name`, `ram`, `serial`, `otherserial`, `contact`, `contact_num`, `tech_num`, `date_mod`, `comments`, `location`, `domain`, `network`, `type`, `firmware`, `FK_glpi_enterprise`, `deleted`, `is_template`, `tplname`, `ifmac`, `ifaddr`) VALUES 
(10, '', '', '', '', '', '', 0, '0000-00-00 00:00:00', '', NULL, 0, 0, NULL, NULL, 0, 'N', '1', 'Blank Template', '', ''),
(11, '', '', '', '', '', '', 0, '2006-01-03 08:30:08', '', 0, 0, 0, 0, 0, 0, 'N', '0', NULL, '', '');



CREATE TABLE `glpi_networking_ports` (
  `ID` int(11) NOT NULL auto_increment,
  `on_device` int(11) NOT NULL default '0',
  `device_type` tinyint(4) NOT NULL default '0',
  `logical_number` int(11) NOT NULL default '0',
  `name` char(30) NOT NULL default '',
  `ifaddr` char(30) NOT NULL default '',
  `ifmac` char(30) NOT NULL default '',
  `iface` int(11) default NULL,
  `netpoint` int(11) default NULL,
  PRIMARY KEY  (`ID`),
  KEY `on_device` (`on_device`,`device_type`),
  KEY `netpoint` (`netpoint`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_networking_vlan` (
  `ID` int(11) NOT NULL auto_increment,
  `FK_port` int(11) NOT NULL default '0',
  `FK_vlan` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `FK_port_2` (`FK_port`,`FK_vlan`),
  KEY `FK_port` (`FK_port`),
  KEY `FK_vlan` (`FK_vlan`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_networking_wire` (
  `ID` int(11) NOT NULL auto_increment,
  `end1` int(11) NOT NULL default '0',
  `end2` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `end1_1` (`end1`,`end2`),
  KEY `end1` (`end1`),
  KEY `end2` (`end2`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_peripherals` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `date_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `contact` varchar(255) NOT NULL default '',
  `contact_num` varchar(255) NOT NULL default '',
  `tech_num` int(11) NOT NULL default '0',
  `comments` text NOT NULL,
  `serial` varchar(255) NOT NULL default '',
  `otherserial` varchar(255) NOT NULL default '',
  `location` int(11) NOT NULL default '0',
  `type` int(11) NOT NULL default '0',
  `brand` varchar(255) NOT NULL default '',
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `is_global` enum('0','1') NOT NULL default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `is_template` enum('0','1') NOT NULL default '0',
  `tplname` varchar(255) default NULL,
  PRIMARY KEY  (`ID`),
  KEY `type` (`type`),
  KEY `location` (`location`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `deleted` (`deleted`),
  KEY `is_template` (`is_template`),
  KEY `tech_num` (`tech_num`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


INSERT INTO `glpi_peripherals` (`ID`, `name`, `date_mod`, `contact`, `contact_num`, `tech_num`, `comments`, `serial`, `otherserial`, `location`, `type`, `brand`, `FK_glpi_enterprise`, `is_global`, `deleted`, `is_template`, `tplname`) VALUES 
(1, '', '0000-00-00 00:00:00', '', '', 0, '', '', '', 0, 0, '', 0, '0', 'N', '1', 'Blank Template'),
(2, 'gf5200', '2005-12-13 09:34:32', 'Herr Herr', '0231', 4, 'f', '45875486468', '4554', 0, 0, 'GC', 1, '0', 'N', '0', NULL);



CREATE TABLE `glpi_printers` (
  `ID` int(10) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `date_mod` datetime NOT NULL default '0000-00-00 00:00:00',
  `contact` varchar(255) NOT NULL default '',
  `contact_num` varchar(255) NOT NULL default '',
  `tech_num` int(11) NOT NULL default '0',
  `serial` varchar(255) NOT NULL default '',
  `otherserial` varchar(255) NOT NULL default '',
  `flags_serial` tinyint(4) NOT NULL default '0',
  `flags_par` tinyint(4) NOT NULL default '0',
  `flags_usb` tinyint(4) NOT NULL default '0',
  `comments` text NOT NULL,
  `ramSize` varchar(6) NOT NULL default '',
  `location` int(11) default NULL,
  `domain` int(11) NOT NULL default '0',
  `network` int(11) NOT NULL default '0',
  `type` int(11) default NULL,
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `is_template` enum('0','1') NOT NULL default '0',
  `tplname` varchar(255) default NULL,
  `initial_pages` varchar(30) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `id` (`ID`),
  KEY `location` (`location`),
  KEY `type` (`type`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `deleted` (`deleted`),
  KEY `is_template` (`is_template`),
  KEY `tech_num` (`tech_num`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=9 ;


INSERT INTO `glpi_printers` (`ID`, `name`, `date_mod`, `contact`, `contact_num`, `tech_num`, `serial`, `otherserial`, `flags_serial`, `flags_par`, `flags_usb`, `comments`, `ramSize`, `location`, `domain`, `network`, `type`, `FK_glpi_enterprise`, `deleted`, `is_template`, `tplname`, `initial_pages`) VALUES 
(1, 'cn=demohost,ou=printers,ou=systems,dc=gonicus,dc=de', '2006-02-01 12:48:33', '', '', 0, '', '', 0, 0, 0, '', '0', 0, 0, 0, 2, 8, 'N', '0', NULL, '0'),
(2, 'cn=drucker4,ou=printers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '2006-09-19 09:49:33', '', '', 0, '', '', 0, 0, 0, '', '0', 0, 0, 0, 3, 12, 'N', '0', NULL, '0'),
(3, 'cn=drucker6,ou=printers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '2006-11-29 14:53:41', '', '', 0, '', '', 0, 0, 0, '', '0', 0, 0, 0, 3, 12, 'N', '0', NULL, '0'),
(4, 'cn=FettsÃ¤uren,ou=printers,ou=systems,ou=keks,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '2006-09-28 09:17:45', '', '', 0, '', '', 1, 1, 1, '', '0', 0, 0, 0, 3, 12, 'N', '0', NULL, '0'),
(5, 'cn=Brennwert,ou=printers,ou=systems,ou=keks,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '2006-09-29 11:02:10', '', '', 0, '', '', 1, 1, 1, 'dfsaaaaaaaaaaaaaaaaa', '0', 0, 0, 0, 3, 12, 'N', '0', NULL, '0'),
(7, 'cn=atz23,ou=printers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '2006-11-29 10:19:48', '', '', 19, '', '', 1, 1, 1, 'ursssszzzzzzzzzzzzzzzzzzzzzzzzzzzzzz', '0', 0, 0, 0, 3, 12, 'N', '0', NULL, '0'),
(8, 'cn=drucker2,ou=printers,ou=systems,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '2006-11-29 10:25:26', '', '', 0, '', '', 1, 1, 1, 'test', '0', 0, 0, 0, 3, 12, 'N', '0', NULL, '0');



CREATE TABLE `glpi_repair_item` (
  `ID` int(11) NOT NULL auto_increment,
  `device_type` tinyint(4) NOT NULL default '0',
  `id_device` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `device_type` (`device_type`),
  KEY `device_type_2` (`device_type`,`id_device`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_reservation_item` (
  `ID` int(11) NOT NULL auto_increment,
  `device_type` tinyint(4) NOT NULL default '0',
  `id_device` int(11) NOT NULL default '0',
  `comments` text NOT NULL,
  PRIMARY KEY  (`ID`),
  KEY `device_type` (`device_type`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;


INSERT INTO `glpi_reservation_item` (`ID`, `device_type`, `id_device`, `comments`) VALUES 
(1, 1, 20, '');



CREATE TABLE `glpi_reservation_resa` (
  `ID` bigint(20) NOT NULL auto_increment,
  `id_item` int(11) NOT NULL default '0',
  `begin` datetime NOT NULL default '0000-00-00 00:00:00',
  `end` datetime NOT NULL default '0000-00-00 00:00:00',
  `id_user` int(11) NOT NULL default '0',
  `comment` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  KEY `id_item` (`id_item`),
  KEY `id_user` (`id_user`),
  KEY `begin` (`begin`),
  KEY `end` (`end`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_software` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(200) NOT NULL default '',
  `version` varchar(20) NOT NULL default '',
  `comments` text,
  `location` int(11) default NULL,
  `tech_num` int(11) NOT NULL default '0',
  `platform` int(11) default NULL,
  `is_update` enum('N','Y') NOT NULL default 'N',
  `update_software` int(11) NOT NULL default '-1',
  `FK_glpi_enterprise` int(11) NOT NULL default '0',
  `deleted` enum('Y','N') NOT NULL default 'N',
  `is_template` enum('0','1') NOT NULL default '0',
  `tplname` varchar(255) default NULL,
  `date_mod` datetime default NULL,
  PRIMARY KEY  (`ID`),
  KEY `platform` (`platform`),
  KEY `location` (`location`),
  KEY `FK_glpi_enterprise` (`FK_glpi_enterprise`),
  KEY `deleted` (`deleted`),
  KEY `is_template` (`is_template`),
  KEY `date_mod` (`date_mod`),
  KEY `tech_num` (`tech_num`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=7 ;


INSERT INTO `glpi_software` (`ID`, `name`, `version`, `comments`, `location`, `tech_num`, `platform`, `is_update`, `update_software`, `FK_glpi_enterprise`, `deleted`, `is_template`, `tplname`, `date_mod`) VALUES 
(6, '', '', NULL, NULL, 0, NULL, 'N', -1, 0, 'N', '1', 'Blank Template', NULL);



CREATE TABLE `glpi_state_item` (
  `ID` int(11) NOT NULL auto_increment,
  `device_type` tinyint(4) NOT NULL default '0',
  `id_device` int(11) NOT NULL default '0',
  `state` int(11) default '1',
  `is_template` enum('0','1') NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `device_type` (`device_type`),
  KEY `device_type_2` (`device_type`,`id_device`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_tracking` (
  `ID` int(11) NOT NULL auto_increment,
  `date` datetime default NULL,
  `closedate` datetime NOT NULL default '0000-00-00 00:00:00',
  `status` enum('new','old') default NULL,
  `author` int(11) NOT NULL default '0',
  `assign` int(11) NOT NULL default '0',
  `assign_type` tinyint(4) NOT NULL default '0',
  `device_type` int(11) NOT NULL default '1',
  `computer` int(11) default NULL,
  `contents` text,
  `priority` tinyint(4) NOT NULL default '1',
  `is_group` enum('no','yes') NOT NULL default 'no',
  `uemail` varchar(100) default NULL,
  `emailupdates` varchar(4) default NULL,
  `realtime` float NOT NULL default '0',
  `category` int(11) NOT NULL default '0',
  PRIMARY KEY  (`ID`),
  KEY `computer` (`computer`),
  KEY `author` (`author`),
  KEY `assign` (`assign`),
  KEY `date` (`date`),
  KEY `closedate` (`closedate`),
  KEY `status` (`status`),
  KEY `category` (`category`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_tracking_planning` (
  `ID` bigint(20) NOT NULL auto_increment,
  `id_tracking` int(11) NOT NULL default '0',
  `id_assign` int(11) NOT NULL default '0',
  `begin` datetime NOT NULL default '0000-00-00 00:00:00',
  `end` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`ID`),
  KEY `id_tracking` (`id_tracking`),
  KEY `begin` (`begin`),
  KEY `end` (`end`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_type_computers` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=30 ;


INSERT INTO `glpi_type_computers` (`ID`, `name`) VALUES 
(26, 'SuperDupfer'),
(27, 'Server'),
(18, 'System tester'),
(19, 'Windows workstation'),
(21, 'Network device'),
(28, 'Microcontroller');



CREATE TABLE `glpi_type_docs` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  `ext` varchar(10) NOT NULL default '',
  `icon` varchar(255) NOT NULL default '',
  `mime` varchar(100) NOT NULL default '',
  `upload` enum('Y','N') NOT NULL default 'Y',
  `date_mod` datetime default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `extension` (`ext`),
  KEY `upload` (`upload`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=54 ;


INSERT INTO `glpi_type_docs` (`ID`, `name`, `ext`, `icon`, `mime`, `upload`, `date_mod`) VALUES 
(1, 'JPEG', 'jpg', 'jpg-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(2, 'PNG', 'png', 'png-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(3, 'GIF', 'gif', 'gif-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(4, 'BMP', 'bmp', 'bmp-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(5, 'Photoshop', 'psd', 'psd-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(6, 'TIFF', 'tif', 'tif-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(7, 'AIFF', 'aiff', 'aiff-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(8, 'Windows Media', 'asf', 'asf-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(9, 'Windows Media', 'avi', 'avi-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(44, 'C source', 'c', '', '', 'Y', '2004-12-13 19:47:22'),
(27, 'RealAudio', 'rm', 'rm-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(16, 'Midi', 'mid', 'mid-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(17, 'QuickTime', 'mov', 'mov-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(18, 'MP3', 'mp3', 'mp3-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(19, 'MPEG', 'mpg', 'mpg-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(20, 'Ogg Vorbis', 'ogg', 'ogg-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(24, 'QuickTime', 'qt', 'qt-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(10, 'BZip', 'bz2', 'bz2-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(25, 'RealAudio', 'ra', 'ra-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(26, 'RealAudio', 'ram', 'ram-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(11, 'Word', 'doc', 'doc-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(12, 'DjVu', 'djvu', '', '', 'Y', '2004-12-13 19:47:21'),
(42, 'MNG', 'mng', '', '', 'Y', '2004-12-13 19:47:22'),
(13, 'PostScript', 'eps', 'ps-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(14, 'GZ', 'gz', 'gz-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(37, 'WAV', 'wav', 'wav-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(15, 'HTML', 'html', 'html-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(34, 'Flash', 'swf', '', '', 'Y', '2004-12-13 19:47:22'),
(21, 'PDF', 'pdf', 'pdf-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(22, 'PowerPoint', 'ppt', 'ppt-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(23, 'PostScript', 'ps', 'ps-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(40, 'Windows Media', 'wmv', '', '', 'Y', '2004-12-13 19:47:22'),
(28, 'RTF', 'rtf', 'rtf-dist.png', '', 'Y', '2004-12-13 19:47:21'),
(29, 'StarOffice', 'sdd', 'sdd-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(30, 'StarOffice', 'sdw', 'sdw-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(31, 'Stuffit', 'sit', 'sit-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(43, 'Adobe Illustrator', 'ai', 'ai-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(32, 'OpenOffice Impress', 'sxi', 'sxi-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(33, 'OpenOffice', 'sxw', 'sxw-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(46, 'DVI', 'dvi', 'dvi-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(35, 'TGZ', 'tgz', 'tgz-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(36, 'texte', 'txt', 'txt-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(49, 'RedHat/Mandrake/SuSE', 'rpm', 'rpm-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(38, 'Excel', 'xls', 'xls-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(39, 'XML', 'xml', 'xml-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(41, 'Zip', 'zip', 'zip-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(45, 'Debian', 'deb', 'deb-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(47, 'C header', 'h', '', '', 'Y', '2004-12-13 19:47:22'),
(48, 'Pascal', 'pas', '', '', 'Y', '2004-12-13 19:47:22'),
(50, 'OpenOffice Calc', 'sxc', 'sxc-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(51, 'LaTeX', 'tex', 'tex-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(52, 'GIMP multi-layer', 'xcf', 'xcf-dist.png', '', 'Y', '2004-12-13 19:47:22'),
(53, 'JPEG', 'jpeg', 'jpg-dist.png', '', 'Y', '2005-03-07 22:23:17');



CREATE TABLE `glpi_type_monitors` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;


INSERT INTO `glpi_type_monitors` (`ID`, `name`) VALUES 
(1, 'testoe');



CREATE TABLE `glpi_type_networking` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_type_peripherals` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;





CREATE TABLE `glpi_type_printers` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(255) default NULL,
  PRIMARY KEY  (`ID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=15 ;


INSERT INTO `glpi_type_printers` (`ID`, `name`) VALUES 
(2, 'Tintenstrahl'),
(3, 'Farb Laser'),
(5, 'Not known'),
(6, 'Nadeldrucker Farbe'),
(10, 'Nadeldrucker'),
(11, 'Laser'),
(12, 'Photodrucker A3'),
(14, 'Tintentrahl');



CREATE TABLE `glpi_users` (
  `ID` int(11) NOT NULL auto_increment,
  `name` varchar(80) NOT NULL default '',
  `password` varchar(80) NOT NULL default '',
  `password_md5` varchar(80) NOT NULL default '',
  `email` varchar(80) NOT NULL default '',
  `phone` varchar(100) default NULL,
  `type` enum('normal','admin','post-only','super-admin') NOT NULL default 'normal',
  `realname` varchar(255) NOT NULL default '',
  `can_assign_job` enum('yes','no') NOT NULL default 'no',
  `location` int(11) default NULL,
  `tracking_order` enum('yes','no') NOT NULL default 'no',
  `language` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`ID`),
  UNIQUE KEY `name` (`name`),
  KEY `type` (`type`),
  KEY `name_2` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=22 ;


INSERT INTO `glpi_users` (`ID`, `name`, `password`, `password_md5`, `email`, `phone`, `type`, `realname`, `can_assign_job`, `location`, `tracking_order`, `language`) VALUES 
(1, 'cn=GOsa main administrator admin,ou=people,dc=gonicus,dc=de', '', '', 'asdfasdf@asdfasdf.de', '333333333', 'normal', '', 'no', NULL, 'no', ''),
(2, 'cn=Getester Hainz,ou=people,dc=gonicus,dc=de', '', '', 'gtest@blafasel.org', NULL, 'normal', '', 'no', NULL, 'no', ''),
(3, 'cn=adff dendnen,ou=people,dc=gonicus,dc=de', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(4, 'cn=tttttttttttt testttttttttttttttttt,ou=people,ou=Diesisteinelangeabteilung,ou=', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(5, 'cn=asdff33 asdf333,ou=people,ou=tester,ou=Vertrieb,dc=gonicus,dc=de', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(6, 'cn=Florian Schiessl,ou=people,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(7, 'cn=blub blab,ou=people,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(8, 'cn=Florian test,ou=people,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '233-92742', 'normal', '', 'no', NULL, 'no', ''),
(9, 'uid=pppppp,ou=people,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(10, 'cn=acltest2 acltest2,ou=people,ou=checkTheFAIStuff,ou=DasTestDep,o=Landeshauptst', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(11, 'cn=acltest acltest,ou=people,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', 'acltest@gonicus.de', '123123', 'normal', '', 'no', NULL, 'no', ''),
(12, 'cn=Max Mustermann,ou=people,ou=PrinterLosesMember,ou=DasTestDep,o=Landeshauptsta', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(13, 'cn=Susanne Neuhaus,ou=people,ou=PrinterLosesMember,ou=DasTestDep,o=Landeshauptst', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(14, '', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(15, 'cn=Florian Meier,ou=people,ou=PrinterLosesMember,ou=DasTestDep,o=Landeshauptstad', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(16, 'cn=abtielung abtielung,ou=people,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '7890', 'normal', '', 'no', NULL, 'no', ''),
(17, 'cn=Franz Keller,ou=people,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', 'ableis@gonicus.de', NULL, 'normal', '', 'no', NULL, 'no', ''),
(18, 'cn=Jan Wenzel,ou=people,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', NULL, 'normal', '', 'no', NULL, 'no', ''),
(19, 'cn=acl2 acl2,ou=people,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', '', '22222222222222', 'normal', '', 'no', NULL, 'no', ''),
(20, 'cn=a11 a11,ou=people,ou=DasTestDep,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', 'a11@gonicus.de', '311112', 'normal', '', 'no', NULL, 'no', ''),
(21, 'cn=Admin Admin,ou=people,o=Landeshauptstadt MÃ¼nchen,c=de', '', '', 'ableis@gonicus.de', '1234', 'normal', '', 'no', NULL, 'no', '');

