<p>
    {t}The main data source used in GOsa is LDAP. In order to access the information stored there, please enter the required information.{/t}
</p>

<hr>

{if $resolve_user}

    <b>{t}Please choose the LDAP user to be used by GOsa{/t}</b>

    <form class="col s12">
        <div class="row valign-wrapper">
            <div class="input-field col s6">
                <select name="admin_to_use">
                    {html_options options=$resolved_users selected=$admin}
                </select>
            </div>
            <div class="input-field col s5">
                <input type="search" id="search" name="resolve_filter" value='{$resolve_filter}'>
                <i class="material-icons">close</i>
            </div>
            <div class="col s1">
                <button class="btn-small gonicus-color right" type='submit' name='resolve_search'>{t}Search{/t}</button>
            </div>
        </div>
        <div class="row">
            <div class="col s12">
                <button class="btn-small gonicus-color" type='submit' name='use_selected_user'>{t}Apply{/t}</button>
                <button class="btn-small gonicus-color right" type='submit' name='resolve_user'>{t}Cancel{/t}</button>
            </div>
        </div>
    </form>

{else}

<b>{t}LDAP connection{/t}</b>
<table summary='{t}LDAP connection{/t}'>
    <tbody>
        <tr>
        <td>{t}Location name{/t}</td>
        <td><input type='text' name='location' maxlength='80' size='40' value='{$location}'></td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td>{t}Connection URI{/t}</td>
            <td><input type='text' name='connection' maxlength='80' size='40' value='{$connection}'></td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td>{t}TLS connection{/t}</td>
        <td>
            <select name="tls" size="1" title="">
                {html_options options=$bool selected=$tls}
            </select>
        </td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td>{t}Base{/t}</td>
            <td>
                <div class="row valign-wrapper">
                    <div class="input-fiel col s10">
                        {if $namingContextsCount >= 1}
                            <select name='base' size=1>
                                {html_options values=$namingContexts output=$namingContexts selected=$base}
                            </select>
                        {else}
                            <input type='text' name='base' maxlength='80' size='40' value='{$base}'>
                        {/if}
                    </div>
                    <div class="col s2">
                        <button class="btn-flat" type="submit" name="reload" title="{t}Reload{/t}"><i class="material-icons black-text md-36">refresh</i></button>
                    </div>
                </div>
            </td>
        </tr>
    </tbody>
</table>

<hr>
<b>{t}Authentication{/t}</b>

<table summary='{t}Authentication{/t}'>
    <tbody>
        <tr>
            <td>{t}Administrator DN{/t}</td>
            <td>
                <div class="row valign-wrapper">
                    <div class="col s8">
                        <input type='text' name='admin_given' maxlength='160' size='40' value='{$admin_given}'>
                    </div>
                    <div class="col s2">
                        {if $append_base_to_admin_dn},{$base_to_append}{/if}
                    </div>
                    <div class="col s2">
                        <button class="btn-flat" type="submit" name="resolve_user" title="{t}Select user{/t}"><i class="material-icons black-text md-36">folder_open</i></button>
                    </div>
                </div>
            </td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td></td>
            <td>
                <label class="black-text">
                    <input onClick='document.mainform.submit();' 
                    {if $append_base_to_admin_dn} checked {/if} 
                    type='checkbox' name='append_base_to_admin_dn' value='1'/>
                    <span>{t}Automatically append LDAP base to administrator DN{/t}</span>
                </label>                    
            </td>
        </tr>
    </tbody>
    <tbody>
        <tr>
            <td>{t}Administrator password{/t}</td>
            <td><input type='password' name='password' value='{$password}'></td>
        </tr>
    </tbody>
    
</table>

<hr> 
<b>{t}Schema based settings{/t}</b>
<table summary='{t}Schema based settings{/t}'>
    <tbody>
        <tr>
            <td>{t}Use RFC 2307bis compliant groups{/t}</td>
            <td>
                <select name="rfc2307bis" size="1" title="">
                    {html_options options=$bool selected=$rfc2307bis}
                </select>
            </td>
        </tr>
    </tbody>
</table>

{if !$resolve_user}
<hr> 
    <b>{t}Current status{/t}</b>
    <table>
        <tbody>
            <tr>
                <td>{t}Information{/t}</td>
                <td>{$connection_status}</td>
            </tr>
        </tbody>
    </table>
{/if}
{/if}
<script type="text/javascript" src="themes/materialize/js/materialize.js"></script>

<script type="text/javascript">
    document.addEventListener('DOMContentLoaded', function() {
        var elems = document.querySelectorAll('select');
        var instances = M.FormSelect.init(elems, {});
      });
</script>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('location');
  -->
</script>

