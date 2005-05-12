
create database golog with owner gomon;

create table golog (
	time_stamp DATETIME,
  	host VARCHAR(50),
	message VARCHAR(255),
	log_level VARCHAR(15),
	matched_dn VARCHAR(255),
	matched_ts DATETIME
	);

GRANT ALL PRIVILEGES ON golog TO gomon WITH GRANT OPTION;
