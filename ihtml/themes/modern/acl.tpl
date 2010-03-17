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
            <select size="1" name="dummy_t" title="{t}Select an acl type{/t}" disabled>
              {html_options options=$aclTypes selected=$aclType}
              <option disabled>&nbsp;</option>
            </select>&nbsp;
          {else} 
            <select size="1" name="aclType" title="{t}Select an acl type{/t}" onChange="document.mainform.submit()">
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
	<table style="width:100%" summary='{t}Member selection{/t}'>
	 <tr>
	  <td style="width:48%">
	   {t}Use members from{/t}
	   <select name="target" onChange="document.mainform.submit()" size=1>
			{html_options options=$targets selected=$target}
			<option disabled>&nbsp;</option>
	   </select>
	   {if $javascript eq 'false'}<button type='submit' name='refresh'>{t}Apply{/t}</button>{/if}
    </td>
    <td>&nbsp;</td>
    <td>{t}Members{/t}</td>
	 <tr>
	  <td style="width:48%">
		{if !$acl_writeable}
	   <select style="width:100%;height:180px;" disabled name="dummy_s[]" size="20" multiple title="{t}List message possible targets{/t}">
				{html_options options=$sources}
				<option disabled>&nbsp;</option>
	   </select>
		{else}
	   <select style="width:100%;height:180px;" name="source[]" size="20" multiple title="{t}List message possible targets{/t}">
				{html_options options=$sources}
				<option disabled>&nbsp;</option>
	   </select>
		{/if}
	  </td>
	  <td>

		{if $acl_writeable}
	   <button type='submit' name='add'>&gt;</button>

	   <br><br>
	   <button type='submit' name='del'>&lt;</button>

		{/if}
	  </td>
	  <td style="width:48%">
		{if !$acl_writeable}
	   <select style="width:100%;height:180px;" disabled name="dummy_r[]" size="20" multiple title="{t}List message recipients{/t}">
				{html_options options=$recipients}
				<option disabled>&nbsp;</option>
	   </select>

		{else}
	   <select style="width:100%;height:180px;" name="recipient[]" size="20" multiple title="{t}List message recipients{/t}">
				{html_options options=$recipients}
				<option disabled>&nbsp;</option>
	   </select>
		{/if}
	  </td>
	 </tr>
	</table>

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
