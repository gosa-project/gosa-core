<script type="text/javascript" src="include/pwdStrength.js"></script>

<h3>{t}GOsa registration{/t}</h3>

{if $step == 0}

    {t}Do you want to register GOsa and benefit from the features it brings?{/t}
    <p>
     <input type='radio' name='registrationType' value='dontWant' id="registrationType_dontWant"
         {if $default == "dontWant"} checked {/if}>
            <b><LABEL for="registrationType_dontWant">{t}I do not want to register{/t}</LABEL></b>
        <p style='padding-left:20px;'>
            <!-- Add a descritive text later -->
        </p>

        <input type='radio' name='registrationType' value='registrate' id="registrationType_registrate"
            {if $default == "registrate"} checked {/if}><b><LABEL for="registrationType_registrate">{t}Register{/t}</LABEL></b>
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

{if $step == 1 && $default == "registrate"}
    <table>
        <tr>
            <td><LABEL for="username">{t}Login{/t}</LABEL></td>
            <td><input type='text' id='username' name='username' value="{$username}"></td>
        </tr>
        <tr>
            <td><LABEL for="password">{t}Password{/t}</LABEL></td>
            <td>{factory type='password' name='password' id='password' value={$password}}</td>
        </tr>
        <tr>
            <td>&nbsp;</td>
            <td><div style="color: red; font-size: small;"><i>{$error}</i></div></td>
        </tr>
    </table>
    <hr>
    <div class="plugin-actions">
        <button name='stepBack'>{msgPool type=backButton}</button>        
        <button name='registerPage1'>{msgPool type=okButton}</button>        
    </div>
{/if}

{if $step == 2 && $default == "registrate"}
    <h3>{t}Registration complete{/t}</h3>
    <p>
        {t}GOsa instance successfully registered{/t}
    </p>
    <hr>
    <div class="plugin-actions">
        <button name='registerComplete'>{msgPool type=okButton}</button>        
    </div>
{/if}

{if $step == 1 && $default == "dontWant"}
    <h3>{t}Registration complete{/t}</h3>
    <p>
        {t}GOsa instance will not be registered{/t}
    </p>
    <hr>
    <div class="plugin-actions">
        <button name='stepBack'>{msgPool type=backButton}</button>        
        <button name='registerComplete'>{msgPool type=okButton}</button>        
    </div>
{/if}
