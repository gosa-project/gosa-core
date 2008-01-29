<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2008 Cajus Pollmeier <pollmeier@gonicus.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

class daemonCommunicator
{
	static function send($header, $to, $data= array())
	{
		$xml_message= "";

		/* Get communication object */
		$d= new gosaSupportDaemon(TRUE,10);

		/* Prepare data */
		foreach ($data as $key => $value){
			$xml_message.= "<$key>$value</$key>";
		}

		return $d->send("<xml><header>$header</header><target>$to</target>".$xml_message."</xml>");
	}
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
