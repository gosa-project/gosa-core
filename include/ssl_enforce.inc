
<?php
/**
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2025 GONICUS GmbH
 *
 * ID: $$Id$$
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

function enforce_ssl_if_forced(): void
{
    global $config;

    if (!$config) {
        http_response_code(500);
        echo "Configuration missing or invalid. Please contact the administrator.";
        exit;
    }

    // if forceSSL in the gosa.conf is false, we can skip the ssl enforcement and return
    if ($config->get_cfg_value('core', 'forceSSL') !== 'true') {
        return;
    }

    // if the current request runs over https theres no need to redirect
    if (is_https()) {
        return;
    }

    $target = build_https_target();
    header('Location: ' . $target, true, 302);
    exit;
}

// detects if the current request runs over https
function is_https(): bool {
    if (!empty($_SERVER['HTTPS']) && strtolower($_SERVER['HTTPS']) === 'on') {
        return true;
    }
    
    return false;
}

// builds the target https url for redirecting
function build_https_target(): string {
    if (empty($_SERVER['REQUEST_URI'])) {
        return 'https://' . $_SERVER['HTTP_HOST'] . $_SERVER['PATH_INFO'];
    }
    
    return 'https://' . $_SERVER['HTTP_HOST'] . $_SERVER['REQUEST_URI'];
}
