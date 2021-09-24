<?php 
class migrate_sudoRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=sudoRole)%s)";
}
?>
