{if !$acl_readable}

	<h3>{msgPool type=permView}</h3>

{else}
	{if $dialogState eq 'head'}
  	<h3>{t}Assigned ACL for current entry{/t}</h3>
	  {$aclList}
    {if $acl_createable}
     <button type='submit' name='new_acl'>{t}New ACL{/t}</button>
 	  {/if}
	{/if}

	{if $dialogState eq 'create'}
    <h3>{t}Options{/t}</h3>
    <table summary='{t}Options{/t}'>
      <tr>
        <td>
  	      {t}ACL type{/t}
        </td>
        <td>
          {if !$acl_writeable}
            <select size="1" name="dummy_t" title="{t}Select an ACL type{/t}" disabled>
              {html_options options=$aclTypes selected=$aclType}
              <option disabled>&nbsp;</option>
            </select>&nbsp;
          {else} 
            <select size="1" name="aclType" title="{t}Select an ACL type{/t}" onChange="document.mainform.submit()">
              {html_options options=$aclTypes selected=$aclType}
              <option disabled>&nbsp;</option>
            </select size=1>&nbsp;
            {if $javascript eq 'false'}
              <button type='submit' name='refresh'>{t}Apply{/t}</button>
            {/if}
          {/if}
        </td>
      </tr>
      <tr>
        <td>
      	  {t}Additional filter options{/t}
        </td>
        <td>
  		    {if !$acl_writeable}
            <input type='text' value='{$aclFilter}' disabled name='dummy_f' style='width:600px;'>
          {else}
            <input type='text' value='{$aclFilter}' name='aclFilter' style='width:600px;'>
          {/if}
        </td>
      </tr>
    </table>

	<hr>
    <h3>{t}Members{/t}</h3>
    {$aclMemberList}
    <button type='submit'
      {if !$acl_writeable} disabled="disabled" {/if}
      name='add_user_or_group'>{msgPool type=addButton}</button>&nbsp;

    <button type='submit'
      {if !$acl_writeable} disabled="disabled" {/if}
      name='add_all_users'>{t}Add all users{/t}</button>&nbsp;

	{if $aclType ne 'reset'}
	{if $aclType ne 'role'}
	{if $aclType ne 'base'}
	<hr>

	<h3>{t}List of available ACL categories{/t}</h3>
	{$aclList}
	{/if}
	{/if}
	{/if}

	{if $aclType eq 'base'}
	<hr>
	<h3>{t}ACL for this object{/t}</h3>
	{$aclSelector}
	{/if}

	{if $aclType eq 'role'}
	<hr>
	<h3>{t}Available roles{/t}</h3>
	{$roleSelector}
	{/if}

	<hr>
	<div style='text-align:right;margin-top:5px'>
		{if $acl_writeable}
		<button type='submit' name='submit_new_acl'>{t}Apply{/t}</button>

		&nbsp;
		{/if}
		<button type='submit' name='cancel_new_acl'>{t}Cancel{/t}</button>

	</div>
	{/if}

	{if $dialogState eq 'edit'}

	<h3>{$headline}</h3>

	{$aclSelector}

	<hr>
	<div style='text-align:right;margin-top:5px'>
		<button type='submit' name='submit_edit_acl'>{t}Apply{/t}</button>

		&nbsp;
		<button type='submit' name='cancel_edit_acl'>{t}Cancel{/t}</button>

	</div>
	{/if}
{/if}
