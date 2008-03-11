{if $dialogState eq 'head'}

<h1>{t}Assigned ACL for current entry{/t}</h1>
<table>
<tr>
	<td>
		{t}Name{/t}
	</td>
	<td>	
{render acl=$cnACL}
		<input type="text" name='cn' value="{$cn}" style='width:200px;'>
{/render}
	</td>
</tr>
<tr>
	<td>
		{t}Description{/t}
	</td>
	<td>
{render acl=$descriptionACL}
		<input type="text" name='description' value="{$description}" style='width:200px;'>
{/render}
	</td>
</tr>
<tr>
	<td>
		{t}Base{/t}
	</td>
	<td>
{render acl=$baseACL}
	      <select id="base" size="1" name="base" title="{t}Choose subtree to place group in{/t}">
	       {html_options options=$bases selected=$base_select}
	      </select>
{/render}

{render acl=$baseACL disable_picture='images/folder_gray.png'}
	        <input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
{/render}

	</td>
</tr>
</table>
{$aclList}
<input type="submit" name="new_acl" value="{t}New ACL{/t}">

{/if}

{if $dialogState eq 'create'}
<h1>{t}ACL type{/t} <select size="1" name="aclType" title="{t}Select an acl type{/t}" onChange="document.mainform.submit()">{html_options options=$aclTypes selected=$aclType}<option disabled>&nbsp;</option></select>&nbsp;{if $javascript eq 'false'}<input type="submit" value="{msgPool type=applyButton}" name="refresh">{/if}</h1>

<p class="seperator">&nbsp;</p>


<h1>{t}List of available ACL categories{/t}</h1>
{$aclList}

<p class="seperator">&nbsp;</p>
<div style='text-align:right;margin-top:5px'>
	<input type="submit" name="submit_new_acl" value="{msgPool type=applyButton}">
	&nbsp;
	<input type="submit" name="cancel_new_acl" value="{msgPool type=cancelButton}">
</div>
{/if}

{if $dialogState eq 'edit'}

<h1>{$headline}</h1>

{$aclSelector}

<p class="seperator">&nbsp;</p>
<div style='text-align:right;margin-top:5px'>
	<input type="submit" name="submit_edit_acl" value="{msgPool type=applyButton}">
	&nbsp;
	<input type="submit" name="cancel_edit_acl" value="{msgPool type=cancelButton}">
</div>
{/if}
<input type='hidden' name='acl_role_posted' value='1'>
