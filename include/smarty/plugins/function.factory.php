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
            $str .= "<input {$nameReady} {$idReady} {$valueReady} {$maxlengthReady}
            {$titleReady} {$onfocusReady} type='password'
            onkeypress=\"
                if (capslock(event)){
                    $('password').style.backgroundImage = 'url(images/caps.png)'
                } else {
                    $('password').style.backgroundImage= ''
                }\">";
    }
    return($str);
}
  
?>
