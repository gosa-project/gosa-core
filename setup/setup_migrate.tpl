<div>
    <div class='default'>

	{if $method == "default"}
			<p>{t}During the LDAP inspection, we're going to check for several common pitfalls that may occur when migration to GOsa base LDAP administration. You may want to fix the problems below, in order to provide smooth services.{/t}
			</p>

			{foreach from=$checks item=val key=key}
				<div style='width:98%; padding:4px; background-color:{cycle values="#F0F0F0, #FFF"}'>

			{if $checks.$key.ERROR_MSG}
                                <!-- Add ability to display info popup -->
                                <div class='step2_entry_container_info'>
                        {else}
                                <!-- Normal entry everything is fine -->
                                <div class='step2_entry_container'>
                        {/if}
				<div class='step2_entry_name'><b>{$checks.$key.TITLE}</b></div>
				<div class='step2_entry_status'>
				{if $checks.$key.STATUS}
					<div class='step2_successful'>{$checks.$key.STATUS_MSG}</div>
				{else}
					<div class='step2_failed'>{$checks.$key.STATUS_MSG}</div>
				{/if}
				</div>
					{if $checks.$key.ERROR_MSG}
						{$checks.$key.ERROR_MSG}
					{/if}
				</div>
				</div>
			{/foreach}
		<br>
		<button type='submit' name='reload'>{t}Check again{/t}</button>


		{elseif $method == "rootOC_migrate_dialog"}

			<h2>{t}Add required object classes to the LDAP base{/t}</h2>

			<b>{t}Current{/t}</b>
			<div class="step2_entry_container_info">
				<div style='padding-left:20px;'>
					<pre>{$details.current}</pre>
				</div>
			</div>
			<br>
			<b>{t}After migration{/t}</b>
			<div class="step2_entry_container_info">
				<div style='padding-left:20px;'>
					<pre>{$details.target}</pre>
				</div>
			</div>

			<br>
			<button type='submit' name='rootOC_migrate_start'>{t}Migrate{/t}</button>

			</p>
				

			<hr>	
			<div style='width:100%; text-align:right; padding:5px;'>
				<button type='submit' name='rootOC_dialog_cancel'>{t}Close{/t}</button>

			</div>

		{elseif $method == "create_acls"}

		{if $acl_create_selected != "" && $what_will_be_done_now!=""}
			<div>
<pre>
{$what_will_be_done_now}
</pre>
			</div>		
			<button type='submit' name='create_acls_create_confirmed'>{t}Next{/t}</button>

			<button type='submit' name='create_acls_create_abort'>{t}Abort{/t}</button>

		{else}
			<h2>{t}Create a new GOsa administrator account{/t}</h2>
	
			<p>
			{t}This dialog will automatically add a new super administrator to your LDAP tree.{/t}
			</p>
			<table>
				<tr>
					<td>
						{t}Name{/t}:&nbsp;
					</td>
					<td>
						<i>System administrator</i>
					</td>
				</tr>
				<tr>
					<td>
						{t}User ID{/t}:&nbsp;
					</td>
					<td>
						<input type='text' value='{$new_user_uid}' name='new_user_uid'><br>
					</td>
				</tr>
				<tr>
					<td>
						{t}Password{/t}:&nbsp;
					</td>
					<td>
						<input type='password' value='{$new_user_password}' name='new_user_password'><br>
					</td>
				</tr>
				<tr>
					<td>
						{t}Password (again){/t}:&nbsp;
					</td>
					<td>

						<input type='password' value='{$new_user_password2}' name='new_user_password2'><br>
					</td>
				</tr>
			</table>
	
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('new_user_password');
  -->
</script>

			<hr>	
			<div style='width:99%; text-align:right; padding:5px;'>
				<button type='submit' name='create_admin_user'>{t}Apply{/t}</button>	

				<button type='submit' name='create_acls_cancel'>{t}Cancel{/t}</button>

			</div>
			{/if}	
	{/if}
    </div>
</div>
