<?php 
class migrate_ogroupRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=gosaGroupOfNames)%s)";
}
?>
