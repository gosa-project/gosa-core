<?php 
class migrate_applicationRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=gosaApplication)%s)";
}
?>
