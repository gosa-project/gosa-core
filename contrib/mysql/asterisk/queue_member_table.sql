#
# Table structure for table `queue_member_table`
#


CREATE TABLE queue_member_table (
 queue_name varchar(128),
 interface varchar(128),
 penalty INT(11),
 PRIMARY KEY (queue_name, interface)
);


