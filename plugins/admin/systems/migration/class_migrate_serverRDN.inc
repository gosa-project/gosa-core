<?php 
class migrate_serverRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=goServer)%s)";
}
?>
