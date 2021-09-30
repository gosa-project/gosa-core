<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003  Cajus Pollmeier
 * Copyright (C) 2005  Guillaume Delecourt
 * Copyright (C) 2005  Vincent Seynhaeve
 * Copyright (C) 2005  Benoit Mortier
 *
 * ID: $$Id: getxls.php 19301 2010-07-30 16:12:29Z cajus $$
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

require_once "../../../include/utils/excel/class.writeexcel_workbook.inc.php";
require_once "../../../include/utils/excel/class.writeexcel_worksheet.inc.php";

function dump_ldap ($mode= 0)
{
  global $config;
  $ldap= $config->get_ldap_link();
  $display = "";


  if($mode == 2){	// Single Entry Export !

    /* Get required attributes */
    $d =  base64_decode($_GET['d']);
    $n =  base64_decode($_GET['n']);

    /* Create dn to search entries in */
    $dn=$d.$n;

    /* Create some strings */
    $date=date('dS \of F Y ');
    $fname = tempnam("/tmp", "demo.xls");

    /* Create xls workbench */
    $workbook= new writeexcel_workbook($fname);

    /* Create some styles to generate xls */
    $title_title= $workbook->addformat(array(
          bold    => 1,
          color   => 'green',
          size    => 11,
          underline => 2,
          font    => 'Helvetica'
          ));

    $title_bold= $workbook->addformat(array(
          bold    => 1,
          color   => 'black',
          size    => 10,
          font    => 'Helvetica'
          ));

    # Create a format for the phone numbers
    $f_phone = $workbook->addformat();
    $f_phone->set_align('left');
    $f_phone->set_num_format('\0#');


    /* If the switch reaches default (it should not), 
        this will be set to false, so nothig will be created ... */
    $save = true;

    /* Check which type of data was requested */
    switch ($d){


      /* PEOPLE 
          Get all peoples from this $dn 
          and put them into the xls work sheet */
      case get_people_ou() : 

        $user    =  $ldap->gen_xls($dn,"(objectClass=*)",array("uid","dateOfBirth","gender","givenName","preferredLanguage"));
        $intitul =  array(_("Birthday").":", _("Sex").":", _("Surname")."/"._("Given name").":",_("Language").":");

        // name of the xls file
        $name_section = _("Users");
        $worksheet    = $workbook->addworksheet(_("Users"));
        $worksheet->set_column('A:B', 51);

        $user_nbr =  count($user);
        $worksheet->write('A1',sprintf(_("User list of %s on %s"),$n,$date),$title_title);
        $r=3;
        for($i=1;$i<$user_nbr;$i++)
        {
          if($i>1)
            $worksheet->write('A'.$r++,"");
          $worksheet->write('A'.$r++,_("User ID").": ".$user[$i][0],$title_bold);

          for($j=1;$j<5;$j++)
          {
            $r++;
            $worksheet->write('A'.$r,$intitul[$j-1]);
            $user[$i][$j]=utf8_decode ($user[$i][$j]);
            $worksheet->write('B'.$r,$user[$i][$j]);
          }
          $worksheet->write('A'.$r++,"");
        }
      break;


      /* GROUPS 
          Get all groups from th $dn 
          and put them into the xls work sheet */
      case get_groups_ou(): 

        /* Get group data */
        $groups    = $ldap->gen_xls($dn,"(objectClass=*)",array("cn","memberUid"),TRUE,1);
        $intitul   = array(_("Members").":");

        //name of the xls file
        $name_section=_("Groups");

        $worksheet = $workbook->addworksheet(_("Groups"));
        $worksheet->set_column('A:B', 51);

        //count number of groups
        $groups_nbr=count($groups);
        $worksheet->write('A1', sprintf(_("Groups of %s on %s"), $n, $date),$title_title);
        $r=3;
        for($i=1;$i<$groups_nbr;$i++)
        {
          $worksheet->write('A'.$r++,_("User ID").": ".$groups[$i][0][0],$title_bold);
          for($j=1;$j<=2;$j++)
          {
            $r++;
            $worksheet->write('A'.$r,$intitul[$j-1]);
            for($k=0;$k<= $groups[$i][$j]['count'];$k++)
            {
              $worksheet->write('B'.$r,$groups[$i][$j][$k]);
              $r++;
            }
          }
        }
     break;


     /* SYSTEMS 
        Get all systems from th $dn
        and put them into the xls work sheet */
     case get_ou("systemManagement", "systemRDN"): 

       $name_section=_("Servers");
       $computers= $ldap->gen_xls($dn,"(&(objectClass=*)(cn=*))",array("cn","description","uid"));

       $intitul=array(_("Description").":",_("User ID").":");
       $worksheet = $workbook->addworksheet(_("Computers"));
       $worksheet->set_column('A:B', 32);

       //count number of computers
       $computers_nbr=count($computers);
       $r=1;
       for($i=1;$i<$computers_nbr;$i++)
       {
         if($i>1)
           $worksheet->write('A'.$r++,"");
         $worksheet->write('A'.$r++,_("Common name").": ".$computers[$i][0],$title_bold);
         for($j=1;$j<3;$j++)
         {
           $r++;
           $worksheet->write('A'.$r,$intitul[$j-1]);
           $computers[$i][$j]=utf8_decode ($computers[$i][$j]);
           $worksheet->write('B'.$r,$computers[$i][$j]);
         }
         $worksheet->write('A'.$r++,"");
       }
    break;

     /* SYSTEMS 
       Get all systems from th $dn
       and put them into the xls work sheet */
     case get_ou("servgeneric", "serverRDN"): $servers= $ldap->gen_xls($dn,"(objectClass=*)",array("cn"));
       $intitul=array(_("Server name").":");

       //name of the xls file
       $name_section=_("Servers");

       $worksheet = $workbook->addworksheet(_("Servers"));
       $worksheet->set_column('A:B', 51);

       //count number of servers
       $servers_nbr=count($servers);
       $worksheet->write('A1',sprintf(_("Servers of %s on %s"), $n, $date),$title_title);
       $r=3;
       $worksheet->write('A'.$r++,_("Servers").": ",$title_bold);
       for($i=1;$i<$servers_nbr;$i++)
       {
         for($j=0;$j<1;$j++)
         {
           $r++;
           $worksheet->write('A'.$r,$intitul[$j]);
           $servers[$i][$j]=utf8_decode ($servers[$i][$j]);
           $worksheet->write('B'.$r,$servers[$i][$j]);
         }
       }
     break;

     case "dc=addressbook,": //data about addressbook

       /* ADDRESSBOOK 
        Get all addressbook entries from  $dn
         and put them into the xls work sheet */

       $address= $ldap->gen_xls($dn,"(objectClass=*)",
                    array("cn","displayName","facsimileTelephoneNumber","givenName",
                          "homePhone","homePostalAddress","initials","l","mail","mobile",
                          "o","ou","pager","telephoneNumber","postalAddress",
                          "postalCode","sn","st","title"));

       $intitul=  array(_("Common name").":",_("Display name").":",_("Fax").":",
                        _("Name")."/"._("Given name").":",_("Home phone").":",
                        _("Home postal address").":",_("Initials").":",_("Location").":",
                        _("Mail address").":",_("Mobile phone").":",_("City").":",
                        _("Postal address").":",_("Pager").":",_("Phone number").":",
                        _("Address").":",_("Postal code").":",_("Surname").":",
                        _("State").":",_("Function").":");

       //name of the xls file
       $name_section=_("Address book");

       $worksheet = $workbook->addworksheet(_("Servers"));
       $worksheet->set_column('A:B', 51);

       //count number of entries
       $address_nbr=count($address);
       $worksheet->write('A1',sprintf(_("Address book of %s on %s"),$n, $date),$title_title);
       $r=3;
       for($i=1;$i<$address_nbr;$i++)
       {
         if($i>1)
           $worksheet->write('A'.$r++,"");
         $worksheet->write('A'.$r++,_("Common Name").": ".$address[$i][0],$title_bold);
         for($j=1;$j<19;$j++)
         {
           $r++;
           $worksheet->write('A'.$r,$intitul[$j]);
           $address[$i][$j]=utf8_decode ($address[$i][$j]);
           $worksheet->write('B'.$r,$address[$i][$j],$f_phone);
         }
         $worksheet->write('A'.$r++,"");
       }

     break;
     default:

        $save = false; 
        echo "Specified parameter '".$d."' was not found in switch-case.";
   }

   if($save){
     $workbook->close();
   }

   // We'll be outputting a xls
   header('Content-type: application/x-msexcel');

   // It will be called demo.xls
   header('Content-Disposition: attachment; filename=xls_export_'.$name_section.".xls");

   // The source is in original.xls
   readfile($fname);
   unlink ($fname);
  }
  elseif($mode == 3){ // Full Export !
    $dn =  base64_decode($_GET['dn']);

    //data about users
    $user= $ldap->gen_xls( get_people_ou().$dn,"(objectClass=*)",array("uid","dateOfBirth","gender","givenName","preferredLanguage"));
    $user_intitul=array(_("Day of birth").":",_("Sex").":",_("Surname")."/"._("Given name").":",_("Language").":");
    //data about groups
    $groups= $ldap->gen_xls(get_groups_ou().$dn,"(objectClass=*)",array("cn","memberUid"),TRUE,1);
    $groups_intitul=array(_("Members").":");
    //data about computers
    $computers= $ldap->gen_xls("ou=computers,".$dn,"(objectClass=*)",array("cn","description","uid"));
    $computers_intitul=array(_("Description").":",_("UID").":");
    //data about servers
    $servers= $ldap->gen_xls(get_ou("servgeneric", "serverRDN").$dn,"(objectClass=*)",array("cn"));
    $servers_intitul=array(_("Name").":");
    //data about addressbook
    $address= $ldap->gen_xls("dc=addressbook,".$dn,"(objectClass=*)",
          array("cn","displayName","facsimileTelephoneNumber","givenName","homePhone","homePostalAddress",
                "initials","l","mail","mobile","o","ou","pager","telephoneNumber","postalAddress",
                "postalCode","sn","st","title"));
    $address_intitul=
          array("cn",_("Display name").":",_("Fax").":",_("Surname")."/"._("Given name").":",
                _("Phone number").":",_("Postal address").":",_("Initials").":",_("City").":",
                _("Email address").":",_("Mobile").":",_("Organization").":",_("Organizational unit").":",
                _("Pager").":",_("Phone number").":",_("Postal address").":",_("Postal Code").":",
                _("Surname").":",_("State").":",_("Title").":");

    //name of the xls file
    $name_section=_("Full");
    $date=date('dS \of F Y ');
    $fname = tempnam("/tmp", "demo.xls");
    $workbook =  new writeexcel_workbook($fname);
    $worksheet = $workbook->addworksheet(_("Users"));
    $worksheet2 = $workbook->addworksheet(_("Groups"));
    $worksheet3 = $workbook->addworksheet(_("Servers"));
    $worksheet4 =$workbook->addworksheet(_("Computers"));
    $worksheet5 = $workbook->addworksheet(_("Address book"));

    $worksheet->set_column('A:B', 51);
    $worksheet2->set_column('A:B', 51);
    $worksheet3->set_column('A:B', 51);
    $worksheet4->set_column('A:B', 51);
    $worksheet5->set_column('A:B', 51);

    $title_title= $workbook->addformat(array(
          bold    => 1,
          color   => 'green',
          size    => 11,
          font    => 'Helvetica'
          ));

    $title_bold = $workbook->addformat(array(
          bold    => 1,
          color   => 'black',
          size    => 10,
          font    => 'Helvetica'
          ));

# Create a format for the phone numbers
    $f_phone = $workbook->addformat();
    $f_phone->set_align('left');
    $f_phone->set_num_format('\0#');

    //count number of users
    $user_nbr=count($user);
    $worksheet->write('A1',sprintf(_("User list of %s on %s"), $dn, $date),$title_title);
    $r=3;
    for($i=1;$i<$user_nbr;$i++)
    {
      if($i>1)
        $worksheet->write('A'.$r++,"");
      $worksheet->write('A'.$r++,_("User ID").": ".$user[$i][0],$title_bold);
      for($j=1;$j<5;$j++)
      {
        $r++;
        $worksheet->write('A'.$r,$user_intitul[$j-1]);
        $user[$i][$j]=utf8_decode ($user[$i][$j]);
        $worksheet->write('B'.$r,$user[$i][$j]);
      }
      $worksheet->write('A'.$r++,"");
    }

    //count number of groups
    $groups_nbr=count($groups);
    $worksheet2->write('A1',sprintf(_("Groups of %s on %s"), $dn, $date),$title_title);
    $r=3;
    for($i=1;$i<$groups_nbr;$i++)
    {
      $worksheet2->write('A'.$r++,_("User ID").": ".$groups[$i][0][0],$title_bold);
      for($j=1;$j<=2;$j++)
      {
        $r++;
        $worksheet2->write('A'.$r,$group_intitul[$j-1]);
        for($k=0;$k<= $groups[$i][$j]['count'];$k++)
        {
          $worksheet2->write('B'.$r,$groups[$i][$j][$k]);
          $r++;
        }
      }
    }

    //count number of servers
    $servers_nbr=count($servers);
    $worksheet3->write('A1',sprintf(_("Servers of %s on %s"),$dn,$date),$title_title);
    $r=3;
    $worksheet3->write('A'.$r++,_("Servers").": ",$title_bold);
    for($i=1;$i<$servers_nbr;$i++)
    {
      for($j=0;$j<1;$j++)
      {
        $r++;
        $worksheet3->write('A'.$r,$servers_intitul[$j]);
        $servers[$i][$j]=utf8_decode ($servers[$i][$j]);
        $worksheet3->write('B'.$r,$servers[$i][$j]);
      }
    }

    //count number of computers
    $computers_nbr=count($computers);
    $worksheet4->write('A1',sprintf(_("Computers of %s on %s"),$dn,$date),$title_title);
    $r=3;
    for($i=1;$i<$computers_nbr;$i++)
    {
      if($i>1)
        $worksheet->write('A'.$r++,"");
      $worksheet4->write('A'.$r++,_("Common name").": ".$computers[$i][0],$title_bold);
      for($j=1;$j<3;$j++)
      {
        $r++;
        $worksheet4->write('A'.$r,$computers_intitul[$j-1]);
        $computers[$i][$j]=utf8_decode ($computers[$i][$j]);
        $worksheet4->write('B'.$r,$computers[$i][$j]);
      }
      $worksheet4->write('A'.$r++,"");
    }

    //count number of entries
    $address_nbr=count($address);
    $worksheet5->write('A1',sprintf(_("Address book of %s on %s"),$dn, $date),$title_title);

    $r=3;
    for($i=1;$i<$address_nbr;$i++)
    {
      if($i>1)
        $worksheet5->write('A'.$r++,"");
      $worksheet5->write('A'.$r++,_("Common name").": ".$address[$i][0],$title_bold);
      for($j=1;$j<19;$j++)
      {
        $r++;
        $worksheet5->write('A'.$r,$address_intitul[$j]);
        $address[$i][$j]=utf8_decode ($address[$i][$j]);
        $worksheet5->write('B'.$r,$address[$i][$j],$f_phone);
      }
      $worksheet5->write('A'.$r++,"");
    }
    $workbook->close();


    // We'll be outputting a xls
    header('Content-type: application/x-msexcel');

    // It will be called demo.xls
    header('Content-Disposition: attachment; filename='.$name_section.".xls");

    readfile($fname);

    unlink ($fname);
  }
  elseif($mode == 4){ // IVBB LDIF Export
    $dn =  base64_decode($_GET['dn']);
    echo $display;
  }
}


/* Basic setup, remove eventually registered sessions */
@require_once ("../../../include/php_setup.inc");
@require_once ("functions.inc");
session::start();
session::set('errorsAlreadyPosted',array());

/* Logged in? Simple security check */
if (!session::is_set('ui')){
  new log("security","all/all","",array(),"Error: getxls.php called without session") ;
  header ("Location: index.php");
  exit;
}
$ui     = session::get('ui');
$config = session::get('config');

/* Check ACL's */
$dn ="";
if(isset($_GET['n'])){
  $dn = base64_decode($_GET['n']);
  $acl_dn = base64_decode($_GET['d']).base64_decode($_GET['n']);
}elseif(isset($_GET['dn'])){
  $dn = base64_decode($_GET['dn']);
  $acl_dn = base64_decode($_GET['dn']);
}

$acl = $ui->get_permissions($acl_dn,"ldapmanager/ldif");
if(!preg_match("/r/",$acl)){
	msg_dialog::display(_("Permission error"),_("You have no permission to do LDAP exports!"),FATAL_ERROR_DIALOG);
  exit();
}

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");

header("Content-type: text/plain");

switch ($_GET['ivbb']){
  case 2: dump_ldap (2);
          break;

  case 3: dump_ldap (3);
          break;

  case 4: dump_ldap (4);
          break;

  default:
          echo "Error in ivbb parameter. Request aborted.";
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
