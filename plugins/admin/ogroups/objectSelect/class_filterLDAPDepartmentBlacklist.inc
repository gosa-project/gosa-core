<?php

class filterLDAPDepartmentBlacklist  extends filterLDAPBlacklist
{

    static function query($base, $scope, $filter, $attributes, $category, $objectStorage = ""): array
    {
        $entries = filterLDAPBlacklist::query($base, $scope, $filter, $attributes, $category, []);
        foreach ($entries as $key => $entry) {
            $entries[$key]['cn'] = $entry['ou'];
            $entries[$key][$entries[$key]['count']] = 'cn';
            $entries[$key]['count']++;
        }
        return filterLDAPBlacklist::filterByBlacklist($entries);
    }
}
