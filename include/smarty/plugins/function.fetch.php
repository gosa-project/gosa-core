<?php
/**
 * Smarty plugin
 *
 * @package Smarty
 * @subpackage PluginsFunction
 */
<<<<<<< HEAD

/**
 * Smarty {fetch} plugin
 *
 * Type:     function<br>
 * Name:     fetch<br>
=======
/**
 * Smarty {fetch} plugin
 * Type:     function
 * Name:     fetch
>>>>>>> gosa-core_v2.8
 * Purpose:  fetch file, web or ftp data and display results
 *
 * @link http://www.smarty.net/manual/en/language.function.fetch.php {fetch}
 *       (Smarty online manual)
 * @author Monte Ohrt <monte at ohrt dot com>
<<<<<<< HEAD
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
=======
 *
 * @param array                    $params   parameters
 * @param Smarty_Internal_Template $template template object
 *
 * @throws SmartyException
>>>>>>> gosa-core_v2.8
 * @return string|null if the assign parameter is passed, Smarty assigns the result to a template variable
 */
function smarty_function_fetch($params, $template)
{
    if (empty($params['file'])) {
<<<<<<< HEAD
        trigger_error("[plugin] fetch parameter 'file' cannot be empty",E_USER_NOTICE);
        return;
    }
    
=======
        trigger_error('[plugin] fetch parameter \'file\' cannot be empty', E_USER_NOTICE);
        return;
    }
>>>>>>> gosa-core_v2.8
    // strip file protocol
    if (stripos($params['file'], 'file://') === 0) {
        $params['file'] = substr($params['file'], 7);
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    $protocol = strpos($params['file'], '://');
    if ($protocol !== false) {
        $protocol = strtolower(substr($params['file'], 0, $protocol));
    }
<<<<<<< HEAD
    
=======
>>>>>>> gosa-core_v2.8
    if (isset($template->smarty->security_policy)) {
        if ($protocol) {
            // remote resource (or php stream, …)
            if(!$template->smarty->security_policy->isTrustedUri($params['file'])) {
                return;
            }
        } else {
            // local file
            if(!$template->smarty->security_policy->isTrustedResourceDir($params['file'])) {
                return;
            }
        }
    }
<<<<<<< HEAD

    $content = '';
    if ($protocol == 'http') {
=======
    $content = '';
    if ($protocol === 'http') {
>>>>>>> gosa-core_v2.8
        // http fetch
        if($uri_parts = parse_url($params['file'])) {
            // set defaults
            $host = $server_name = $uri_parts['host'];
            $timeout = 30;
<<<<<<< HEAD
            $accept = "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*";
            $agent = "Smarty Template Engine ". Smarty::SMARTY_VERSION;
            $referer = "";
=======
            $accept = 'image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*';
            $agent = 'Smarty Template Engine ' . Smarty::SMARTY_VERSION;
            $referer = '';
>>>>>>> gosa-core_v2.8
            $uri = !empty($uri_parts['path']) ? $uri_parts['path'] : '/';
            $uri .= !empty($uri_parts['query']) ? '?' . $uri_parts['query'] : '';
            $_is_proxy = false;
            if(empty($uri_parts['port'])) {
                $port = 80;
            } else {
                $port = $uri_parts['port'];
            }
            if(!empty($uri_parts['user'])) {
                $user = $uri_parts['user'];
            }
            if(!empty($uri_parts['pass'])) {
                $pass = $uri_parts['pass'];
            }
            // loop through parameters, setup headers
            foreach($params as $param_key => $param_value) {
                switch($param_key) {
<<<<<<< HEAD
                    case "file":
                    case "assign":
                    case "assign_headers":
                        break;
                    case "user":
=======
                    case 'file':
                    case 'assign':
                    case 'assign_headers':
                        break;
                    case 'user':
>>>>>>> gosa-core_v2.8
                        if(!empty($param_value)) {
                            $user = $param_value;
                        }
                        break;
<<<<<<< HEAD
                    case "pass":
=======
                    case 'pass':
>>>>>>> gosa-core_v2.8
                        if(!empty($param_value)) {
                            $pass = $param_value;
                        }
                        break;
<<<<<<< HEAD
                    case "accept":
=======
                    case 'accept':
>>>>>>> gosa-core_v2.8
                        if(!empty($param_value)) {
                            $accept = $param_value;
                        }
                        break;
<<<<<<< HEAD
                    case "header":
                        if(!empty($param_value)) {
                            if(!preg_match('![\w\d-]+: .+!',$param_value)) {
                                trigger_error("[plugin] invalid header format '".$param_value."'",E_USER_NOTICE);
=======
                    case 'header':
                        if (!empty($param_value)) {
                            if (!preg_match('![\w\d-]+: .+!', $param_value)) {
                                trigger_error("[plugin] invalid header format '{$param_value}'", E_USER_NOTICE);
>>>>>>> gosa-core_v2.8
                                return;
                            } else {
                                $extra_headers[] = $param_value;
                            }
                        }
                        break;
<<<<<<< HEAD
                    case "proxy_host":
=======
                    case 'proxy_host':
>>>>>>> gosa-core_v2.8
                        if(!empty($param_value)) {
                            $proxy_host = $param_value;
                        }
                        break;
<<<<<<< HEAD
                    case "proxy_port":
                        if(!preg_match('!\D!', $param_value)) {
                            $proxy_port = (int) $param_value;
                        } else {
                            trigger_error("[plugin] invalid value for attribute '".$param_key."'",E_USER_NOTICE);
                            return;
                        }
                        break;
                    case "agent":
=======
                    case 'proxy_port':
                        if (!preg_match('!\D!', $param_value)) {
                            $proxy_port = (int)$param_value;
                        } else {
                            trigger_error("[plugin] invalid value for attribute '{$param_key }'", E_USER_NOTICE);
                            return;
                        }
                        break;
                    case 'agent':
>>>>>>> gosa-core_v2.8
                        if(!empty($param_value)) {
                            $agent = $param_value;
                        }
                        break;
<<<<<<< HEAD
                    case "referer":
=======
                    case 'referer':
>>>>>>> gosa-core_v2.8
                        if(!empty($param_value)) {
                            $referer = $param_value;
                        }
                        break;
<<<<<<< HEAD
                    case "timeout":
                        if(!preg_match('!\D!', $param_value)) {
                            $timeout = (int) $param_value;
                        } else {
                            trigger_error("[plugin] invalid value for attribute '".$param_key."'",E_USER_NOTICE);
=======
                    case 'timeout':
                        if (!preg_match('!\D!', $param_value)) {
                            $timeout = (int)$param_value;
                        } else {
                            trigger_error("[plugin] invalid value for attribute '{$param_key}'", E_USER_NOTICE);
>>>>>>> gosa-core_v2.8
                            return;
                        }
                        break;
                    default:
<<<<<<< HEAD
                        trigger_error("[plugin] unrecognized attribute '".$param_key."'",E_USER_NOTICE);
=======
                        trigger_error("[plugin] unrecognized attribute '{$param_key}'", E_USER_NOTICE);
>>>>>>> gosa-core_v2.8
                        return;
                }
            }
            if(!empty($proxy_host) && !empty($proxy_port)) {
                $_is_proxy = true;
                $fp = fsockopen($proxy_host,$proxy_port,$errno,$errstr,$timeout);
            } else {
                $fp = fsockopen($server_name,$port,$errno,$errstr,$timeout);
            }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
            if(!$fp) {
                trigger_error("[plugin] unable to fetch: $errstr ($errno)",E_USER_NOTICE);
                return;
            } else {
                if($_is_proxy) {
                    fputs($fp, 'GET ' . $params['file'] . " HTTP/1.0\r\n");
                } else {
                    fputs($fp, "GET $uri HTTP/1.0\r\n");
                }
                if(!empty($host)) {
                    fputs($fp, "Host: $host\r\n");
                }
                if(!empty($accept)) {
                    fputs($fp, "Accept: $accept\r\n");
                }
                if(!empty($agent)) {
                    fputs($fp, "User-Agent: $agent\r\n");
                }
                if(!empty($referer)) {
                    fputs($fp, "Referer: $referer\r\n");
                }
                if(isset($extra_headers) && is_array($extra_headers)) {
                    foreach($extra_headers as $curr_header) {
                        fputs($fp, $curr_header."\r\n");
                    }
                }
                if(!empty($user) && !empty($pass)) {
<<<<<<< HEAD
                    fputs($fp, "Authorization: BASIC ".base64_encode("$user:$pass")."\r\n");
                }

=======
                    fputs($fp, 'Authorization: BASIC ' . base64_encode("$user:$pass") . "\r\n");
                }
>>>>>>> gosa-core_v2.8
                fputs($fp, "\r\n");
                while(!feof($fp)) {
                    $content .= fgets($fp,4096);
                }
                fclose($fp);
                $csplit = preg_split("!\r\n\r\n!",$content,2);
<<<<<<< HEAD

                $content = $csplit[1];

=======
                $content = $csplit[ 1 ];
>>>>>>> gosa-core_v2.8
                if(!empty($params['assign_headers'])) {
                    $template->assign($params['assign_headers'],preg_split("!\r\n!",$csplit[0]));
                }
            }
        } else {
            trigger_error("[plugin fetch] unable to parse URL, check syntax",E_USER_NOTICE);
            return;
        }
    } else {
        $content = @file_get_contents($params['file']);
        if ($content === false) {
            throw new SmartyException("{fetch} cannot read resource '" . $params['file'] ."'");
        }
    }
<<<<<<< HEAD

=======
>>>>>>> gosa-core_v2.8
    if (!empty($params['assign'])) {
        $template->assign($params['assign'], $content);
    } else {
        return $content;
    }
}
<<<<<<< HEAD

?>
=======
>>>>>>> gosa-core_v2.8
