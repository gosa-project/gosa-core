<?php 
class migrate_userRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=gosaAccount)(!(objectClass=sambaSamAccount))%s)";
}
?>
