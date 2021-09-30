<?php 
class migrate_systemIncomingRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=nothing)%s)";
}
?>
