<?php 
class migrate_aclRoleRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_aclRoleRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
