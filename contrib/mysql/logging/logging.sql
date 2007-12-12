create database gosa_log;
use gosa_log;

CREATE TABLE `gosa_log` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `timestamp` int(10) NOT NULL,
  `user` longtext NOT NULL,
  `action` varchar(255) NOT NULL,
  `objecttype` varchar(255) NOT NULL,
  `object` varchar(255) NOT NULL,
  `changes` blob NOT NULL,
  `result` varchar(255) NOT NULL,
  PRIMARY KEY  (`id`)
  KEY `timestamp` (`timestamp`)
  KEY `action` (`action`),
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

