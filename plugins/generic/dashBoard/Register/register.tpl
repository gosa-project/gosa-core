<script type="text/javascript" src="include/pwdStrength.js"></script>

<h3>{t}GOsa registration{/t}</h3>


{if $step == 0}

    {t}Do you want to register GOsa and benefit from the features it brings?{/t}
    <p>
     <input type='radio' name='registrationType' value='' id="registrationType_"
         {if $default == ""} checked {/if}><b><LABEL for="registrationType_">{t}I do not want to register{/t}</LABEL></b>
        <p style='padding-left:20px;'>
            <!-- Add a descritive text later -->
        </p>

        <!--
        <input type='radio' name='registrationType' value='annonym' id="registrationType_annonym"
            {if $default == "annonym"} checked {/if}><b><LABEL for="registrationType_annonym">{t}Annonym access{/t}</LABEL></b>
        <p style='padding-left:20px;'>
        <ul>
            <li>{t}Access to the 'Open-Channels' to easily install or remove plugins within the GOsa user interface{/t}</li>
            <li>{t}Use the inbuild bugtracker and keep track of their status on the dash-board.{/t}</li>
        </ul>
        </p>

        <p style='padding-left:20px;'>
        {t}What information will be transmitted to the backend and maybe stored:{/t}
        <ul>
            <li>{t}Information about the installed plugins and their version.{/t}</li>
            <li>{t}The GOsa-UUID (will be generated during the registration) and a password, to authenticate.{/t}</li>
            <li>{t}The bugs you will report and the corresponding trace. You can select what information you want to send in.{/t}</li>
        </ul>
        </p>
        -->
        <input type='radio' name='registrationType' value='registered' id="registrationType_registered"
            {if $default == "registered"} checked {/if}><b><LABEL for="registrationType_registered">{t}Register{/t}</LABEL></b>
        <p style='padding-left:20px;'>
        {t}Additionally to the 'Annonomous' account you can:{/t}
        <ul>
            <li>{t}Access to 'Premium-Channels'.{/t}</li>
            <li>{t}Watch the status of current plugin updates/patches and the availability of new plugins.{/t}</li>
            <li>{t}Recieve newsletter, if wanted.{/t}</li>
            <li>{t}View several usefull statistics about your GOsa installation{/t}.</li>
        </ul>
        </p>
           
        <p style='padding-left:20px;'>
        {t}What information will be transmitted to the backend and maybe stored:{/t}
        <ul>
            <li>{t}All personal information filled in the registration form.{/t}</li>
            <li>{t}Information about the installed plugins and their version.{/t}</li>
            <li>{t}The GOsa-UUID (will be generated during the registration) and a password, to authenticate.{/t}</li>
            <li>{t}The bugs you will report and the corresponding trace. You can select what information you want to send in.{/t}</li>
            <li>{t}When the statistics extension is used. GOsa will transmit information about plugins, their usage and the amount of objects present in your ldap database. No sensitive data is transmitted here, just the object type, the action performed, cpu usage, memory usage, elapsed time...{/t}</li>
        </ul>
        </p>
    </p>
    <hr>
    <div class="plugin-actions">
        <button name='startRegistration'>{msgPool type=okButton}</button>
    </div>

{/if}

{if $step == 1}

    {if $default == ""}
        <b>{t}Selected registration type:{/t} {$default}</b>
    {elseif $default == "annonym"}
        <b>{t}Selected registration type:{/t} {$default}</b>
    {elseif $default == "registered"}
        
        <h3>{t}Account settings{/t}</h3>
        <table style='width:100%;'>
            <tr> 
                <td style='width:220px;'><LABEL for="uid">{t}Login-ID{/t}{$must}</LABEL></td>
                <td>
                    <input type='text' name="uid" value="{$uid}" id="uid">
                </td>
            </tr>
            <tr> 
                <td style='width:220px;'><LABEL for="mailAddress">{t}Mail address{/t}{$must}</LABEL></td>
                <td>
                    <input type='text' name="mailAddress" value="{$mailAddress}" id="mailAddress">
                </td>
            </tr>
        </table>

        <hr>

        <h3>{t}Password{/t}</h3>
        <table style='width:100%;'>
            <tr> 
                <td style='width:220px;'><LABEL for="password">{t}Password{/t}{$must}</LABEL></td>
                <td>
                    {factory type='password' name='password' id='password' value={$password}
                          onkeyup="testPasswordCss(\$('password').value)"}
                </td>
            </tr>
            <tr>
                <td></td>
                <td>
                    <div style='width:200px'>
                    <span id="meterEmpty" style="padding:0;margin:0;width:100%;
                            background-color:#DC143C;display:block;height:7px;">
                    <span id="meterFull" style="padding:0;margin:0;z-index:100;
                            width:0;background-color:#006400;display:block;height:7px;"></span></span>
                    </div>
                </td>
            </tr>
            <tr> 
                <td style='width:220px;'><LABEL for="password_repeated">{t}Password{/t} <i>({t}repeated{/t})</i>{$must}</LABEL></td>
                <td>
                    {factory type='password' name='password_repeated' id='password_repeated' value={$password_repeated}}
                </td>
            </tr>
            <tr> 
                <td style='width:220px;'><LABEL for="restoreQuestion">{t}Password restore question{/t}{$must}</LABEL></td>
                <td>
                    <input type='text' style='width:400px' name="restoreQuestion" value="{$restoreQuestion}" id="restoreQuestion">
                </td>
            </tr>
            <tr> 
                <td style='width:220px;'><LABEL for="restoreAnswer">{t}Password restore answer{/t}{$must}</LABEL></td>
                <td>
                    <input type='text' name="restoreAnswer" value="{$restoreAnswer}" id="restoreAnswer">
                </td>
            </tr>
        </table>
        <hr>
       
        <h3>{t}Personal information{/t}</h3>
        <table style='width:100%;'>
            <tr> 
                <td style='width:220px;'><LABEL for="surname">{t}Surname{/t}{$must}</LABEL></td>
                <td>
                    <input type='text' name="surname" value="{$surname}" id="surname">
                </td>
            </tr>
            <tr> 
                <td style='width:220px;'><LABEL for="givenName">{t}Given name{/t}{$must}</LABEL></td>
                <td>
                    <input type='text' name="givenName" value="{$givenName}" id="givenName">
                </td>
            </tr>
        </table>

        <hr>
        <h3>{t}Newsletter{/t}</h3>
        <table style='width:100%;'>
            <tr> 
                <td style='width:220px;'><LABEL for="newsletter">{t}Newsletter{/t}</LABEL></td>
                <td>
                    <input type='checkbox' name="newsletter" value="1" {if $newsletter} checked {/if}id="newsletter">
                </td>
            </tr>
        </table>
        <hr>
        <i>({$must} {t}Required fields{/t})</i>
    {/if}

    <hr>
    <div class="plugin-actions">
        <button name='stepBack'>{msgPool type=backButton}</button>
        <button name='registerPage2'>{msgPool type=okButton}</button>
    </div>
    <script type="text/javascript">
        focus_field('uid');
    </script>
{/if}


{if $step == 2}

    <h3>{t}Company information{/t}</h3>
    <table style='width:100%;'>
        <tr> 
            <td style='width:220px;'><LABEL for="company">{t}Company name{/t}</LABEL></td>
            <td>
                <input type='text' name="company" value="{$company}" id="company">
            </td>
        </tr>
        <tr> 
            <td style='width:220px;'><LABEL for="street">{t}Street{/t}</LABEL></td>
            <td>
                <input type='text' name="street" value="{$street}" id="street">
            </td>
        </tr>
        <tr> 
            <td style='width:220px;'><LABEL for="city">{t}City{/t}</LABEL></td>
            <td>
                <input type='text' name="city" value="{$city}" id="city">
            </td>
        </tr>
        <tr> 
            <td style='width:220px;'><LABEL for="postal_code">{t}Postal code{/t}</LABEL></td>
            <td>
                <input type='text' name="postal_code" value="{$postal_code}" id="postal_code">
            </td>
        </tr>
    </table>
    <hr>

    <h3>{t}Contact information{/t}</h3>
    <table style='width:100%;'>
        <tr> 
            <td style='width:220px;'><LABEL for="phone">{t}Contact phone number{/t}</LABEL></td>
            <td>
                <input type='text' name="phone" value="{$phone}" id="phone">
            </td>
        </tr>
        <tr> 
            <td style='width:220px;'><LABEL for="contact_mail">{t}Contact mail address{/t}</LABEL></td>
            <td>
                <input type='text' name="contact_mail" value="{$contact_mail}" id="contact_mail">
            </td>
        </tr>
    </table>
    <hr>

    <h3>{t}Details{/t}</h3>
    <table style='width:100%;'>
        <tr> 
            <td style='width:220px;'><LABEL for="employees">{t}Number of employees{/t}</LABEL></td>
            <td>
                <input type='text' name="employees" value="{$employees}" id="employees">
            </td>
        </tr>
        <tr> 
            <td style='width:220px;'><LABEL for="trade">{t}What does your company deal with?{/t}</LABEL></td>
            <td>
                <textarea style="width:240px;" rows=3 id='trade' name="trade">{$trade}</textarea>
            </td>
        </tr>
        <tr> 
            <td style='width:220px;'><LABEL for="description">{t}Additional information{/t}</LABEL></td>
            <td>
                <textarea style="width:240px;" rows=3 id='description' name="description">{$description}</textarea>
            </td>
        </tr>
    </table>

    <hr>
    <div class="plugin-actions">
        <button name='stepBack'>{msgPool type=backButton}</button>
        <button name='registerPage3'>{msgPool type=okButton}</button>
    </div>
{/if}

{if $step == 200}
   <h3>VERIFY TEXTS!!!</h3> You have receifed a mail, bafasel, please click link and so on.

{/if}
