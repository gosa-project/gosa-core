create database gofax;
use gofax;

CREATE TABLE faxlog (
	id                    VARCHAR(20) NOT NULL,
	status                INTEGER NOT NULL,
	status_message        TEXT,
	uid                   VARCHAR(20) NOT NULL,
	queuing_time          TIMESTAMP NOT NULL,
	sender_msn            VARCHAR(100),
	sender_id             VARCHAR(100),
	receiver_msn          VARCHAR(100),
	receiver_id           VARCHAR(100),
	transfer_time         INTEGER,
	pages 		      INTEGER
	);

CREATE TABLE faxdata (
	id 		      VARCHAR(20) PRIMARY KEY,
	fax_data 	      BLOB
	);

GRANT INSERT,SELECT ON gofax.faxlog TO logger@localhost IDENTIFIED BY 'somemysqlpass';
GRANT INSERT,SELECT,DELETE ON gofax.faxdata TO logger@localhost IDENTIFIED BY 'somemysqlpass';
