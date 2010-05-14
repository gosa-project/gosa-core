<?php 
class migrate_ogroupRDN extends migrateRDN implements propertyMigration
{
	protected $filter = "(migrate_ogroupRDN(objectClass=gosaAccount)((objectClass=sambaSamAccount))%s)";
}
?>
