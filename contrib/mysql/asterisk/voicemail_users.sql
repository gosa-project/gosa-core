#
# Table structure for table `voicemail_users`
#

CREATE TABLE `voicemail_users` (
 `uniqueid` int(11) NOT NULL auto_increment,
 `customer_id` int(11) NOT NULL default '0',
 `context` varchar(50) NOT NULL default '',
 `mailbox` int(5) NOT NULL default '0',
 `password` varchar(4) NOT NULL default '0',
 `fullname` varchar(50) NOT NULL default '',
 `email` varchar(50) NOT NULL default '',
 `pager` varchar(50) NOT NULL default '',
 `stamp` timestamp(14) NOT NULL,
 PRIMARY KEY  (`uniqueid`),
 KEY `mailbox_context` (`mailbox`,`context`)
) TYPE=MyISAM; 


