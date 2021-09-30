<?php 
class migrate_mimetypeRDN extends migrateRDN implements propertyMigration
{
    protected $filter = "(&(objectClass=gotoMimeType)%s)";
}
?>
