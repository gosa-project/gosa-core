<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Generic{/t}</h2>
<table width="100%">
	<tr>
		<td style='width:50%;'>
			<table summary="">
				<tr>
					<td>{t}Server identifier{/t}{$must}
					</td>
					<td>
{render acl=$goImapNameACL}
						<input name="goImapName" id="goImapName" size=40 maxlength=60 value="{$goImapName}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Connect URL{/t}{$must}
					</td>
					<td>
{render acl=$goImapConnectACL}
						<input name="goImapConnect" id="goImapConnect" size=40 maxlength=60 value="{$goImapConnect}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Admin user{/t}{$must}
					</td>
					<td>
{render acl=$goImapAdminACL}
						<input name="goImapAdmin" id="goImapAdmin" size=30 maxlength=60 value="{$goImapAdmin}" >
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
					<td>{t}Sieve port{/t}{$must}
					</td>
					<td>
{render acl=$goImapSievePortACL}
						<input name="goImapSievePort" id="goImapSievePort" size=10 maxlength=30 value="{$goImapSievePort}">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			<table>
				<tr>
					<td>
{render acl=$cyrusImapACL}
						<input type='checkbox' name='cyrusImap' value=1 {if $cyrusImap} checked {/if} > 
{/render}
					</td>
					<td>{t}Start IMAP service{/t}
					</td>
				</tr>
				<tr>
					<td>
{render acl=$cyrusImapSSLACL}
						<input type='checkbox' name='cyrusImapSSL' value=1 {if $cyrusImapSSL} checked {/if}> 
{/render}
					</td>
					<td>{t}Start IMAP SSL service{/t}
					</td>
				</tr>
				<tr>
					<td>
{render acl=$cyrusPop3ACL}
						<input type='checkbox' name='cyrusPop3' value=1 {if $cyrusPop3} checked {/if} > 
{/render}
					</td>
					<td>{t}Start POP3 service{/t}
					</td>
				</tr>
				<tr>
					<td>
{render acl=$cyrusPop3SSLACL}
						<input type='checkbox' name='cyrusPop3SSL' value=1 {if $cyrusPop3SSL} checked {/if} > 
{/render}
					</td>
					<td>{t}Start POP3 SSL service{/t}
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<p class='seperator'>&nbsp;</p>
<br>
<h2><img class="center" alt="" src="images/rocket.png" align="middle"> Action</h2>
{if $is_new == "new"}
    {t}The server must be saved before you can use the status flag.{/t}
{elseif !$is_acc}
    {t}The service must be saved before you can use the status flag.{/t}
{/if}
<br>
<select name="action" title='{t}Set new status{/t}' 
	{if $is_new =="new" || !$is_acc} disabled {/if}
>
	<option value="none">&nbsp;</option>
    {html_options options=$Actions}
</select>
<input type='submit' name='ExecAction' title='{t}Set status{/t}' value='{t}Execute{/t}' 
	{if $is_new == "new" | !$is_acc} disabled {/if}
>

<p class="seperator">&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
<input type="hidden" name="goImapServerPosted" value="1">
