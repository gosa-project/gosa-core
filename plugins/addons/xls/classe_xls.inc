<?php

class xls extends plugin
{
  /* Definitions */
  var $plHeadline= "xls";
  var $plDescription= "This does something";

   function xls ($config, $dn= NULL)
  {
	/* Include config object */
	$this->config= $config;

	print $dn;

	#FIXME: ACL is set to default for testing
	$this->acl= "#all#";

        /* Get global filter config */
        if (!is_global("phonefilter")){
		$ui= get_userinfo();
                $base= get_base_from_people($ui->dn);
                $phonefilter= array("search_base" => $base,
				  "organizational" => "checked",
				  "global" => "checked",
                                  "search_for" => "*",
				  "object_type" => "*");
                register_global("phonefilter", $phonefilter);
        }
  }
}
?>