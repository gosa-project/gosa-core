<?php 
class migrate_faxBlocklistRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(&(|(objectClass=goFaxSBlocklist)(objectClass=goFaxRBlocklist))%s)";
}
?>
