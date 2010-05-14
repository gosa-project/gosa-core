<?php 
class migrate_userRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_userRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
