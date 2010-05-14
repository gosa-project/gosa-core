<?php 
class migrate_groupRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_groupRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
