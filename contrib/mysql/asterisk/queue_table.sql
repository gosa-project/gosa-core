#
# Table structure for table `queue_table`
#

CREATE TABLE queue_table (
 name VARCHAR(128) PRIMARY KEY,
 musiconhold VARCHAR(128),
 announce VARCHAR(128),
 context VARCHAR(128),
 timeout INT(11),
 monitor_join BOOL,
 monitor_format VARCHAR(128),
 queue_youarenext VARCHAR(128),
 queue_thereare VARCHAR(128),
 queue_callswaiting VARCHAR(128),
 queue_holdtime VARCHAR(128),
 queue_minutes VARCHAR(128),
 queue_seconds VARCHAR(128),
 queue_lessthan VARCHAR(128),
 queue_thankyou VARCHAR(128),
 queue_reporthold VARCHAR(128),
 announce_frequency INT(11),
 announce_round_seconds INT(11),
 announce_holdtime VARCHAR(128),
 retry INT(11),
 wrapuptime INT(11),
 maxlen INT(11),
 servicelevel INT(11),
 strategy VARCHAR(128),
 joinempty VARCHAR(128),
 leavewhenempty VARCHAR(128),
 eventmemberstatus BOOL,
 eventwhencalled BOOL,
 reportholdtime BOOL,
 memberdelay INT(11),
 weight INT(11),
 timeoutrestart BOOL
);

