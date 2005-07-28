<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:50%;">
  <div class="contentboxh">
    <p class="contentboxh">
     <LABEL for="userlist">{t}List of users{/t}</LABEL> {$hint}
    </p>
  </div>
  <div class="contentboxb">
		{$userlisthead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
			{$userlist}
      <input type=hidden name="edit_helper">
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
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]"><LABEL for='filter'>{t}Filters{/t}</LABEL></p>
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
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;"><tr><td><img alt="Search" src="{$search_image}" align="middle" title="{t}Display users matching{/t}"></td>
    <td><input id='filter' style='width:99%' type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching user names{/t}' onChange="mainform.submit()"></td></tr></table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
