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

class tripleSelect extends management {

    protected $skipFooter = TRUE;
    protected $skipHeader = TRUE;

	function __construct($config, $ui, $excludeDN) {

        $this->config = $config;
        $this->ui = $ui;
		$this->storagePoints = array_unique(array(
						get_ou("core", "userRDN"),
						get_ou("netgroupSystem", "netgroupSystemRDN"),
						get_ou("servgeneric", "serverRDN"),
						get_ou("workgeneric", "workstationRDN"),
						get_ou("termgeneric", "terminalRDN"),
						get_ou("componentGeneric", "componentRDN")));

        $myFilter = new filter(get_template_path("triple-filter.xml", true, dirname(__FILE__)));
        $myFilter->setObjectStorage($this->storagePoints);
        $this->setFilter($myFilter);

        // Build headpage
        $myHeadpage = new listing(get_template_path("triple-list.xml", true, dirname(__FILE__)));
        $myHeadpage->setFilter($myFilter);
        parent::__construct($config, $ui, "triples", $myHeadpage);
    }
}
?>
