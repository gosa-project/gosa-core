<?php 
class migrate_workstationRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=gotoWorkstation)%s)";
}
?>
