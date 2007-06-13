#
# Table structure for table `iax_buddies`
#

CREATE TABLE iax_buddies (
       name varchar(30) primary key NOT NULL,
       username varchar(30), 
       type varchar(6) NOT NULL, 
       secret varchar(50), 
       md5secret varchar(32), 
       dbsecret varchar(100), 
       notransfer varchar(10), 
       inkeys varchar(100),
       outkey varchar(100),
       auth varchar(100), 
       accountcode varchar(100), 
       amaflags varchar(100), 
       callerid varchar(100), 
       context varchar(100), 
       defaultip varchar(15), 
       host varchar(31) NOT NULL default 'dynamic', 
       language char(5), 
       mailbox varchar(50), 
       deny varchar(95),
       permit varchar(95),  
       qualify varchar(4), 
       disallow varchar(100), 
       allow varchar(100), 
       ipaddr varchar(15), 
       port integer default 0,
       regseconds integer default 0
);
CREATE UNIQUE INDEX iax_buddies_username_idx ON iax_buddies(username); 

