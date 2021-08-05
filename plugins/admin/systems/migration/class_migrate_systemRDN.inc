<?php 
class migrate_systemRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=nothing)%s)";
}
?>
