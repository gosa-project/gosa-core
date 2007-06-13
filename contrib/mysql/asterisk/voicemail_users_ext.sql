#
# Table structure for table `voicemail_users`
# Extended version
#

CREATE TABLE `voicemail_users` (
 `uniqueid` int(11) NOT NULL auto_increment,
 `customer_id` varchar(11) NOT NULL default '0',
 `context` varchar(50) NOT NULL default '',
 `mailbox` varchar(11) NOT NULL default '0',
 `password` varchar(5) NOT NULL default '0',
 `fullname` varchar(150) NOT NULL default '',
 `email` varchar(50) NOT NULL default '',
 `pager` varchar(50) NOT NULL default '',
 `tz` varchar(10) NOT NULL default 'central',
 `attach` varchar(4) NOT NULL default 'yes',
 `saycid` varchar(4) NOT NULL default 'yes',
 `dialout` varchar(10) NOT NULL default '',
 `callback` varchar(10) NOT NULL default '',
 `review` varchar(4) NOT NULL default 'no',
 `operator` varchar(4) NOT NULL default 'no',
 `envelope` varchar(4) NOT NULL default 'no',
 `sayduration` varchar(4) NOT NULL default 'no',
 `saydurationm` tinyint(4) NOT NULL default '1',
 `sendvoicemail` varchar(4) NOT NULL default 'no',
 `delete` varchar(4) NOT NULL default 'no',
 `nextaftercmd` varchar(4) NOT NULL default 'yes',
 `forcename` varchar(4) NOT NULL default 'no',
 `forcegreetings` varchar(4) NOT NULL default 'no',
 `hidefromdir` varchar(4) NOT NULL default 'yes',
 `stamp` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
 PRIMARY KEY  (`uniqueid`),
 KEY `mailbox_context` (`mailbox`,`context`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 ; 

