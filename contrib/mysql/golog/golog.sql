
create database gomon;
use gomon;

create table golog (
	time_stamp DATETIME,
  	host VARCHAR(50),
	message VARCHAR(255),
	log_level VARCHAR(15),
	matched_dn VARCHAR(255),
	matched_ts DATETIME
	);

GRANT INSERT,SELECT,DELETE ON gomon.golog TO gomon@localhost IDENTIFIED BY 'somemysqlpass';
