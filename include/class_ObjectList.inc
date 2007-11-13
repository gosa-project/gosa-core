<?php

/*! \brief   Exception implementation for ObjectList
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    This class handles the exceptions occuring in ObjectList.
 */
class ObjectListException extends Exception {
	public function __construct($message, $code = 0) {
		parent::__construct($message, $code);
	}
}
 

/*! \brief   Implementation for keeping a list of objects
    \author  Cajus Pollmeier <pollmeier@gonicus.de>
    \version 1.00
    \date    2007/11/02

    The class ObjectList handles a list of objects found in the database
    based on an optional filter modules. This objects can be iterated
    directly.

    \sa ObjectListIterator
 */
class ObjectList implements IteratorAggregate {

  /*!
    \brief Container for objects

    This variable stores the list of objects.
   */
	private $objects;

  /*!
    \brief Config

    Config array that keeps the classes we do lists for, filter information, etc.
   */
  private $config;


  /*! \brief ObjectList constructor

    The ObjectList is initialized by a list of classes we're interested
    in. Rest is done by instances of Filter registered in the Registry.

    \param config Config section that is used to configure this ObjectList
    \sa Registry
    \sa Filter
   */
	public function __construct(&$config){

    /* Save current config */
    $this->config= &$config;

    /* Load and instanciate classes, extract filter, icons, view hooks, etc. */
    $this->load();

  }


	/*! \brief Function to initialy load object list

	  Internally loads the relevant list of objects depending on eventually
	  defined filter modules. Handles sorting, too.
	 */
	private function load(){
  
    $icon = 
'<img src="images/editcut.png" alt="">&nbsp;'.
'<img src="images/editcopy.png" alt="">&nbsp;'.
'<img src="images/edit.png" alt="">&nbsp;'.
'<img src="images/list_password.png" alt="">&nbsp;'.
'<img src="images/restore.png" alt="">&nbsp;'.
'<img src="images/snapshot.png" alt="">&nbsp;'.
'<img src="images/edittrash.png" alt="">';

    # Crap filling
    $tmp= array(

 array("dn" => "cn=1aherbertskiste,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "1aherbertskiste",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=72:9c:13:e9:72:2b,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "72:9c:13:e9:72:2b",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=8f:9c:d0:d5:42:c0,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "8f:9c:d0:d5:42:c0",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=a.b_c,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "a.b_c",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=a5:35:77:86:ff:ca,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "a5:35:77:86:ff:ca",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=cl1--141,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "cl1--141",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=d9:57:9d:08:2c:65,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "d9:57:9d:08:2c:65",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=dyn-111,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "dyn-111",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=e3:b4:43:51:f6:13,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "e3:b4:43:51:f6:13",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=f2:d7:ef:92:e6:43,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "f2:d7:ef:92:e6:43",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=f9:fa:4e:f8:1a:40,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "f9:fa:4e:f8:1a:40",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=hu_ber-test.gonicus.de,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "hu_ber-test.gonicus.de",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=loghost-01,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "loghost-01",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=ntp.jp.,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "ntp.jp.",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=phone-1.gonicus.de.,ou=phones,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "phone-1.gonicus.de.",
              "_icon" => "select_phone.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=printer.222,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "printer.222",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=printer,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "printer",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=shares.jp.,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "shares.jp. [Share server]",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=terminal.jp.,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "terminal.jp.",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=test.test.etst,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "test.test.etst",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=tester,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "tester",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=vserver-04.intranet.gonicus.de,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "vserver-04.",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=workstation,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "workstation",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),
 array("dn" => "cn=1aherbertskiste,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "1aherbertskiste",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=72:9c:13:e9:72:2b,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "72:9c:13:e9:72:2b",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=8f:9c:d0:d5:42:c0,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "8f:9c:d0:d5:42:c0",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=a.b_c,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "a.b_c",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=a5:35:77:86:ff:ca,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "a5:35:77:86:ff:ca",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=cl1--141,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "cl1--141",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=d9:57:9d:08:2c:65,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "d9:57:9d:08:2c:65",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=dyn-111,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "dyn-111",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=e3:b4:43:51:f6:13,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "e3:b4:43:51:f6:13",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=f2:d7:ef:92:e6:43,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "f2:d7:ef:92:e6:43",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=f9:fa:4e:f8:1a:40,ou=incoming,o=Landeshauptstadt München,c=de",
              "cn" => "f9:fa:4e:f8:1a:40",
              "_icon" => "select_newsystem.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=hu_ber-test.gonicus.de,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "hu_ber-test.gonicus.de",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=loghost-01,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "loghost-01",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=ntp.jp.,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "ntp.jp.",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=phone-1.gonicus.de.,ou=phones,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "phone-1.gonicus.de.",
              "_icon" => "select_phone.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=printer.222,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "printer.222",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=printer,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "printer",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=shares.jp.,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "shares.jp. [Share server]",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=terminal.jp.,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "terminal.jp.",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=test.test.etst,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "test.test.etst",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=tester,ou=printers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "tester",
              "_icon" => "select_printer.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=vserver-04.intranet.gonicus.de,ou=servers,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "vsernicus.de",
              "_icon" => "select_server.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation')),

 array("dn" => "cn=workstation,ou=workstations,ou=systems,o=Landeshauptstadt München,c=de",
              "cn" => "workstation",
              "_icon" => "select_workstation.png",
              "_actions" => $icon,
              "objectClass" => array('gotoWorkstation'))

        );

    $tmp2 = array();
    foreach($tmp as $key => $entry){
      $entry['id']= $key;
      $tmp2[] = $entry;
    }

    $this->objects = $tmp2;
  }


	/*! \brief Function to reload object list in case of external changes

	  Triggers a reload the relevant list of objects depending on eventually
	  defined filter modules. Handles sorting, too.
	 */
  public function reload(){
  }


	/*! \brief Function to return the iterator object for this class.

      This function is used internally by PHP to produce an iterator.

      \return Iterator object

      \sa ObjectListIterator
	 */
  public function getIterator() {
    return new ObjectListIterator($this->objects);
  }

}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>
