	{if $method == "default"}
		<p>{t}During the LDAP inspection, we're going to check for several common pitfalls that may occur when migration to GOsa base LDAP administration. You may want to fix the problems below, in order to provide smooth services.{/t}
		</p>

        <table style='width:100%'>
            {foreach from=$checks item=val key=key}
                <tr> 
                    <td>
                        <b>{$checks.$key.TITLE}</b>&nbsp; {$checks.$key.STATUS_MSG}
                        <p>
                            {if $checks.$key.ERROR_MSG}
                                {$checks.$key.ERROR_MSG}
                            {/if}
                        </p>
                    </td>
                </tr>
            {/foreach}
        </table>
		<br>
		<button type='submit' name='reload'>{t}Check again{/t}</button>

	{elseif $method == "rootOC_migrate_dialog"}

			<h2>{t}Add required object classes to the LDAP base{/t}</h2>

			<b>{t}Current{/t}</b>
			<pre>{$details.current}</pre>

			<br>
			<b>{t}After migration{/t}</b>
    		<pre>{$details.target}</pre>

			<br>
			<button type='submit' name='rootOC_migrate_start'>{t}Migrate{/t}</button>
			<hr>	
			<div class="plugin-actions">
				<button type='submit' name='rootOC_dialog_cancel'>{t}Close{/t}</button>
			</div>

		{elseif $method == "create_acls"}

    <h2>{t}Create a new GOsa administrator account{/t}</h2>
    <p>{t}This dialog will automatically add a new super administrator to your LDAP tree.{/t}</p>

    <table>
        <tr>
            <td>{t}Name{/t}:</td>
            <td><i>System administrator</i></td>
        </tr>
        <tr>
            <td>{t}User ID{/t}:&nbsp;</td>
            <td><input type='text' value='{$new_user_uid}' name='new_user_uid'><br></td>
        </tr>
        <tr>
            <td>{t}Password{/t}:&nbsp;</td>
            <td><input type='password' value='{$new_user_password}' name='new_user_password'><br></td>
        </tr>
        <tr>    
            <td>{t}Password (again){/t}:&nbsp;</td>
            <td><input type='password' value='{$new_user_password2}' name='new_user_password2'></td>
        </tr>
    </table>

    <script language="JavaScript" type="text/javascript">
        <!-- // First input field on page
            focus_field('new_user_password');
        -->
    </script>

    <hr>	
    <div class="plugin-actions">
        <button type='submit' name='create_admin_user'>{t}Apply{/t}</button>	
        <button type='submit' name='create_acls_cancel'>{t}Cancel{/t}</button>
    </div>
{/if}
