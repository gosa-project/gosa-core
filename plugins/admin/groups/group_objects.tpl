<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top; width:600px">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
     <b><LABEL for="users">{t}Select users to add{/t}</LABEL> {$hint}</b>
    </p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <select id="users" style="width:600px; height:450px; margin-top:4px;" name="users[]" size="15" multiple>
      {html_options options=$users}
     </select>
     <input type=hidden name="edit_helper">
    </p>
   </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px"><img src="{$launchimage}" align="right" alt="[F]"><b>{t}Filters{/t}</b></p>
   </div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;background-color:#F8F8F8">
      {$alphabet}
     </table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		<tr>
			<td>
				<input type=checkbox name="SubSearchGroup"  value="1" {if $SubSearchGroup == 1} checked {/if} 
					 onClick="mainform.submit()" title="{t}Select to see servers{/t}">{t}Search within subtree{/t}<br>
			</td>
		</tr>	
	</table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
		<tr>
			<td>
				<img alt="" src="{$tree_image}" align=middle title="{t}Display users of department{/t}">
				&nbsp;
				<select name="dselect" id="dselect" size=1 onChange="mainform.submit()" 
					title="{t}Choose the department the search will be based on{/t}">
      				{html_options options=$deplist selected=$dselect}
    			</select>
			</td>
		</tr>
	</table>
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8"><tr><td style="width:18px"><img alt="" src="{$search_image}" align=middle title="{t}Display users matching{/t}"></td>
    <td><input type='text' name='regex' maxlength='20' style='width:99%' value='{$regex}' title='{t}Regular expression for matching user names{/t}' onChange="mainform.submit()"></td></tr></table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<p class="plugbottom">
  <input type=submit name="add_users_finish" value="{msgPool type=addButton}">
  &nbsp;
  <input type=submit name="add_users_cancel" value="{msgPool type=cancelButton}">
</p>

