<?php

/**
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2025 GONICUS GmbH
 *
 * ID: $$Id: class_target_list.inc 9850 2008-03-14 14:15:12Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class userGroupSelect extends management
{
    // Tab definition 
    protected $skipFooter = true;
    protected $skipHeader = true;

    public $plHeadline = 'User and group selection';

    function __construct($config, $ui, array $dns = [])
    {
        $this->config = $config;
        $this->ui = $ui;
        $this->storagePoints = [get_ou('core', 'userRDN'), get_ou('core', 'groupRDN')];

        // Build filter
        if (session::global_is_set(get_class($this) . '_filter')) {
            $filter = session::global_get(get_class($this) . '_filter');
            $filter->setConstraint(['dn' => $dns]);
        } else {
            $filter = new filter(get_template_path('selectUserGroup-filter.xml', true, dirname(__FILE__)));
            $filter->setObjectStorage($this->storagePoints);
            $filter->setConstraint(['dn' => $dns]);
        }
        $this->setFilter($filter);

        // Build headpage
        $headpage = new listing(get_template_path('selectUserGroup-list.xml', true, dirname(__FILE__)));
        $headpage->setFilter($filter);
        parent::__construct($config, $ui, "object", $headpage);
    }
}
