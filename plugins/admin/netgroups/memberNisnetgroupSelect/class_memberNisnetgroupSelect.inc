<?php

/* This code is part of GOsa-contrib (https://oss.gonicus.de/labs/gosa-contrib)
 * Copyright (C) 2011 Alejandro Escanero Blanco (aescanero@gmail.com)
 *
 * Ported to GOsa 2.7 by Fabian Hickert (hickert@gonicus.de)
 * Copyright (C) 2012 GONICUS GmbH
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

class memberNisnetgroupSelect extends management {

    protected $skipFooter = TRUE;
    protected $skipHeader = TRUE;
    private $myFilter;
    private $myHeadpage;

    function __construct($config, $ui) {
        $this->config = $config;
        $this->ui = $ui;

        $this->storagePoints = array(get_ou("netgroupSystem", "netgroupSystemRDN"));

        $myFilter = new filter(get_template_path("memberNisnetgroup-filter.xml", true, dirname(__FILE__)));
        $myFilter->setObjectStorage($this->storagePoints);
        $this->setFilter($myFilter);

        // Build headpage
        $myHeadpage = new listing(get_template_path("memberNisnetgroup-list.xml", true, dirname(__FILE__)));
        $myHeadpage->setFilter($myFilter);
        parent::__construct($config, $ui, "memberNisnetgroup", $myHeadpage);
    }

}

?>
