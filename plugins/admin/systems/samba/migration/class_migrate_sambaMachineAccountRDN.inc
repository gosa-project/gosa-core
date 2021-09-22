<?php 
class migrate_sambaMachineAccountRDN extends migrateRDN implements propertyMigration
{
	protected $filter = '(&(objectClass=sambaSamAccount)(uid=*$)%s)';
}
?>
