#!/usr/bin/php -q
<?php
/*
/*
 * This code is part of GOsa (https://gosa.gonicus.de)
 * Copyright (C) 2005 Jan Wenzel
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

$current_dir=trim(`pwd`);
include($current_dir.'/include/functions_test.inc');

// Build Array of called functions
$array= test_defined_functions();

// Write List to file
$file= $current_dir.'/include/functions_list.inc';

// If file does not exist, create it
if(!file_exists($file)) {
	touch($file);
}

// Write Array to file
if(is_writable($file)) {
	// Open filehandle
	$fh= fopen($file,'w');
	if($fh != null) {
		fwrite($fh,"return(\n".var_export($array,true).");");
		fclose($fh);
	}
}
?>
