<?php 
class migrate_deviceRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=gotoDevice)%s)";
}
?>
