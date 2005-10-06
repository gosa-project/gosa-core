<?php

function convert_package_list($server, $release, $sections)
{
	$result= array();
	$package= "";
	
	foreach ($sections as $section){
		$fh = fopen("/etc/gosa/fai/$server/$release/$section.in", "r");
		$fw = fopen("/etc/gosa/fai/$server/$release/$section", "w");
		
		while (!feof($fh)){
			$buffer= fgets($fh, 256);

			if (preg_match("/^Package:/", $buffer)){
				// Write out result
				if ($package != ""){
					fputs($fw, $package."|".
					           $result[$package]["Version"]."|".
					           $result[$package]["Section"]."|".
					           $result[$package]["Description"]."\n");
				}
				
				$package= trim(preg_replace("/^[^:]+:\s+/", "", $buffer));
				$result[$package]= array();

				continue;
			}

			foreach (array("Version", "Description", "Section") as $var){
				if (preg_match("/^$var:/", $buffer)){
					$result[$package]["$var"]= trim(preg_replace("/^[^:]+:\s+/", "", $buffer));
					break;
				}
			}

		}
		
		fclose($fw);
		fclose($fh);
	}

	return ($result);
}

convert_package_list("vserver-01", "sarge", array("main"));

?>
