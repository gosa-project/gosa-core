<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of users{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
    <p class="contentboxb">
      <select style="width:500px; height:450px;" name="userlist" size=15 ondblclick="this.form.edit_helper.value='1'; this.form.submit()">
			{html_options options=$userlist}
			<option disabled></option>
      </select>
      <br>
      <input type=submit value="{t}New{/t}..." name="new_user">&nbsp;
      <input type=submit value="{t}New template{/t}" name="new_template">&nbsp;
      <input type=submit value="{t}Edit{/t}" name="select_user">&nbsp;
      <input type=submit value="{t}Delete{/t}" name="delete_user">&nbsp;
      <input type=submit value="{t}Set password{/t}" name="setpass_user">
      <input type=hidden name="edit_helper">
    </p>
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
    {t}This menu allows you to create, edit and delete selected users. Having a great number of users, you may want to use the range selectors on top of the user list.{/t}
    </p>
   <p class="contentboxb">
   {t}-Edit- and -New...- execute an assistant to aid you in managing your account informations. -Delete- will ask for confirmation before deleting users.{/t}
   </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
	<table summary="" style="border-top:1px solid #B0B0B0;" width="100%">
		<tr>
			<td>
				<input type=checkbox name="templates" value="1" {$templates} onClick="mainform.submit()" title="{t}Select to see template pseudo users{/t}">{t}Show templates{/t}<br>
				<input type=checkbox name="functionalusers" value="1" {$functionalusers} onClick="mainform.submit()" title="{t}Select to see users that have only a GOsa object{/t}">{t}Show functional users{/t}<br>
				<input type=checkbox name="unixusers" value="1" {$unixusers} onClick="mainform.submit()" title="{t}Select to see users that have posix settings{/t}">{t}Show unix users{/t}<br>
				<input type=checkbox name="mailusers" value="1" {$mailusers} onClick="mainform.submit()" title="{t}Select to see users that have mail settings{/t}">{t}Show mail users{/t}<br>
				<input type=checkbox name="sambausers" value="1" {$sambausers} onClick="mainform.submit()" title="{t}Select to see users that have samba settings{/t}">{t}Show samba users{/t}<br>
				<input type=checkbox name="proxyusers" value="1" {$proxyusers} onClick="mainform.submit()" title="{t}Select to see users that have proxy settings{/t}">{t}Show proxy users{/t}<br>

            </td>
        </tr>
    </table>
    <table summary="" style="border-top:1px solid #B0B0B0;" width="100%">
        <tr>
            <td>

				<input type=checkbox name="subsearch" value="1" {$subsearch} onClick="mainform.submit()" title="{t}Dive into LDAP subtrees when searching{/t}">{t}Search in subtrees{/t}<br>
			</td>
		</tr>
	</table>	

			<table summary="" width="100%" style="border-top:1px solid #B0B0B0;"><tr><td width="50%"><img alt="" src="{$tree_image}" align="middle">&nbsp;{t}Display users of department{/t}</td>
    <td><select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
      {html_options options=$deplist selected=$depselect}
    </select></td></tr></table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;"><tr><td width="50%"><img alt="" src="{$search_image}" align="middle">&nbsp;{t}Display users matching{/t}</td>
    <td><input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching user names{/t}' onChange="mainform.submit()"></td></tr></table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
