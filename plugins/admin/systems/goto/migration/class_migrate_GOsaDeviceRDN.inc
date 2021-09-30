<?php 
class migrate_GOsaDeviceRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=device)%s)";
}
?>
