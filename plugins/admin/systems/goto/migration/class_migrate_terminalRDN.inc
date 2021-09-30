<?php 
class migrate_terminalRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=gotoTerminal)%s)";
}
?>
