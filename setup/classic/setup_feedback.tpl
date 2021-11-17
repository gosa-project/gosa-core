{if $feedback_send}
	<font color='green'>{t}Feedback successfully send{/t}</font>
{else}
	
    <input {if $subscribe} checked {/if} type='checkbox' name='subscribe' value='1' class='center'>
    {t}Subscribe to the gosa-announce mailing list{/t}

    <p>{t}When checking this option, GOsa will try to connect http://oss.gonicus.de in order to subscribe you to the gosa-announce mailing list. You've to confirm this by mail.{/t}</p>

	<table summary="{t}Organization{/t}">	
		<tr>
			<td>{t}Organization{/t}</td>
			<td><input name='organization' type='text' value='{$organization}' style='width:300px;'></td>
		</tr>
		<tr>
			<td>{t}Name{/t}</td>
			<td><input name='name' type='text' value='{$name}' style='width:300px;'></td>
		</tr>
		<tr>
			<td>{t}Mail address{/t}{$must}</td>
			<td><input name='eMail' type='text' value='{$eMail}' style='width:300px;'></td>
		</tr>
	</table>

    <hr>

	<input {if $use_gosa_announce} checked {/if} type='checkbox' name='use_gosa_announce' value='1' class='center'>
    {t}Send feedback to the GOsa project team{/t}

	<p>
	{t}When checking this option, GOsa will try to connect http://oss.gonicus.de in order to submit your form anonymously.{/t}
	</p>

    <hr>
	<b>{t}Generic{/t}</b>
	<table  summary="{t}Generic{/t}">	
		<tr>
			<td>{t}Did the setup procedure help you to get started?{/t}</td>
			<td><input {if $get_started} checked {/if} type='radio' name='get_started' value='1'>{t}Yes{/t}
				<br>
				<input {if !$get_started} checked {/if} type='radio' name='get_started' value='0'>{t}No{/t}</td>
		</tr>
		<tr>
			<td>{t}If not, what problems did you encounter{/t}:</td>
			<td><textarea name='problems_encountered' rows='4' cols='50' style='width:100%'>{$problems_encountered}</textarea></td>
		</tr>
		<tr>
			<td>{t}Is this the first time you use GOsa?{/t}</td>
			<td>
				<input {if $first_use} checked {/if} type='radio' name='first_use' value='1'>{t}Yes{/t}
				<br>
				<input {if !$first_use} checked {/if} type='radio' name='first_use' value='0'>{t}No{/t},
				{t}I use it since{/t}
				<select name='use_since' title='{t}Select the year since when you are using GOsa{/t}' size=1> 
					{html_options options=$years}
				</select>
			</td>	
		</tr>
		<tr>
			<td>{t}What operating system / distribution do you use?{/t}</td>
			<td><input type='text' name='distribution' size=50 value='{$distribution}'></td>
		</tr>
		<tr>
			<td>{t}What web server do you use?{/t}</td>
			<td><input type='text' size=50 name='web_server' value='{$web_server}'></td>
		</tr>
		<tr>
			<td>{t}What PHP version do you use?{/t}</td>
			<td><input type='text' size=50 name='php_version' value='{$php_version}'></td>
		</tr>
		<tr>
			<td>{t}GOsa version{/t}</td>
			<td>{$gosa_version}</td>
		</tr>
	</table>

	<hr>
    <b>{t}LDAP{/t}</b>
	<table  summary="{t}LDAP{/t}">
		<tr>
			<td>{t}What kind of LDAP server(s) do you use?{/t}</td>
			<td><input type='text' name='ldap_server' size=50 value='{$ldap_server}'></td>
		</tr>
		<tr>
			<td>{t}How many objects are in your LDAP?{/t}</td>
			<td><input type='text' name='object_count' size=50 value='{$object_count}'></td>
		</tr>
	</table>

	<hr>
    <b>{t}Features{/t}</b>
	<table  summary="{t}Features{/t}">
		<tr>
			<td>{t}What features of GOsa do you use?{/t}</td>
			<td>
				{foreach from=$features_used item=data key=key}
					<input type='checkbox' name='feature_{$key}' {if $data.USED} checked {/if}>
					{$data.NAME}<br>
				{/foreach}
			</td>
		</tr>
		<tr>
			<td>{t}What features do you want to see in future versions of GOsa?{/t}</td>
			<td><textarea name='want_to_see_next' cols=50 rows=3>{$want_to_see_next}</textarea></td>
		</tr>
	</table>
<button type='submit' name='send_feedback'>{t}Send feedback{/t}</button>

{/if}
<input type='hidden' name='step_feedback' value='1'>
