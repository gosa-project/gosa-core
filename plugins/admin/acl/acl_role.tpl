{if $dialogState eq 'head'}

<h1>{t}Assigned ACL for current entry{/t}</h1>
<table summary="{t}Assigned ACL for current entry{/t}">
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
		{t}Base{/t}{$MUST}
	</td>
	<td>
{render acl=$baseACL}
  {$base}
{/render}
	</td>
</tr>
</table>
{$aclList}
{render acl=$gosaAclEntryACL}
<button type='submit' name='new_acl'>{t}New ACL{/t}</button>

{/render}

{/if}

{if $dialogState eq 'create'}
<h1>{t}ACL type{/t} <select size="1" name="aclType" title="{t}Select an acl type{/t}" onChange="document.mainform.submit()">{html_options options=$aclTypes selected=$aclType}<option disabled>&nbsp;</option></select>&nbsp;{if $javascript eq 'false'}<button type='submit' name='refresh'>{msgPool type=applyButton}</button>{/if}
</h1>

<hr>


<h1>{t}List of available ACL categories{/t}</h1>
{$aclList}

<hr>
<div style='text-align:right;margin-top:5px'>
{render acl=$gosaAclEntryACL}
	<button type='submit' name='submit_new_acl'>{msgPool type=applyButton}</button>

	&nbsp;
{/render}
	<button type='submit' name='cancel_new_acl'>{msgPool type=cancelButton}</button>

</div>
{/if}

{if $dialogState eq 'edit'}

<h1>{$headline}</h1>

{render acl=$gosaAclEntryACL}
{$aclSelector}
{/render}

<hr>
<div style='text-align:right;margin-top:5px'>
{render acl=$gosaAclEntryACL}
	<button type='submit' name='submit_edit_acl'>{msgPool type=applyButton}</button>

{/render}
	&nbsp;
	<button type='submit' name='cancel_edit_acl'>{msgPool type=cancelButton}</button>

</div>
{/if}
<input type='hidden' name='acl_role_posted' value='1'>
