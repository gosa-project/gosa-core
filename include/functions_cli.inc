<?php

function print_usage()
{
	echo "Usage: gosa command [options] <attributes>\n";
	echo "Commands:\n";
	echo "\tcreate\t\tCreates an object under 'entry dn', the content\n";
	echo "\t\t\tand type is set by the -plugin option\n";
	echo "\tdelete\t\tRemoves the either the entry given by 'entry dn',\n";
	echo "\t\t\tor removes a property set by the -plugin option\n";
	echo "\tlist\t\tLists either all objects, object types given by\n";
	echo "\t\t\tthe -plugin option or a single object given by\n";
	echo "\t\t\tthe 'entry dn'. Use -list to specify the list type\n";
	echo "\tmodify\t\tModifies the entry given by 'entry dn' using\n";
	echo "\t\t\tthe object type given by the -plugin option\n";
	echo "Options:\n";
	echo "\t--location=name\tChoose the LDAP profile to work on\n";
	echo "\t--entry-dn=<dn>\tDN to perform actions on\n";
	echo "\t--entry-id=<id>\tID to perform actions on. This option is more fuzzy\n";
	echo "\t\t\tthan the -entry-dn one. It depends on the selected plugin\n";
	echo "\t\t\tand can be i.e. an uid in case of the user plugin or a cn\n";
	echo "\t\t\tin case of a group or department\n";
	echo "\t--list=<type>\tSpecifys the detail of the list command. Possible\n";
	echo "\t\t\tvalues are 'summary' and 'ldif'. The list command defaults\n";
	echo "\t\t\tto 'summary'\n";
	echo "\t--modifyer=<id>\tSet the name of the person who's performing changes\n";
	echo "\t--plugin=<name>\tSelect plugin to work with, use 'list'\n";
	echo "\t\t\tto get a list of available plugins. Every plguin\n";
	echo "\t\t\trequires one or more attributes to be set. You can\n";
	echo "\t\t\tget the list of attributes by using '<name>-help'\n";
}


function get_plugin_list($config)
{
        $plugins= array();
        foreach($config->data['MENU'] as $plugin){
                foreach ($plugin as $value){
                        if (isset($value['CLASS'])){
				if (class_exists($value['CLASS'])){
					$obj= new $value['CLASS']($config, 'new');
					if (isset($obj->cli_summary)){
		                                $plugins[$value['CLASS']]= $obj->cli_summary;
					} else {
		                                $plugins[$value['CLASS']]= "- no description -";
					}
				}
                        }
                }
        }
        foreach($config->data['TABS'] as $plugin){
                foreach ($plugin as $value){
                        if (isset($value['CLASS'])){
				if (class_exists($value['CLASS'])){
					$obj= new $value['CLASS']($config, 'new');
					if (isset($obj->cli_summary)){
		                                $plugins[$value['CLASS']]= $obj->cli_summary;
					} else {
		                                $plugins[$value['CLASS']]= "- no description -";
					}
				}
                        }
                }
        }
        #ksort ($plugins);

	return ($plugins);
}


function show_plugin_list($config)
{
        printf ("\t%-25s%s\n", "Plugin", "Description");
	$plugins= get_plugin_list($config);
        foreach ($plugins as $plugin => $description){
                printf ("\t %-25s%s\n", $plugin, $description);
        }
}


function show_plugin_help($plugin)
{
	global $config;

	$plugins= get_plugin_list($config);
	if (!class_exists($plugin)){
		echo "Plugin '$plugin' does not exist\n";
	} else {
		$obj= new $plugin($config, 'new');
		if (isset($obj->cli_summary)){
			echo $obj->cli_summary."\n\n";
		}
		if (isset($obj->cli_description)){
			echo $obj->cli_description."\n\n";
		}
		if (isset($obj->cli_parameters)){
			print_r ($obj->cli_parameters);
		} else {
			echo "There's no parameter description for this plugin, sorry.\n";
		}
	}
	
}

?>
