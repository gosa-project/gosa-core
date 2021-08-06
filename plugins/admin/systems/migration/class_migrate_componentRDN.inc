<?php 
class migrate_componentRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=ieee802Device)%s)";
}
?>
