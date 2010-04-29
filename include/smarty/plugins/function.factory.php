<?php

function smarty_function_factory($params, &$smarty)
{

    // Capture params
    foreach(array('type','id','name','title','value','maxlength',
                'onfocus','onclick') as $var){
        $$var = (isset($params[$var]))? $params[$var] : "";
        $tmp  = "{$var}Ready";
        $$tmp = (isset($params[$var]))? "{$var}=\"{$params[$var]}\"" : "";
    }

    $str = "";
    switch($type){

        // Generate a password input field, with CapsLock detection.
        case 'password' :

            // Maxlength has a default of 40 characters
            $maxlengthReady = (empty($maxlength))?'maxlength="40"': $maxlengthReady; 
            $str .= "<input {$nameReady} {$idReady} {$valueReady} {$maxlengthReady}
            {$titleReady} {$onfocusReady} type='password'
            onkeypress=\"
                if (capslock(event)){
                    $('{$id}').style.backgroundImage = 'url(images/caps.png)'
                } else {
                    $('{$id}').style.backgroundImage= ''
                }\">";
    }
    return($str);
}
  
?>
