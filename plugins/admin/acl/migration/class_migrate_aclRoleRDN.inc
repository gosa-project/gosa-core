<?php 
class migrate_aclRoleRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(objectClass=gosaRole)%s)";
}
?>
