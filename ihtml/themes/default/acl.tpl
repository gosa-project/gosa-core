<div class="acl-content-wrapper">
	{if !$acl_readable}
		<h2>{msgPool type=permView}</h2>
	{else}
		{if $dialogState eq 'head'}
			<h2>{t}Assigned ACL for current entry{/t}</h2>

			<div class="row">
				<div class="col s12">
					<div class="acl-ds-head card-content-scroll">
						{$aclList}
					</div>

					{if $acl_createable}
						<button class="btn-small" type='submit' name='new_acl'>{t}New ACL{/t}</button>
					{/if}
				</div>
			</div>
		{/if}

		{if $dialogState eq 'create'}
			<h2>{t}New ACL{/t}</h2>
			<div class="acl-ds-create card-content-scroll">
				<div class="row">
					<div class="col s12">
						<h3>{t}Options{/t}</h3>
						<div class="row options">
							<div class="col s12 xl6">
								<div class="input-field">
									{if !$acl_writeable}
										<select id="acl_type" size="1" name="dummy_t" title="{t}Select an ACL type{/t}" disabled>
											{html_options options=$aclTypes selected=$aclType}
										</select>
									{else}
										<select id="acl_type" size="1" name="aclType" title="{t}Select an ACL type{/t}"
											onChange="document.mainform.submit()">
											{html_options options=$aclTypes selected=$aclType}
										</select size="1">

										{if $javascript eq 'false'}
											<button class="btn-small" type='submit' name='refresh'>{t}Apply{/t}</button>
										{/if}
									{/if}
									<label for="acl_type">{t}ACL type{/t}</label>
								</div>
							</div>

							<div class="col s12 xl6">
								<div class="input-field">
									{if !$acl_writeable}
										<input id="acl_filter" type="text" value='{$aclFilter}' disabled name='dummy_f'>
									{else}
										<input id="acl_filter" type='text' value='{$aclFilter}' name='aclFilter'>
									{/if}

									<label for="acl_filter">{t}Additional filter options{/t}</label>
								</div>
							</div>
						</div>

						<div class="members">
							<h3>{t}Members{/t}</h3>
							{$aclMemberList}
							<button class="btn-small" type='submit' {if !$acl_writeable} disabled="disabled" {/if}
								name='add_user_or_group'>{msgPool type=addButton}</button>

							<button class="btn-small" type='submit' {if !$acl_writeable} disabled="disabled" {/if}
								name='add_all_users'>{t}Add all users{/t}</button>
						</div>

						{if $aclType ne 'reset'}
							{if $aclType ne 'role'}
								{if $aclType ne 'base'}
									<div class="acl-categories">
										<h3>{t}List of available ACL categories{/t}</h3>
										{$aclList}
									</div>
								{/if}
							{/if}
						{/if}

						{if $aclType eq 'base'}
							<div class="acl-for-object">
								<h3>{t}ACL for this object{/t}</h3>
								{$aclSelector}
							</div>
						{/if}

						{if $aclType eq 'role'}
							<div class="acl-roles">
								<h3>{t}Available roles{/t}</h3>
								{$roleSelector}
							</div>
						{/if}
					</div>
				</div>
			</div>

			<div class="card-action">
				{if $acl_writeable}
				<button class="btn-small primary" type='submit' name='submit_new_acl'>{t}Apply{/t}</button>
				{/if}
				<button class="btn-small primary" type='submit' name='cancel_new_acl'>{t}Cancel{/t}</button>
			</div>
		{/if}

		{if $dialogState eq 'edit'}
			<h2>{$headline}</h2>

			<div class="acl-ds-edit card-content-scroll">
				<div class="row">
					<div class="col s12">
						<div class="acl-for-object">
							{$aclSelector}
						</div>
					</div>
				</div>
			</div>

			<div class="card-action">
				<button class="btn-small primary" type='submit' name='submit_edit_acl'>{t}Apply{/t}</button>
				<button class="btn-small primary" type='submit' name='cancel_edit_acl'>{t}Cancel{/t}</button>
			</div>
		{/if}
	{/if}
</div>
