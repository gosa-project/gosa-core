#
# Table structure for table `extensions_table`
#

CREATE TABLE `extensions_table` (
 `id` int(11) NOT NULL auto_increment,
 `context` varchar(20) NOT NULL default '',
 `exten` varchar(20) NOT NULL default '',
 `priority` tinyint(4) NOT NULL default '0',
 `app` varchar(20) NOT NULL default '',
 `appdata` varchar(128) NOT NULL default '',
 PRIMARY KEY  (`context`,`exten`,`priority`),
 KEY `id` (`id`)
) TYPE=MyISAM;

