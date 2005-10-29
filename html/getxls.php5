<?php
/*
   This code is part of GOsa (https://gosa.gonicus.de)
   Copyright (C) 2003  Cajus Pollmeier

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

require_once "../include/php_writeexcel/class.writeexcel_workbook.inc.php";
require_once "../include/php_writeexcel/class.writeexcel_worksheet.inc.php";

function dump_ldap ($mode= 0)
{
  global $config;
  $ldap= $config->get_ldap_link();
  error_reporting (E_ALL & ~E_NOTICE);

  $display = "";
  if($mode == 2){	// Single Entry Export !
    $d =  base64_decode($_GET['d']);
    $n =  base64_decode($_GET['n']);
    // 	print_r($_GET);
    $dn=$d.$n;
    // 	echo $dn ;
    $fname = tempnam("/tmp", "demo.xls");
    $workbook= new writeexcel_workbook($fname);

    $title_bold= $workbook->addformat(array(
          bold    => 1,
          color   => 'black',
          size    => 10,
          font    => 'Helvetica'
          ));


    switch ($d){
      case "ou=people," : 
        $user= 				   $ldap->gen_xls($dn,"(objectClass=*)",array("uid","dateOfBirth","gender","givenName","preferredLanguage"));
      $intitul=array("Date de naissance:","Sexe:","Nom/prénom","Langue");

      $worksheet= $workbook->addworksheet('Users');
      $worksheet->set_column('A:B', 32);


      $user_nbr=count($user);
      $r=1;
      for($i=1;$i<$user_nbr;$i++)
      {
        if($i>1)
          $worksheet->write('A'.$r++,"");
        $worksheet->write('A'.$r++,"uid : ".$user[$i][0],$title_bold);
        for($j=1;$j<5;$j++)
        {
          $r++;
          $worksheet->write('A'.$r,$intitul[$j-1]);
          $worksheet->write('B'.$r,$user[$i][$j]);
        }
        $worksheet->write('A'.$r++,"");
      }
      break;

      case "ou=groups,": $groups= $ldap->gen_xls($dn,"(objectClass=*)",array("cn","memberUid"),TRUE,1);
      $intitul=array("Membres:");
      $worksheet =& $workbook->addworksheet('Groups');
      $worksheet->set_column('A:B', 32);

      //count number of groups
      $groups_nbr=count($groups);
      $r=1;
      for($i=1;$i<$groups_nbr;$i++)
      {
        $worksheet->write('A'.$r++,"uid : ".$groups[$i][0][0],$title_bold);
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

      case "ou=computers,": $computers= $ldap->gen_xls($dn,"(objectClass=*)",array("cn","description","uid"));
      $intitul=array("Description:","Uid:");
      $worksheet =& $workbook->addworksheet('Computers');
      $worksheet->set_column('A:B', 32);

      //count number of computers
      $computers_nbr=count($computers);
      $r=1;
      for($i=1;$i<$computers_nbr;$i++)
      {
        if($i>1)
          $worksheet->write('A'.$r++,"");
        $worksheet->write('A'.$r++,"cn : ".$computers[$i][0],$title_bold);
        for($j=1;$j<3;$j++)
        {
          $r++;
          $worksheet->write('A'.$r,$intitul[$j-1]);
          $worksheet->write('B'.$r,$computers[$i][$j]);
        }
        $worksheet->write('A'.$r++,"");
      }
      break;

      case "ou=servers,ou=systems,": $servers= $ldap->gen_xls($dn,"(objectClass=*)",array("cn"));
      $intitul=array("cn:");

      $worksheet =& $workbook->addworksheet('Servers');
      $worksheet->set_column('A:B', 32);

      //count number of servers
      $servers_nbr=count($servers);
      $r=1;
      $worksheet->write('A'.$r++,"Servers : ",$title_bold);
      for($i=1;$i<$servers_nbr;$i++)
      {
        for($j=0;$j<1;$j++)
        {
          $r++;
          $worksheet->write('A'.$r,$intitul[$j]);
          $worksheet->write('B'.$r,$servers[$i][$j]);
        }
      }
      break;

      case "dc=addressbook,": //data about addressbook
        $address= $ldap->gen_xls($dn,"(objectClass=*)",array("cn","displayName","facsimileTelephoneNumber","givenName","homePhone","homePostalAddress","initials","l","mail","mobile","o","ou","pager","telephoneNumber","postalAddress","postalCode","sn","st","title"));

      $intitul=array("cn","DisplayName:","Fax:","Nom/prénom:","Numero de telelphone:","Adresse du domicile:","Initiales:","Ville:","Adresse email:","GSM:","Societe:","Poste:","Pager:","Numero de telelphone:","Adresse:","Code postal:","Sn:","Pays:","Fonction:");

      $worksheet =& $workbook->addworksheet('Servers');
      $worksheet->set_column('A:B', 32);

      //count number of entries
      $address_nbr=count($address);
      $r=1;
      for($i=1;$i<$address_nbr;$i++)
      {
        if($i>1)
          $worksheet->write('A'.$r++,"");
        $worksheet->write('A'.$r++,"cn : ".$address[$i][0],$title_bold);
        for($j=1;$j<19;$j++)
        {
          $r++;
          $worksheet->write('A'.$r,$intitul[$j]);
          $worksheet->write('B'.$r,$address[$i][$j]);
        }
        $worksheet->write('A'.$r++,"");
      }

      break;

      default: echo "error!!";
    }

    $workbook->close();


    // We'll be outputting a xls
    header('Content-type: application/x-msexcel');

    // It will be called demo.xls
    header('Content-Disposition: attachment; filename="demo.xls"');

    // The PDF source is in original.xl
    readfile($fname);

  }
  elseif($mode == 3){ // Full LDIF Export !
    $dn =  base64_decode($_GET['dn']);

    //data about users
    $user= $ldap->gen_xls("ou=people,".$dn,"(objectClass=*)",array("uid","dateOfBirth","gender","givenName","preferredLanguage"));
    $user_intitul=array("Date de naissance:","Sexe:","Nom/prénom","Langue");
    //data about groups
    $groups= $ldap->gen_xls("ou=groups,".$dn,"(objectClass=*)",array("cn","memberUid"),TRUE,1);
    $groups_intitul=array("Membres:");
    //data about computers
    $computers= $ldap->gen_xls("ou=computers,".$dn,"(objectClass=*)",array("cn","description","uid"));
    $computers_intitul=array("Description:","Uid:");
    //data about servers
    $servers= $ldap->gen_xls("ou=servers,ou=systems,".$dn,"(objectClass=*)",array("cn"));
    $servers_intitul=array("cn:");
    //data about addressbook
    $address= $ldap->gen_xls("dc=addressbook,".$dn,"(objectClass=*)",array("cn","displayName","facsimileTelephoneNumber","givenName","homePhone","homePostalAddress","initials","l","mail","mobile","o","ou","pager","telephoneNumber","postalAddress","postalCode","sn","st","title"));
    $address_intitul=array("cn","DisplayName:","Fax:","Nom/prénom:","Numero de telelphone:","Adresse du domicile:","Initiales:","Ville:","Adresse email:","GSM:","Societe:","Poste:","Pager:","Numero de telelphone:","Adresse:","Code postal:","Sn:","Pays:","Fonction:");


    $fname = tempnam("/tmp", "demo.xls");
    $workbook = new writeexcel_workbook($fname);
    $worksheet = $workbook->addworksheet('Users');
    $worksheet2 = $workbook->addworksheet('Groups');
    $worksheet3 = $workbook->addworksheet('Servers');
    $worksheet4 = $workbook->addworksheet('Computers');
    $worksheet5 = $workbook->addworksheet('Adressbook');

    $worksheet->set_column('A:B', 32);
    $worksheet2->set_column('A:B', 32);
    $worksheet3->set_column('A:B', 32);
    $worksheet4->set_column('A:B', 32);
    $worksheet5->set_column('A:B', 32);

    $title_bold =& $workbook->addformat(array(
          bold    => 1,
          color   => 'black',
          size    => 10,
          font    => 'Helvetica'
          ));


    //count number of users
    $user_nbr=count($user);
    $r=1;
    for($i=1;$i<$user_nbr;$i++)
    {
      if($i>1)
        $worksheet->write('A'.$r++,"");
      $worksheet->write('A'.$r++,"uid : ".$user[$i][0],$title_bold);
      for($j=1;$j<5;$j++)
      {
        $r++;
        $worksheet->write('A'.$r,$user_intitul[$j-1]);
        $worksheet->write('B'.$r,$user[$i][$j]);
      }
      $worksheet->write('A'.$r++,"");
    }

    //count number of groups
    $groups_nbr=count($groups);
    $r=1;
    for($i=1;$i<$groups_nbr;$i++)
    {
      $worksheet2->write('A'.$r++,"uid : ".$groups[$i][0][0],$title_bold);
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
    $r=1;
    $worksheet3->write('A'.$r++,"Servers : ",$title_bold);
    for($i=1;$i<$servers_nbr;$i++)
    {
      for($j=0;$j<1;$j++)
      {
        $r++;
        $worksheet3->write('A'.$r,$servers_intitul[$j]);
        $worksheet3->write('B'.$r,$servers[$i][$j]);
      }
    }

    //count number of computers
    $computers_nbr=count($computers);
    $r=1;
    for($i=1;$i<$computers_nbr;$i++)
    {
      if($i>1)
        $worksheet->write('A'.$r++,"");
      $worksheet4->write('A'.$r++,"cn : ".$computers[$i][0],$title_bold);
      for($j=1;$j<3;$j++)
      {
        $r++;
        $worksheet4->write('A'.$r,$computers_intitul[$j-1]);
        $worksheet4->write('B'.$r,$computers[$i][$j]);
      }
      $worksheet4->write('A'.$r++,"");
    }

    //count number of entries
    $address_nbr=count($address);
    $r=1;
    for($i=1;$i<$address_nbr;$i++)
    {
      if($i>1)
        $worksheet5->write('A'.$r++,"");
      $worksheet5->write('A'.$r++,"cn : ".$address[$i][0],$title_bold);
      for($j=1;$j<19;$j++)
      {
        $r++;
        $worksheet5->write('A'.$r,$address_intitul[$j]);
        $worksheet5->write('B'.$r,$address[$i][$j]);
      }
      $worksheet5->write('A'.$r++,"");
    }
    $workbook->close();


    // We'll be outputting a xls
    header('Content-type: application/x-msexcel');

    // It will be called demo.xls
    header('Content-Disposition: attachment; filename="demo.xls"');

    // The PDF source is in original.xl
    readfile($fname);

  }
  elseif($mode == 4){ // IVBB LDIF Export
    $dn =  base64_decode($_GET['dn']);
    /*$display= $ldap->gen_ldif($dn,"(objectClass=ivbbEntry)",array(
      "GouvernmentOrganizationalUnit","houseIdentifier","vocation",
      "ivbbLastDeliveryCollective","gouvernmentOrganizationalPersonLocality",
      "gouvernmentOrganizationalUnitDescription","gouvernmentOrganizationalUnitSubjectArea",
      "functionalTitle","role","certificateSerialNumber","userCertificate","publicVisible",
      "telephoneNumber","seeAlso","description","title","x121Address","registeredAddress",
      "destinationIndicator","preferredDeliveryMethod","telexNumber","teletexTerminalIdentifier",
      "telephoneNumber","internationaliSDNNumber","facsimileTelephoneNumber","street",
      "postOfficeBox","postalCode","postalAddress","physicalDeliveryOfficeName","ou",
      "st","l","audio","businessCategory","carLicense","departmentNumber","displayName",
      "employeeNumber","employeeType","givenName","homePhone","homePostalAddress",
      "initials","jpegPhoto","labeledURI","mail","manager","mobile","o","pager","photo",
      "roomNumber","secretary","userCertificate","x500uniqueIdentifier","preferredLanguage",
      "userSMIMECertificate","userPKCS12"));*/

    echo $display;
  }
}


/* Basic setup, remove eventually registered sessions */
@require_once ("../include/php_setup.inc");
@require_once ("functions.inc");
error_reporting (E_ALL);
session_start ();

/* Logged in? Simple security check */
if (!isset($_SESSION['ui'])){
  gosa_log ("Error: getldif.php called without session");
  header ("Location: ../index.php");
  exit;
}
$ui= $_SESSION["ui"];
$config= $_SESSION['config'];

header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
header("Last-Modified: ".gmdate("D, d M Y H:i:s")." GMT");
header("Cache-Control: no-cache");
header("Pragma: no-cache");
header("Cache-Control: post-check=0, pre-check=0");

header("Content-type: text/plain");

/* Check ACL's */
$acl= get_permissions ($config->current['BASE'], $ui->subtreeACL);
$acl= get_module_permission($acl, "all", $config->current['BASE']);
if (chkacl($acl, "all") != ""){
  header ("Location: ../index.php");
  exit;
}

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
