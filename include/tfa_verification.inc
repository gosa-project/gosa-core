<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: functions.inc 13100 2008-12-01 14:07:48Z hickert $$
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

declare(strict_types=1);

/* Include required files */
include "sonata/FixedBitNotation.php";
include "sonata/GoogleAuthenticator.php";
include "sonata/GoogleQrUrl.php";

/* Generates the QR-Code grafic */
function generate_qrCode ($uid, $token)
{
    return GoogleQrUrl::generate($uid, bin2base32($token), 'GOsa');
}

/* Verify the entered code with the user specific secret */
function verify_secret ($token, $code, $otp_config)
{
    $secret = bin2base32($token);

    /* Use the values from the ldap to configure the google authenticator */
    $passCodeLength = intval($otp_config['oathOTPLength']);
    $secretLength = mb_strlen($token , '8bit');
    $codePeriod = intval($otp_config['oathTOTPTimeStepPeriod']);
    $g = new GoogleAuthenticator($passCodeLength, $secretLength, null, $codePeriod);

    /* Check code and return the result */
    return $g->checkCode($secret, $code);
}

/* Encode binary secret to base 32 */
function bin2base32 ($token) 
{
    $base32 = new FixedBitNotation(5, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ234567', true, true);
    $secret = $base32->encode($token);

    return $secret;
}

?>