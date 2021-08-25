<div class="acl-content-wrapper">
	{if $dialogState eq 'head'}
		<h2>{t}Assigned ACL for current entry{/t}</h2>

		<div class="acl-ds-head card-content-scroll">
			<div class="row">
				<div class="col s6">
					<div class="input-field">
						{render acl=$cnACL}
							<input type="text" name='cn' value="{$cn}" id="cn">
						{/render}

						<label for="cn">{t}Name{/t}</label>
					</div>
				</div>

				<div class="col s6">
					<div class="input-field">
						{render acl=$descriptionACL}
							<input type="text" name='description' value="{$description}" id="description">
						{/render}

						<label for="description">{t}Description{/t}</label>
					</div>
				</div>

				<div class="col s6">
					{render acl=$baseACL}
						<div class="input-field ldap-tree">
							{$base}
						</div>
					{/render}
				</div>
			</div>

			{$aclList}
		</div>

		{render acl=$gosaAclTemplateACL}
			<button class="btn-small" type='submit' name='new_acl'>{t}New ACL{/t}</button>
		{/render}

	{/if}

	{if $dialogState eq 'create'}

		<div class="row">
			<div class="col s1">
				<h2>{t}ACL type{/t}</h2>
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

		<div class="acl-ds-create card-content-scroll">
			{if $javascript eq 'false'}
				<button type='submit' name='refresh'>{msgPool type=applyButton}</button>
			{/if}

			<h3>{t}List of available ACL categories{/t}</h3>

			{$aclList}
		</div>

		<div class="card-action">
			{render acl=$gosaAclTemplateACL}
				<button class="btn-small gonicus-color" type='submit' name='submit_new_acl'>{msgPool type=applyButton}</button>
			{/render}

			<button class="btn-small gonicus-color" type='submit' name='cancel_new_acl'>{msgPool type=cancelButton}</button>
		</div>
	{/if}

	{if $dialogState eq 'edit'}

		<h2>{$headline}</h2>

		<div class="acl-ds-edit card-content-scroll">
			{render acl=$gosaAclTemplateACL}
				{$aclSelector}
			{/render}
		</div>

		<div class="card-action">
			{render acl=$gosaAclTemplateACL}
			<button class="btn-small gonicus-color" type='submit' name='submit_edit_acl'>{msgPool type=applyButton}</button>
			{/render}
			<button class="btn-small gonicus-color" type='submit' name='cancel_edit_acl'>{msgPool type=cancelButton}</button>
		</div>
	{/if}
	<input type='hidden' name='acl_role_posted' value='1'>
</div>
