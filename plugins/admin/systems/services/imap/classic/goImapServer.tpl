<h3>{t}Generic{/t}</h3>
<table width="100%" summary="{t}IMAP service{/t}">
	<tr>
		<td style='width:50%;'>
			<table summary="{t}Generic settings{/t}">
				<tr>
					<td>{t}Server identifier{/t}{$must}
					</td>
					<td>
{render acl=$goImapNameACL}
						<input type='text' name="goImapName" id="goImapName" size=40 maxlength=60 value="{$goImapName}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Connect URL{/t}{$must}
					</td>
					<td>
{render acl=$goImapConnectACL}
						<input type='text' name="goImapConnect" id="goImapConnect" size=40 maxlength=100 value="{$goImapConnect}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Administrator{/t}{$must}
					</td>
					<td>
{render acl=$goImapAdminACL}
						<input type='text' name="goImapAdmin" id="goImapAdmin" size=30 maxlength=60 value="{$goImapAdmin}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Password{/t}{$must}
					</td>
					<td>
{render acl=$goImapPasswordACL}
					<input type=password name="goImapPassword" id="goImapPassword" size=30 maxlength=60 value="{$goImapPassword}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Sieve connect URL{/t}{$must}
					</td>
					<td>
{render acl=$goImapSieveServerACL}
						<input type='text' name="goImapSieveServer" id="goImapSieveServer" size=30 maxlength=60 value="{$goImapSieveServer}">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td class='left-border'>
      {render acl=$cyrusImapACL}
        <input type='checkbox' name='cyrusImap' value=1 {if $cyrusImap} checked {/if} > 
      {/render}
      {t}Start IMAP service{/t}
      <br>

      {render acl=$cyrusImapSSLACL}
       <input type='checkbox' name='cyrusImapSSL' value=1 {if $cyrusImapSSL} checked {/if}> 
      {/render}
      {t}Start IMAP SSL service{/t}
      <br>

      {render acl=$cyrusPop3ACL}
        <input type='checkbox' name='cyrusPop3' value=1 {if $cyrusPop3} checked {/if} > 
      {/render}
      {t}Start POP3 service{/t}
      <br>

      {render acl=$cyrusPop3SSLACL}
        <input type='checkbox' name='cyrusPop3SSL' value=1 {if $cyrusPop3SSL} checked {/if} > 
      {/render}
      {t}Start POP3 SSL service{/t}
		</td>
	</tr>
</table>
<hr>
<br>
<h3>Action</h3>

{if $is_new == "new"}
 {t}The server must be saved before you can use the status flag.{/t}
{elseif !$is_acc}
 {t}The service must be saved before you can use the status flag.{/t}
{/if}

<br>
<select name="action" title='{t}Set new status{/t}'
	{if $is_new =="new" || !$is_acc} disabled {/if}>
	<option value="none">&nbsp;</option>
    {html_options options=$Actions}
</select>

<button type='submit' name='ExecAction' title="{t}Set status{/t}"
	{if $is_new == "new" || !$is_acc} disabled {/if}>{t}Execute{/t}</button>

<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>

<input type="hidden" name="goImapServerPosted" value="1">
