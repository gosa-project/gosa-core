<?php 
class migrate_printerRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=gotoPrinter)%s)";
}
?>
