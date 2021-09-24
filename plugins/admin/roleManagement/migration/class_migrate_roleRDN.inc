<?php 
class migrate_roleRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=organizationalRole)%s)";
}
?>
