<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2005 Alejandro Escanero Blanco

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

class oxchangeAccount extends plugin
{
  /* Definitions */
  var $plHeadline= "Open-Xchange";
  var $plDescription= "This does something";

  /* GW attributes */
  var $OXAppointmentDays="500";
  var $OXTaskDays="5";
  var $OXTimeZone="GMT";
  var $mailEnabled="OK";
  var $dnModeValue = "";
  var $view_logged = FALSE;

  /* attribute list for save action */
  var $attributes= array("OXAppointmentDays", "OXTaskDays", "OXTimeZone","mailEnabled");
  var $objectclasses= array("OXUserObject");
  var $ReadOnly =false;
  var $oxconf = array();
  var $timezones=array(
      "Africa/Abidjan",
      "Africa/Accra",
      "Africa/Addis_Ababa",
      "Africa/Algiers",
      "Africa/Asmera",
      "Africa/Bamako",
      "Africa/Bangui",
      "Africa/Banjul",
      "Africa/Bissau",
      "Africa/Blantyre",
      "Africa/Brazzaville",
      "Africa/Bujumbura",
      "Africa/Cairo",
      "Africa/Casablanca",
      "Africa/Ceuta",
      "Africa/Conakry",
      "Africa/Dakar",
      "Africa/Dar_es_Salaam",
      "Africa/Djibouti",
      "Africa/Douala",
      "Africa/El_Aaiun",
      "Africa/Freetown",
      "Africa/Gaborone",
      "Africa/Harare",
      "Africa/Johannesburg",
      "Africa/Kampala",
      "Africa/Khartoum",
      "Africa/Kigali",
      "Africa/Kinshasa",
      "Africa/Lagos",
      "Africa/Libreville",
      "Africa/Lome",
      "Africa/Luanda",
      "Africa/Lubumbashi",
      "Africa/Lusaka",
      "Africa/Malabo",
      "Africa/Maputo",
      "Africa/Maseru",
      "Africa/Mbabane",
      "Africa/Mogadishu",
      "Africa/Monrovia",
      "Africa/Nairobi",
      "Africa/Ndjamena",
      "Africa/Niamey",
      "Africa/Nouakchott",
      "Africa/Ouagadougou",
      "Africa/Porto-Novo",
      "Africa/Sao_Tome",
      "Africa/Timbuktu",
      "Africa/Tripoli",
      "Africa/Tunis",
      "Africa/Windhoek",
      "America/Adak",
      "America/Anchorage",
      "America/Anguilla",
      "America/Antigua",
      "America/Araguaina",
      "America/Aruba",
      "America/Asuncion",
      "America/Atka",
      "America/Barbados",
      "America/Belem",
      "America/Belize",
      "America/Boa_Vista",
      "America/Bogota",
      "America/Boise",
      "America/Buenos_Aires",
      "America/Cambridge_Bay",
      "America/Cancun",
      "America/Caracas",
      "America/Catamarca",
      "America/Cayenne",
      "America/Cayman",
      "America/Chicago",
      "America/Chihuahua",
      "America/Cordoba",
      "America/Costa_Rica",
      "America/Cuiaba",
      "America/Curacao",
      "America/Danmarkshavn",
      "America/Dawson",
      "America/Dawson_Creek",
      "America/Denver",
      "America/Detroit",
      "America/Dominica",
      "America/Edmonton",
      "America/Eirunepe",
      "America/El_Salvador",
      "America/Ensenada",
      "America/Fortaleza",
      "America/Fort_Wayne",
      "America/Glace_Bay",
      "America/Godthab",
      "America/Goose_Bay",
      "America/Grand_Turk",
      "America/Grenada",
      "America/Guadeloupe",
      "America/Guatemala",
      "America/Guayaquil",
      "America/Guyana",
      "America/Halifax",
      "America/Havana",
      "America/Hermosillo",
      "America/Indiana",
      "America/Indiana/Indianapolis",
      "America/Indiana/Knox",
      "America/Indiana/Marengo",
      "America/Indiana/Vevay",
      "America/Indianapolis",
      "America/Inuvik",
      "America/Iqaluit",
      "America/Jamaica",
      "America/Jujuy",
      "America/Juneau",
      "America/Kentucky",
      "America/Kentucky/Louisville",
      "America/Kentucky/Monticello",
      "America/Knox_IN",
      "America/La_Paz",
      "America/Lima",
      "America/Los_Angeles",
      "America/Louisville",
      "America/Maceio",
      "America/Managua",
      "America/Manaus",
      "America/Martinique",
      "America/Mazatlan",
      "America/Mendoza",
      "America/Menominee",
      "America/Merida",
      "America/Mexico_City",
      "America/Miquelon",
      "America/Monterrey",
      "America/Montevideo",
      "America/Montreal",
      "America/Montserrat",
      "America/Nassau",
      "America/New_York",
      "America/Nipigon",
      "America/Nome",
      "America/Noronha",
      "America/North_Dakota",
      "America/North_Dakota/Center",
      "America/Panama",
      "America/Pangnirtung",
      "America/Paramaribo",
      "America/Phoenix",
      "America/Port-au-Prince",
      "America/Porto_Acre",
      "America/Porto_Velho",
      "America/Port_of_Spain",
      "America/Puerto_Rico",
      "America/Rainy_River",
      "America/Rankin_Inlet",
      "America/Recife",
      "America/Regina",
      "America/Rio_Branco",
      "America/Rosario",
      "America/Santiago",
      "America/Santo_Domingo",
      "America/Sao_Paulo",
      "America/Scoresbysund",
      "America/Shiprock",
      "America/St_Johns",
      "America/St_Kitts",
      "America/St_Lucia",
      "America/St_Thomas",
      "America/St_Vincent",
      "America/Swift_Current",
      "America/Tegucigalpa",
      "America/Thule",
      "America/Thunder_Bay",
      "America/Tijuana",
      "America/Tortola",
      "America/Vancouver",
      "America/Virgin",
      "America/Whitehorse",
      "America/Winnipeg",
      "America/Yakutat",
      "America/Yellowknife",
      "Antarctica/Casey",
      "Antarctica/Davis",
      "Antarctica/DumontDUrville",
      "Antarctica/Mawson",
      "Antarctica/McMurdo",
      "Antarctica/Palmer",
      "Antarctica/South_Pole",
      "Antarctica/Syowa",
      "Antarctica/Vostok",
      "Arctic/Longyearbyen",
      "Asia/Aden",
      "Asia/Almaty",
      "Asia/Amman",
      "Asia/Anadyr",
      "Asia/Aqtau",
      "Asia/Aqtobe",
      "Asia/Ashgabat",
      "Asia/Ashkhabad",
      "Asia/Baghdad",
      "Asia/Bahrain",
      "Asia/Baku",
      "Asia/Bangkok",
      "Asia/Beirut",
      "Asia/Bishkek",
      "Asia/Brunei",
      "Asia/Calcutta",
      "Asia/Choibalsan",
      "Asia/Chongqing",
      "Asia/Chungking",
      "Asia/Colombo",
      "Asia/Dacca",
      "Asia/Damascus",
      "Asia/Dhaka",
      "Asia/Dili",
      "Asia/Dubai",
      "Asia/Dushanbe",
      "Asia/Gaza",
      "Asia/Harbin",
      "Asia/Hong_Kong",
      "Asia/Hovd",
      "Asia/Irkutsk",
      "Asia/Istanbul",
      "Asia/Jakarta",
      "Asia/Jayapura",
      "Asia/Jerusalem",
      "Asia/Kabul",
      "Asia/Kamchatka",
      "Asia/Karachi",
      "Asia/Kashgar",
      "Asia/Katmandu",
      "Asia/Krasnoyarsk",
      "Asia/Kuala_Lumpur",
      "Asia/Kuching",
      "Asia/Kuwait",
      "Asia/Macao",
      "Asia/Macau",
      "Asia/Magadan",
      "Asia/Manila",
      "Asia/Muscat",
      "Asia/Nicosia",
      "Asia/Novosibirsk",
      "Asia/Omsk",
      "Asia/Oral",
      "Asia/Phnom_Penh",
      "Asia/Pontianak",
      "Asia/Pyongyang",
      "Asia/Qatar",
      "Asia/Qyzylorda",
      "Asia/Rangoon",
      "Asia/Riyadh",
      "Asia/Riyadh87",
      "Asia/Riyadh88",
      "Asia/Riyadh89",
      "Asia/Saigon",
      "Asia/Sakhalin",
      "Asia/Samarkand",
      "Asia/Seoul",
      "Asia/Shanghai",
      "Asia/Singapore",
      "Asia/Taipei",
      "Asia/Tashkent",
      "Asia/Tbilisi",
      "Asia/Tehran",
      "Asia/Tel_Aviv",
      "Asia/Thimbu",
      "Asia/Thimphu",
      "Asia/Tokyo",
      "Asia/Ujung_Pandang",
      "Asia/Ulaanbaatar",
      "Asia/Ulan_Bator",
      "Asia/Urumqi",
      "Asia/Vientiane",
      "Asia/Vladivostok",
      "Asia/Yakutsk",
      "Asia/Yekaterinburg",
      "Asia/Yerevan",
      "Atlantic/Azores",
      "Atlantic/Bermuda",
      "Atlantic/Canary",
      "Atlantic/Cape_Verde",
      "Atlantic/Faeroe",
      "Atlantic/Jan_Mayen",
      "Atlantic/Madeira",
      "Atlantic/Reykjavik",
      "Atlantic/South_Georgia",
      "Atlantic/Stanley",
      "Atlantic/St_Helena",
      "Australia/ACT",
      "Australia/Adelaide",
      "Australia/Brisbane",
      "Australia/Broken_Hill",
      "Australia/Canberra",
      "Australia/Darwin",
      "Australia/Hobart",
      "Australia/LHI",
      "Australia/Lindeman",
      "Australia/Lord_Howe",
      "Australia/Melbourne",
      "Australia/North",
      "Australia/NSW",
      "Australia/Perth",
      "Australia/Queensland",
      "Australia/South",
      "Australia/Sydney",
      "Australia/Tasmania",
      "Australia/Victoria",
      "Australia/West",
      "Australia/Yancowinna",
      "Brazil/Acre",
      "Brazil/DeNoronha",
      "Brazil/East",
      "Brazil/West",
      "Canada/Atlantic",
      "Canada/Central",
      "Canada/East-Saskatchewan",
      "Canada/Eastern",
      "Canada/Mountain",
      "Canada/Newfoundland",
      "Canada/Pacific",
      "Canada/Saskatchewan",
      "Canada/Yukon",
      "CET",
      "Chile/Continental",
      "Chile/EasterIsland",
      "CST6CDT",
      "Cuba",
      "EET",
      "Egypt",
      "Eire",
      "EST",
      "EST5EDT",
      "Etc/GMT",
      "Etc/GMT+0",
      "Etc/GMT+1",
      "Etc/GMT+10",
      "Etc/GMT+11",
      "Etc/GMT+12",
      "Etc/GMT+2",
      "Etc/GMT+3",
      "Etc/GMT+4",
      "Etc/GMT+5",
      "Etc/GMT+6",
      "Etc/GMT+7",
      "Etc/GMT+8",
      "Etc/GMT+9",
      "Etc/GMT-0",
      "Etc/GMT-1",
      "Etc/GMT-10",
      "Etc/GMT-11",
      "Etc/GMT-12",
      "Etc/GMT-13",
      "Etc/GMT-14",
      "Etc/GMT-2",
      "Etc/GMT-3",
      "Etc/GMT-4",
      "Etc/GMT-5",
      "Etc/GMT-6",
      "Etc/GMT-7",
      "Etc/GMT-8",
      "Etc/GMT-9",
      "Etc/GMT0",
      "Etc/Greenwich",
      "Etc/UCT",
      "Etc/Universal",
      "Etc/UTC",
      "Etc/Zulu",
      "Europe/Amsterdam",
      "Europe/Andorra",
      "Europe/Athens",
      "Europe/Belfast",
      "Europe/Belgrade",
      "Europe/Berlin",
      "Europe/Bratislava",
      "Europe/Brussels",
      "Europe/Bucharest",
      "Europe/Budapest",
      "Europe/Chisinau",
      "Europe/Copenhagen",
      "Europe/Dublin",
      "Europe/Gibraltar",
      "Europe/Helsinki",
      "Europe/Istanbul",
      "Europe/Kaliningrad",
      "Europe/Kiev",
      "Europe/Lisbon",
      "Europe/Ljubljana",
      "Europe/London",
      "Europe/Luxembourg",
      "Europe/Madrid",
      "Europe/Malta",
      "Europe/Minsk",
      "Europe/Monaco",
      "Europe/Moscow",
      "Europe/Nicosia",
      "Europe/Oslo",
      "Europe/Paris",
      "Europe/Prague",
      "Europe/Riga",
      "Europe/Rome",
      "Europe/Samara",
      "Europe/San_Marino",
      "Europe/Sarajevo",
      "Europe/Simferopol",
      "Europe/Skopje",
      "Europe/Sofia",
      "Europe/Stockholm",
      "Europe/Tallinn",
      "Europe/Tirane",
      "Europe/Tiraspol",
      "Europe/Uzhgorod",
      "Europe/Vaduz",
      "Europe/Vatican",
      "Europe/Vienna",
      "Europe/Vilnius",
      "Europe/Warsaw",
      "Europe/Zagreb",
      "Europe/Zaporozhye",
      "Europe/Zurich",
      "Factory",
      "GB",
      "GB-Eire",
      "GMT",
      "GMT+0",
      "GMT-0",
      "GMT0",
      "Greenwich",
      "Hongkong",
      "HST",
      "Iceland",
      "Indian/Antananarivo",
      "Indian/Chagos",
      "Indian/Christmas",
      "Indian/Cocos",
      "Indian/Comoro",
      "Indian/Kerguelen",
      "Indian/Mahe",
      "Indian/Maldives",
      "Indian/Mauritius",
      "Indian/Mayotte",
      "Indian/Reunion",
      "Iran",
      "Israel",
      "Jamaica",
      "Japan",
      "Kwajalein",
      "Libya",
      "MET",
      "Mexico/BajaNorte",
      "Mexico/BajaSur",
      "Mexico/General",
      "Mideast/Riyadh87",
      "Mideast/Riyadh88",
      "Mideast/Riyadh89",
      "MST",
      "MST7MDT",
      "Navajo",
      "NZ",
      "NZ-CHAT",
      "Pacific/Apia",
      "Pacific/Auckland",
      "Pacific/Chatham",
      "Pacific/Easter",
      "Pacific/Efate",
      "Pacific/Enderbury",
      "Pacific/Fakaofo",
      "Pacific/Fiji",
      "Pacific/Funafuti",
      "Pacific/Galapagos",
      "Pacific/Gambier",
      "Pacific/Guadalcanal",
      "Pacific/Guam",
      "Pacific/Honolulu",
      "Pacific/Johnston",
      "Pacific/Kiritimati",
      "Pacific/Kosrae",
      "Pacific/Kwajalein",
      "Pacific/Majuro",
      "Pacific/Marquesas",
      "Pacific/Midway",
      "Pacific/Nauru",
      "Pacific/Niue",
      "Pacific/Norfolk",
      "Pacific/Noumea",
      "Pacific/Pago_Pago",
      "Pacific/Palau",
      "Pacific/Pitcairn",
      "Pacific/Ponape",
      "Pacific/Port_Moresby",
      "Pacific/Rarotonga",
      "Pacific/Saipan",
      "Pacific/Samoa",
      "Pacific/Tahiti",
      "Pacific/Tarawa",
      "Pacific/Tongatapu",
      "Pacific/Truk",
      "Pacific/Wake",
      "Pacific/Wallis",
      "Pacific/Yap",
      "Poland",
      "Portugal",
      "PRC",
      "PST8PDT",
      "ROC",
      "ROK",
      "Singapore",
      "SystemV/AST4",
      "SystemV/AST4ADT",
      "SystemV/CST6",
      "SystemV/CST6CDT",
      "SystemV/EST5",
      "SystemV/EST5EDT",
      "SystemV/HST10",
      "SystemV/MST7",
      "SystemV/MST7MDT",
      "SystemV/PST8",
      "SystemV/PST8PDT",
      "SystemV/YST9",
      "SystemV/YST9YDT",
      "Turkey",
      "UCT",
      "Universal",
      "US/Alaska",
      "US/Aleutian",
      "US/Arizona",
      "US/Central",
      "US/East-Indiana",
      "US/Eastern",
      "US/Hawaii",
      "US/Indiana-Starke",
      "US/Michigan",
      "US/Mountain",
      "US/Pacific",
      "US/Samoa",
      "UTC",
      "W-SU",
      "WET",
      "Zulu"
        );

  var $dnmode= "";
  var $uid ="";

  function oxchangeAccount (&$config, $dn= NULL)
  {
    plugin::plugin ($config, $dn);
    for ($i=0;$i<count($this->config->data['TABS']['CONNECTIVITY']);$i++){
      if($this->config->data['TABS']['CONNECTIVITY'][$i]['CLASS']=='oxchangeAccount') {
        $this->oxconf=$this->config->data['TABS']['CONNECTIVITY'][$i];
        break;
      }
    }

    /* Setting uid to default */
    if(isset($this->attrs['uid'][0])){
      $this->uid = $this->attrs['uid'][0];
    }

    $this->dnmode= $this->config->current['DNMODE'];
    if(isset($this->attrs[$this->dnmode][0])){
      $this->dnModeValue = $this->attrs[$this->dnmode][0];
    }
  }

  function execute()
  {
    /* Call parent execute */
    //plugin::execute();

    /* Log view */
    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","users/".get_class($this),$this->dn);
    }

    $display="";

    /* Show main page */
    $smarty= get_smarty();

    if (function_exists("pg_connect")){
      $smarty->assign("pg", true);
    }else{
      $smarty->assign("pg", false);
    }

    /* Load attributes */
    foreach($this->attributes as $val){
      $smarty->assign("$val", $this->$val);
    }
    if ($this->is_account){
      $smarty->assign("oxchangeState", "checked");
      $smarty->assign("oxState", "");
    } else {
      $smarty->assign("oxchangeState", "");
      $smarty->assign("oxState", "disabled");
    }

    $tmp = $this->plInfo();
    foreach($tmp['plProvidedAcls'] as $key => $desc){
      $smarty->assign($key."ACL",$this->getacl($key,$this->ReadOnly));
      $smarty->assign($key."_W",$this->acl_is_writeable($key));
    }

    if((!$this->ReadOnly) && (($this->is_account && $this->acl_is_removeable()) || (!$this->is_account && $this->acl_is_createable())) ){
      $smarty->assign('oxchangeAccountACL', "");
    }else{
      $smarty->assign('oxchangeAccountACL', " disabled ");
    }

    $smarty->assign("timezones", $this->timezones);


    if ($this->parent !== NULL){
      $smarty->assign("tabbed", 1);
    }else{
      $smarty->assign("tabbed", 0);
	}

	/* Trying to open a Postgresql Database Server */
	if (function_exists("pg_connect")){
		if(isset($this->oxconf["PGUSER"]) and isset($this->oxconf["PGHOST"]) and isset($this->oxconf["PGDBNAME"]) and isset($this->oxconf["PGPASSWD"])){
			$pgcon =  @pg_connect("host=".$this->oxconf["PGHOST"]." user=".$this->oxconf["PGUSER"]." password=".$this->oxconf["PGPASSWD"]." dbname=".$this->oxconf["PGDBNAME"]);
			if (! $pgcon){
				$smarty->assign("pg", false);
			}
		}else{
			$smarty->assign("pg", false);
		}
	}else{
		$smarty->assign("pg", false);
	}

    $display.= $smarty->fetch (get_template_path('oxchange.tpl', TRUE, dirname(__FILE__)));
    return ($display);
  }

  function remove_from_parent()
  {
    if(!$this->acl_is_removeable())  return;
    $this->attrs[$this->dnmode][0] = $this->dnModeValue;
    $sqldeluser=array(
        "delete from prg_notes where (user_right like '".$this->attrs[$this->dnmode][0]."') and (group_right like 's')",
        "delete from prg_documents_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_documents_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_docufolders_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_docufolders_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_date_rights where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_date_notification where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_dates_members where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_folder_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_knowledge_folder_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_pin_board_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_pin_board_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_folder_read where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_bookmarks_folder_mod where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_tasks_rights where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_tasks_notification where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_tasks_members where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_projects_rights where (user_right like '".$this->attrs[$this->dnmode][0]."')",
        "delete from prg_projects_notification where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete	from prg_projects_members where (member_uid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from oxfolder_permissions where (entity like '".$this->attrs[$this->dnmode][0]."') AND ((role = 256) OR (role = 1024))",
        "delete from oxfolder_standardfolders where owner like '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_read where user_right = '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_mod where user_right = '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_abo where user_right = '".$this->attrs[$this->dnmode][0]."'",
        "delete from prg_forum_seen where username = '".$this->attrs[$this->dnmode][0]."'",
        "delete from sys_holiday where (userid like '".$this->attrs[$this->dnmode][0]."')",
        "delete from usr_holiday where (userid like '".$this->attrs[$this->dnmode][0]."')");

    /* Trying to open a Postgresql Database Server */
    if (function_exists("pg_connect")){
      if(isset($this->oxconf["PGUSER"]) and 
		 isset($this->oxconf["PGHOST"]) and 
	     isset($this->oxconf["PGDBNAME"]) and 
         isset($this->oxconf["PGPASSWD"])){
        $pgcon = @pg_connect("host=".$this->oxconf["PGHOST"]." user=".$this->oxconf["PGUSER"]." password=".$this->oxconf["PGPASSWD"]." dbname=".$this->oxconf["PGDBNAME"]);
        if (! $pgcon){
          msg_dialog::display(_("Error"), sprintf(_("Cannot connect to %s database!"), "Open-Xchange"), ERROR_DIALOG);
          return;
        }
      }else{
        msg_dialog::display(_("Configuration error"), sprintf(_("%s database configuration is missing!"), "Open-Xchange"), ERROR_DIALOG);
        return;
      }
    }else{
      msg_dialog::display(_("Configuration error"), sprintf(_("Missing %s PHP extension!"), "postgresql"), ERROR_DIALOG);
      return;
    }

    plugin::remove_from_parent();
    $ldap= $this->config->get_ldap_link();

    if($ldap->dn_exists("ou=addr,".$this->dn)){
    	$ldap->rmdir_recursive("ou=addr,".$this->dn);
			show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/oxchange addressbook account with dn '%s' failed."),$this->dn));
    }

    $ldap->cd($this->dn);
    @DEBUG (DEBUG_LDAP, __LINE__, __FUNCTION__, __FILE__,
        $this->attributes, "Save");
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    new log("remove","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());

		show_ldap_error($ldap->get_error(), sprintf(_("Removing of user/oxchange addressbook account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->postremove(array("uid" => $this->uid));

    /* Finally remove data from postgresql server */
    foreach ($sqldeluser as $sqls){
       @pg_exec($pgcon,$sqls);
    }
    pg_close();
  }

  /* Save data to object */
  function save_object()
  {
    /* Do we need to flip is_account state? */
    if (isset($_POST['connectivityTab'])){
      if (isset($_POST['oxchange'])){
        if (!$this->is_account && $_POST['oxchange'] == "B") {
          if($this->acl_is_createable()){
            $this->is_account= TRUE;
          }
        }
      } else {
        if($this->acl_is_removeable()){
          $this->is_account= FALSE;
        }
      }
    }

    /* Get objects */
    foreach(array("OXTimeZone","OXTaskDays","OXAppointmentDays") as $name) {
      if($this->acl_is_writeable($name) && isset($_POST[$name])){
        $this->$name = $_POST[$name];
      } 
    }
  
    if (isset($_POST["oxchangeStatus"])){
      $this->oxchangeStatus = "disabled";
    } else {
      $this->oxchangeStatus = "enabled";
    }
  }


  /* Save to LDAP */
  function save()
  {

    $needupdate=TRUE;
    $istemplate=FALSE;

    /*First at all, we must check if this is new or is updated */
    /*Also check is we have a template, if is a template, is a new user */
    if (isset($this->attrs['objectClass'])){
      foreach ($this->attrs['objectClass'] as $object){
        if($object=="OXUserObject") $needupdate=FALSE;
	if($object=="gosaUserTemplate") $istemplate=TRUE;
      }
    }

    $uidarray=array();
    preg_match("/^(\w+(?=\=))=((\w|\s|\.)+(?=\,)),.*/",$this->dn,$uidarray);
    $uid=$uidarray[2];
    if (trim($uid) == "") {
      msg_dialog::display(_("Error"), _("Open-Xchange account name is invalid!"), ERROR_DIALOG);
      return;
    }
	
    if ($istemplate) $needupdate=TRUE;

    if($needupdate){
      /* Trying to open a Postgresql Database Server */
      if (function_exists("pg_connect")){
        if(isset($this->oxconf["PGUSER"]) and isset($this->oxconf["PGHOST"]) and isset($this->oxconf["PGDBNAME"]) and isset($this->oxconf["PGPASSWD"])){
          $pgcon =  @pg_connect("host=".$this->oxconf["PGHOST"]." user=".$this->oxconf["PGUSER"]." password=".$this->oxconf["PGPASSWD"]." dbname=".$this->oxconf["PGDBNAME"]);
          if (! $pgcon){
            msg::display(_("Error"), sprintf(_("Cannot connect to %s database!"), "Open-Xchange"), ERROR_DIALOG);
            return;
          }
        }else{
          msg_dialog::display(_("Configuration error"), sprintf(_("%s database configuration is missing!"), "Open-Xchange"), ERROR_DIALOG);
          return;
        }
      }else{
        msg_dialog::display(_("Configuration error"), sprintf(_("Missing %s PHP extension!"), "postgresql"), ERROR_DIALOG);
        return;
      }
    }

    plugin::save();

    /* Write back to ldap */
    $ldap= $this->config->get_ldap_link();
    $ldap->cd($this->dn);
    $this->cleanup();
    $ldap->modify ($this->attrs); 

    if($this->initially_was_account){
      new log("modify","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","users/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }

		show_ldap_error($ldap->get_error(), sprintf(_("Saving of user/oxchange account with dn '%s' failed."),$this->dn));

    /* Optionally execute a command after we're done */
    $this->postcreate(array("uid" => $this->uid));

    if($needupdate){
      $ldap->create_missing_trees("ou=addr,".$this->dn);
			show_ldap_error($ldap->get_error(), sprintf(_("Creating of user/oxchange account with dn '%s' failed."),$this->dn));
      /* Finally save data to postgresql server */
      pg_set_client_encoding ("UNICODE");
      $nv = "SELECT nextval ('serial_id')";
      $ot = "insert into oxfolder_tree (fuid, parent, fname, module, type, owner, creator, creating_date, created_from, changing_date, changed_from) VALUES (%d, 1, '%s', '%s', 'private', '%s', '%s', 'now', 'System', 'now', 'System')";
      $op = "INSERT INTO oxfolder_permissions (puid, pid, role, entity, sealed, fp, orp, owp, odp) VALUES (%d, %d, 32768,  '%s', 0, 128, 128, 128, 128)";
      $os = "insert into oxfolder_standardfolders (owner,module_calendar,module_contact,module_task) VALUES ('%s',%d,%d,%d)";
      $ugr= "INSERT INTO usr_general_rights SELECT creating_date, created_from, changing_date, changed_from,text('%s'),  addr_u, addr_r, addr_d, cont_u, cont_r, cont_d, data_u, data_r, data_d, serie_u, serie_r, serie_d, task_u, task_r, task_d,  refer, proj_u, proj_r, proj_d, dfolder_u, dfolder_r, dfolder_d, doc_u, doc_r, doc_d, knowl_u, knowl_r, knowl_d, bfolder_u,  bfolder_r, bfolder_d, bookm_u, bookm_r, bookm_d, pin_u, pin_r, pin_d, forum_n, fentrie_n, setup, pin_public, internal,  int_groups, kfolder_u, kfolder_r, kfolder_d, webmail FROM sys_gen_rights_template WHERE login LIKE 'default_template'";

      $error = FALSE;
      $res=@pg_exec($pgcon,$nv); 

      if(!$res){
        $error = TRUE;
      }else{
        $calendarid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($ot,$calendarid[0],'My Appointments','calendar',$uid,$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 

      if(!$res){
        $error = TRUE;
      }else{
        $nid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($op,$nid[0],$calendarid[0],$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $contactsid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($ot,$contactsid[0],'My Contacts','contact',$uid,$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $nid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($op,$nid[0],$contactsid[0],$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $tasksid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($ot,$tasksid[0],'My Tasks','task',$uid,$uid);
        @pg_exec($pgcon,$q);
      }

      $res=@pg_exec($pgcon,$nv); 
      if(!$res){
        $error = TRUE;
      }else{
        $nid=pg_fetch_row($res); 
        pg_freeresult($res);

        $q=sprintf($op,$nid[0],$tasksid[0],$uid);
        @pg_exec($pgcon,$q);
      
        $q=sprintf($os,$uid,$calendarid[0],$contactsid[0],$tasksid[0]);
        @pg_exec($pgcon,$q);
      
        $q=sprintf($ugr,$uid);
        @pg_exec($pgcon,$q);
      }
    
      @pg_close($pgcon);

      if($error){
        msg_dialog::display(_("Error"), _("Cannot save Open-Xchange account!"), ERROR_DIALOG);
      }
    }
  }


  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"     => _("Open-Xchange"),
          "plDescription"   => _("Open-Xchange account settings")."&nbsp;:&nbsp;<u>"._("Connectivity addon")."</u>",
          "plSelfModify"    => TRUE,
          "plDepends"       => array("user"),
          "plPriority"      => 27,                                 // Position in tabs
          "plSection"     => array("personal" => _("My account")),
          "plCategory"    => array("users"),
          "plOptions"       => array(),

          "plProvidedAcls"  => array(
            "OXAppointmentDays" => _("OXAppointmentDays"),
            "OXTaskDays"   	    => _("OXTaskDays"),
            "OXTimeZone"   	    => _("OXTimeZone"))
          ));
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
