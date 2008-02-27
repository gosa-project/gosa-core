create database gosa_log;
use gosa_log;


CREATE TABLE `gosa_locations` (
  `id` int(11) NOT NULL auto_increment,
  `location` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


CREATE TABLE `gosa_log` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `timestamp` int(10) NOT NULL,
  `user` text NOT NULL,
  `action` varchar(255) NOT NULL,
  `objecttype` varchar(255) NOT NULL,
  `object` text NOT NULL,
  `changes` blob NOT NULL,
  `result` varchar(255) NOT NULL,
  `location_id` int(11) NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `action` (`action`),
  KEY `timestamp` (`timestamp`),
  KEY `objecttype` (`objecttype`),
  KEY `result` (`result`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=7 ;

