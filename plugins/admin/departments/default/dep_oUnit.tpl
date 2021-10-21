<div class="directory-structure-wrapper">
	<h2>{t}Properties{/t}</h2>

	<div class="row">
		{render acl=${$namingAttr}ACL}
		<div class="col s6">
			<div class="input-field">
				<input type="text" id="{$namingAttr}" name="{$namingAttr}" value="{$dc}" title="{t}Name of {$trString[0]} to create{/t}">
				<label for="{$namingAttr}">{t}{$trString[1]} name{/t}{$must}</label>
			</div>
		</div>
		{/render}

		{render acl=$descriptionACL}
		<div class="col s6">
			<div class="input-field">
				<input type="text" id="description" name="description" value="{$description}" title="{t}Descriptive text for department{/t}">
				<label for="description">{t}Description{/t}{$must}</label>
			</div>
		</div>
		{/render}

		{if !$is_root_dse}
		{render acl=$baseACL}
		<div class="col s6">
			<div class="input-field ldap-tree">
				{$base}
			</div>
		</div>
		{/render}
		{/if}

		{if $manager_enabled}
		{render acl=$managerACL}
		<div class="col s6">
			<div class="input-field manager">
				<input type="text" name="manager_name" id="manager_name" value="{$manager_name}" title="{$manager}" disabled>
				<label for="manager">{t}Manager{/t}</label>

				<div class="icons">
					<div class="icon">
						{image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
					</div>

					{if $manager!=""}
					<div class="icon">
						{image path="<i class='material-icons input-icons tooltipped' data-postion='bottom' data-tooltip='{$manager}'>info_outline</i>" title="{$manager}"}
					</div>

					<div class="icon">
						{image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
					</div>
					{else}
					<div class="icon">
						{image path="<i class='material-icons md-disabled'>delete</i>"}
					</div>

					<div class="icon">
						{image path="<i class='material-icons tooltipped md-disabled context' data-postion='bottom' data-tooltip='{t}Manager{/t}'>info_outline</i>"}
					</div>
					{/if}
				</div>
			</div>
		</div>
		{/render}
		{/if}
	</div>

	<hr class="divider">

    <div class="row">
		<h3>{t}Administrative settings{/t}</h3>

        <div class="col s6">
			{render acl=$gosaUnitTagACL}
			<label>
				<p>
					<input id="is_administrational_unit" type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
					<span>{t}Tag department as an independent administrative unit{/t}</span>
				</p>
			</label>
			{/render}
        </div>
    </div>

	<input type="hidden" name="dep_generic_posted" value="1">
</div>
