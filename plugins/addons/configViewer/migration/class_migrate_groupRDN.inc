<?php
class migrate_groupRDN extends migrateRDN implements propertyMigration 
{
    protected $filter = "(&(objectClass=posixGroup)%s)";
}
?>
