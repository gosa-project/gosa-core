{if !$acl_readable}

	<h5>{msgPool type=permView}</h5>

{else}
	{if $dialogState eq 'head'}
  	<h5>{t}Assigned ACL for current entry{/t}</h5>
	  {$aclList}
    {if $acl_createable}
     	<button class="btn-small gonicus-color" type='submit' name='new_acl'>{t}New ACL{/t}</button>
 	  {/if}
	{/if}

	{if $dialogState eq 'create'}
    <h5>{t}Options{/t}</h5>
	<div class="input-field col s12">
		{if !$acl_writeable}
			<select size="1" name="dummy_t" title="{t}Select an ACL type{/t}" disabled>
				{html_options options=$aclTypes selected=$aclType}
			</select>
		{else}
			<select size="1" name="aclType" title="{t}Select an ACL type{/t}" onChange="document.mainform.submit()">
				{html_options options=$aclTypes selected=$aclType}
			</select size="1">
			{if $javascript eq 'false'}
				<button class="btn-small gonicus-btn" type='submit' name='refresh'>{t}Apply{/t}</button>
			{/if}
		{/if}
		<label>{t}ACL type{/t}</label>
		</div>
	<div class="input-field col s12">
		{if !$acl_writeable}
			<input id="acl_filter" type="text" value='{$aclFilter}' disabled name='dummy_f'>
		{else}
			<input id="acl_filter" type='text' value='{$aclFilter}' name='aclFilter'>
		{/if}
		<label for="acl_filter">{t}Additional filter options{/t}</label>
	</div>

	<hr>
    <h5>{t}Members{/t}</h5>
    {$aclMemberList}
    <button class="btn-small gonicus-color" type='submit'
      {if !$acl_writeable} disabled="disabled" {/if}
      name='add_user_or_group'>{msgPool type=addButton}</button>

    <button class="btn-small gonicus-color" type='submit'
      {if !$acl_writeable} disabled="disabled" {/if}
      name='add_all_users'>{t}Add all users{/t}</button>

	{if $aclType ne 'reset'}
	{if $aclType ne 'role'}
	{if $aclType ne 'base'}
	<hr>

	<h5>{t}List of available ACL categories{/t}</h5>
	{$aclList}
	{/if}
	{/if}
	{/if}

	{if $aclType eq 'base'}
	<hr>
	<h5>{t}ACL for this object{/t}</h5>
	{$aclSelector}
	{/if}

	{if $aclType eq 'role'}
	<hr>
	<h5>{t}Available roles{/t}</h5>
	{$roleSelector}
	{/if}

	<hr>
	<div class="row">
		{if $acl_writeable}
		<button class="btn gonicus-color" type='submit' name='submit_new_acl'>{t}Apply{/t}</button>

		&nbsp;
		{/if}
		<button class="btn gonicus-color mg2" type='submit' name='cancel_new_acl'>{t}Cancel{/t}</button>

	</div>
	{/if}

	{if $dialogState eq 'edit'}

	<h5>{$headline}</h5>

	{$aclSelector}

	<hr>
	<div style='text-align:right;margin-top:5px'>
		<button class="btn-small gonicus-color" type='submit' name='submit_edit_acl'>{t}Apply{/t}</button>

		&nbsp;
		<button class="btn-small gonicus-color" type='submit' name='cancel_edit_acl'>{t}Cancel{/t}</button>

	</div>
	{/if}
{/if}
