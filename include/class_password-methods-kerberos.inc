<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2004 Fabian Hickert 

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

class passwordMethodkerberos extends passwordMethod
{

	function passwordMethodkerberos($config)  
	{
    $this->config=$config;
	}

  function is_available()
  {
    if (function_exists('kadm5_init_with_password')){
      return(true);
    }else{
      return false;
    }
  }

	function generate_hash($pwd)
	{
		$cfg= $this->config->data['SERVERS']['KERBEROS'];

		if (!function_exists('kadm5_init_with_password'))
		{
			print_red (_("Warning: Can't set kerberos password. Your PHP version has no kerberos support, password has not been changed."));
		} 
		else 
		{
			$handle = kadm5_init_with_password($cfg['SERVER'],
					$cfg['REALM'], $cfg['ADMIN'], $cfg['PASSWORD']);

			if ($handle === FALSE)
			{
				print_red (_("Kerberos database communication failed!"));
			}

			$ret= kadm5_chpass_principal($handle, $this->attrs['uid'][0]."@".$cfg['REALM'],$password);

			if ($ret === FALSE)
			{
				print_red (_("Changing password in kerberos database failed!"));
			}

			kadm5_destroy($handle);

			$newpass= "{kerberos}".$this->attrs['uid'][0]."@".$cfg['REALM'];

			return $newpass;       
		}
	}
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
