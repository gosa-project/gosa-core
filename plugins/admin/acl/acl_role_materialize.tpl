{if $dialogState eq 'head'}

<h5>{t}Assigned ACL for current entry{/t}</h5>

<div class="row">
	<div class="col s6">
		<div class="input-field">
			{render acl=$cnACL}
			<input type="text" name='cn' value="{$cn}" id="cn">
			{/render}
			<label for="cn">{t}Name{/t}</label>
		</div>

		<div class="input-field">
			{render acl=$descriptionACL}
			<input type="text" name='description' value="{$description}" id="description">
			{/render}
			<label for="description">{t}Description{/t}</label>
		</div>

		{render acl=$baseACL}
		<div class="input-field col s3">
			{$base}
		</div>
		{/render}
	</div>
</div>

{$aclList}

{render acl=$gosaAclTemplateACL}
<button class="btn-small gonicus-color" type='submit' name='new_acl'>{t}New ACL{/t}</button>
{/render}

{/if}

{if $dialogState eq 'create'}

<div class="row">
	<div class="col s1">
		<h5>{t}ACL type{/t}</h5>
	</div>
	<div class="col s3">
		<div class="input-field">
			<select size="1" name="aclType" title="{t}Select an ACL type{/t}" onChange="document.mainform.submit()">
				{html_options options=$aclTypes selected=$aclType}
				<option disabled>&nbsp;</option>
			</select>
		</div>
	</div>
</div>

{if $javascript eq 'false'}

<button type='submit' name='refresh'>{msgPool type=applyButton}</button>

{/if}

<h5>{t}List of available ACL categories{/t}</h5>
{$aclList}

<div class="card-action">
	{render acl=$gosaAclTemplateACL}
	<button class="btn-small gonicus-color" type='submit' name='submit_new_acl'>{msgPool type=applyButton}</button>
	{/render}

	<button class="btn-small gonicus-color" type='submit' name='cancel_new_acl'>{msgPool type=cancelButton}</button>
</div>
{/if}

{if $dialogState eq 'edit'}

<h5>{$headline}</h5>

{render acl=$gosaAclTemplateACL}
{$aclSelector}
{/render}

<hr>
<div class="card-action">
	{render acl=$gosaAclTemplateACL}
	<button class="btn-small gonicus-color" type='submit' name='submit_edit_acl'>{msgPool type=applyButton}</button>
	{/render}
	<button class="btn-small gonicus-color" type='submit' name='cancel_edit_acl'>{msgPool type=cancelButton}</button>
</div>
{/if}

<script type="text/javascript">
	document.addEventListener('DOMContentLoaded', function() {
		var elems = document.querySelectorAll('select');
		var instances = M.FormSelect.init(elems, {});
	  });
</script>

<input type='hidden' name='acl_role_posted' value='1'>