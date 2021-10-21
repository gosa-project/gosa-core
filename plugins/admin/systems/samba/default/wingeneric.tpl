<div class="win-generic-wrapper">
    <div class="row">
        <div class="col s12 xl6">
			{render acl=$cnACL}
				<div class="input-field">
					<input type='text' id="cn" name="cn" maxlength=60 value="{$cn}">
					<label for="cn">{t}Machine name{/t}{$must}</label>
				</div>
			{/render}

			{render acl=$baseACL}
				<div class="input-field ldap-tree">
					<label for="base">{t}Base{/t}{$must}</label>
					{$base}
				</div>
			{/render}

			{render acl=$descriptionACL}
				<div class="input-field">
					<input type='text' id="description" name="description" maxlength=80 value="{$description}">
					<label for="description">{t}Description{/t}</label>
				</div>
			{/render}
		</div>

		{if $netconfig}
			<div class="col s12 xl6">
				{$netconfig}
			</div>
		{/if}
	</div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  	// First input field on page
	focus_field('cn');
</script>
