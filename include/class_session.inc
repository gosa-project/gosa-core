<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
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

class session {

	public static function get_session_size()
	{

	}

	public static function get_element_size()
	{
	
	}

	public static function is_set($name)
	{
		return(isset($_SESSION[$name]));
	}

	public static function set($name,$value)
	{
		$_SESSION[$name] = $value;
	}

	public static function &get($name)
	{
		$ret = &$_SESSION[$name];
		return($ret);
	}

	public static function delete($name)
	{
		if(session::is_set($name)){
			unset($_SESSION[$name]);
		}
	}

	public static function un_set($name)
	{
		return(session::delete($name));
	}

	public static function start()
	{
		/* Set cookie lifetime to one day (The parameter is in seconds ) */
		session_set_cookie_params(24*60*60);

		/* Set cache limter to one day (parameter is minute !!)*/
		session_cache_expire(60*24);  // default is 180

		/* Set session max lifetime, to prevent the garbage collector to delete session before timeout.
		   !! The garbage collector is a cron job on debian systems, the cronjob will fetch the timeout from
		   the php.ini, so if you use debian, you must hardcode session.gc_maxlifetime in your php.ini */
		ini_set("session.gc_maxlifetime",24*60*60);
		session_start();

    /* Check for changed browsers and bail out */
    if (isset($_SESSION['HTTP_USER_AGENT']))
    {
      if ($_SESSION['HTTP_USER_AGENT'] !=  md5($_SERVER['HTTP_USER_AGENT'])) {
        session_destroy();
        session_start();
      }
    } else {
      $_SESSION['HTTP_USER_AGENT'] = md5($_SERVER['HTTP_USER_AGENT']);
    }

    /* Regenerate ID to increase security */
    if (!isset($_SESSION['started'])){
      session_regenerate_id();
      $_SESSION['started'] = true;
    }
	}

	public static function destroy()
	{
		@session_destroy();
	}

	public static function set_lifetime($seconds = -1)
	{
		echo "Not implemented yet";
	}

	public static function &get_all()
	{
		$ret = &$_SESSION;
		return($ret);
	}
}

?>
