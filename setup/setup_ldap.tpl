
	{if $resolve_user}

		<b>{t}Please choose the LDAP user to be used by GOsa{/t}</b>
		<select name='admin_to_use' size=20 style="width:100%; margin-bottom:10px;">				
			{html_options options=$resolved_users selected=$admin}
		</select>

		<input type='text' value='{$resolve_filter}' name='resolve_filter'>
		<button type='submit' name='resolve_search'>{t}Search{/t}</button>
		
        <hr>
        <div class="plugin-actions">
            <button type='submit' name='use_selected_user'>{t}Apply{/t}</button>
            <button type='submit' name='resolve_user'>{t}Cancel{/t}</button>
		</div>
	</div>		
	
	<div style="clear:both;"></div>

	{else}

	<b>{t}LDAP connection{/t}</b>
    <table style='width:100%'>
        <tr>
            <td style='width:200px;'>{t}Location name{/t}</td>
            <td><input type='text' name='location' maxlength='80' size='40' value='{$location}'></td>    
        </tr>
        <tr>
            <td>{t}Connection URI{/t}</td>
			<td><input type='text' name='connection' maxlength='80' size='40' value='{$connection}'></td>
        </tr>
        <tr>
            <td>{t}TLS connection{/t}</td>
            <td>
                <select name="tls" size="1" title="">
                    {html_options options=$bool selected=$tls}
                </select>
            </td>
        </tr>
        <tr>
            <td>{t}Base{/t}</td>
            <td>
                {if $namingContextsCount >= 1}
                    <select name='base' size=1>
                        {html_options values=$namingContexts output=$namingContexts selected=$base}
                    </select>
                {else}
                    <input type='text' name='base' maxlength='80' size='40' value='{$base}'>
                {/if}
            <input type='image' class='center' src='images/lists/reload.png' 
                title='{t}Reload{/t}' name='reload' alt='{t}Reload{/t}'>
            </td>
        </tr>
    </table>

    <hr> 
	<b>{t}Authentication{/t}</b>
    <table style='width:100%'>
        <tr>
            <td style='width:200px;'>{t}Admin DN{/t}</td>
            <td>
                <input type='text' name='admin_given' maxlength='160' size='40' value='{$admin_given}'>
			    {if $append_base_to_admin_dn},{$base_to_append}{/if}
			    <input type='image' class='center' src='images/lists/folder.png' 
                    title='{t}Select user{/t}' name='resolve_user' alt='{t}Select user{/t}'>
            </td>
        </tr>
        <tr>
            <td>
            </td>    
            <td>
                <input onClick='document.mainform.submit();' 
                    {if $append_base_to_admin_dn} checked {/if} 
                    type='checkbox' name='append_base_to_admin_dn' value='1'>&nbsp;
                {t}Automatically append LDAP base to admin DN{/t}
            </td>
        </tr>
        <tr>
            <td>{t}Admin password{/t}</td>
            <td><input type='password' name='password' maxlength='20' size='20' value='{$password}'></td>
        </tr>
    </table>

    <hr> 
	<b>{t}Schema based settings{/t}</b>
    <table style='width:100%'>
        <tr>
            <td style='width:200px;'>{t}Use rfc2307bis compliant groups{/t}</td>
            <td>
                <select name="rfc2307bis" size="1" title="">
                   {html_options options=$bool selected=$rfc2307bis}
                </select>
            </td>
        </tr>
    </table>

	{if !$resolve_user}
    <hr> 
        <b>{t}Current status{/t}</b>
        <table style='width:100%'>
            <tr>
                <td style='width:200px;'>{t}Information{/t}</td>
                <td>{$connection_status}</td>
            </tr>
        </table>
	{/if}
{/if}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('location');
  -->
</script>

