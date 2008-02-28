
CREATE DATABASE `gosa_log`;
USE `gosa_log`;

DROP TABLE IF EXISTS `gosa_locations`;
CREATE TABLE `gosa_locations` (
  `id` int(11) NOT NULL auto_increment,
  `location` text NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

DROP TABLE IF EXISTS `gosa_log`;
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
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;


-- %HOST%               The name of the source host (e.g. locahost,vserver-01,%  WHERE % allows all)
-- %USER%               The username
-- %PWD%                The password for the new user

-- CREATE USER '%USER%'@'%HOST%' IDENTIFIED BY '%PWD%';

-- GRANT  SELECT , INSERT , UPDATE , DELETE
-- ON `gosa_log`.*
-- TO '%USER%'@'%HOST%'
-- IDENTIFIED BY '%PWD%';

