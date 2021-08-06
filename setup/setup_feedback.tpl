{if $feedback_send}
	<p class="green-text">{t}Feedback successfully send{/t}</p>
{else}

	<label>
		<input {if $subscribe} checked {/if} type='checkbox' name='subscribe' value='1'/>
		<span class="black-text">{t}Subscribe to the gosa-announce mailing list{/t}</span>
	</label>    

    <p>
		{t}When checking this option, GOsa will try to connect http://oss.gonicus.de in order to subscribe you to the gosa-announce mailing list. You've to confirm this by mail.{/t}
	</p>

	<table summary="{t}Organization{/t}">	
		<tbody>
			<tr>
				<td>{t}Organization{/t}</td>
				<td><input name='organization' type='text' value='{$organization}' style='width:300px;'></td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}Name{/t}</td>
				<td><input name='name' type='text' value='{$name}' style='width:300px;'></td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}Mail address{/t}{$must}</td>
				<td><input name='eMail' type='text' value='{$eMail}' style='width:300px;'></td>
			</tr>
		</tbody>
	</table>

    <hr>

	<label>
		<input {if $use_gosa_announce} checked {/if} type='checkbox' name='use_gosa_announce' value='1'/>
		<span class="black-text">{t}Send feedback to the GOsa project team{/t}</span>
	</label>    

	<p>
		{t}When checking this option, GOsa will try to connect http://oss.gonicus.de in order to submit your form anonymously.{/t}
	</p>

    <hr>
	<b>{t}Generic{/t}</b>
	<table summary="{t}Generic{/t}">	
		<tbody>
			<tr>
				<td>{t}Did the setup procedure help you to get started?{/t}</td>
				<td>
					<label>
						<input class="with-gap" {if $get_started} checked {/if} type='radio' name='get_started' value='1' />
						<span class="black-text">{t}Yes{/t}</span>
					</label>
					<br>
					<label>
						<input class="with-gap" {if !$get_started} checked {/if} type='radio' name='get_started' value='0' />
						<span class="black-text">{t}No{/t}</span>
					</label>
				</td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}If not, what problems did you encounter{/t}:</td>
				<td>
					<div class="input-field col s12">
						<textarea id="textarea1" class="materialize-textarea" name='problems_encountered' rows='4' cols='50'></textarea>
						<label for="textarea1">{$problems_encountered}</label>
					</div>
				</td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}Is this the first time you use GOsa?{/t}</td>
				<td>
					<div class="col s6">
						<label>
							<input class="with-gap" {if $first_use} checked {/if} type='radio' name='first_use' value='1' />
							<span class="black-text">{t}Yes{/t}</span>
						</label>
						<br>
						<label>
							<input class="with-gap" {if !$first_use} checked {/if} type='radio' name='first_use' value='0' />
							<span class="black-text">{t}No{/t}</span>
						</label>
					</div>
					<div class="col s6">
						<div class="input-field">
							<select name='use_since' title='{t}Select the year since when you are using GOsa{/t}'>
							{html_options options=$years}
							</select>
						</div>
					</div>
				</td>
			</tr>
		<tbody>
			<tr>
				<td>{t}What operating system / distribution do you use?{/t}</td>
				<td><input type='text' name='distribution' size=50 value='{$distribution}'></td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}What web server do you use?{/t}</td>
				<td><input type='text' size=50 name='web_server' value='{$web_server}'></td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}What PHP version do you use?{/t}</td>
				<td><input type='text' size=50 name='php_version' value='{$php_version}'></td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}GOsa version{/t}</td>
				<td>{$gosa_version}</td>
			</tr>
		</tbody>
		
	</table>

	<hr>
    <b>{t}LDAP{/t}</b>
	<table summary="{t}LDAP{/t}">
		<tbody>
			<tr>
				<td>{t}What kind of LDAP server(s) do you use?{/t}</td>
				<td><input type='text' name='ldap_server' size=50 value='{$ldap_server}'></td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}How many objects are in your LDAP?{/t}</td>
				<td><input type='text' name='object_count' size=50 value='{$object_count}'></td>
			</tr>
		</tbody>
	</table>

	<hr>
    <b>{t}Features{/t}</b>
	<table summary="{t}Features{/t}">
		<tbody>
			<tr>
				<td>{t}What features of GOsa do you use?{/t}</td>
				<td>
					{foreach from=$features_used item=data key=key}
					<label>
						<input type='checkbox' name='feature_{$key}' {if $data.USED} checked {/if} />
						<span class="black-text">{$data.NAME}</span>
						<br>
					</label>
					{/foreach}
				</td>
			</tr>
		</tbody>
		<tbody>
			<tr>
				<td>{t}What features do you want to see in future versions of GOsa?{/t}</td>
				<td>
					<textarea id="textarea2" class="materialize-textarea" name='want_to_see_next' cols=50 rows=3></textarea>
					<label for="textarea2">{$want_to_see_next}</label>
				</td>
			</tr>
		</tbody>
	</table>

<button class="btn-small gonicus-color" type='submit' name='send_feedback'>{t}Send feedback{/t}</button>

{/if}
<input type='hidden' name='step_feedback' value='1'>
