<div class="system-wrapper">
	<div class="row">
		<div class="col s12 xl6">
          	<h3>{t}Generic settings{/t}</h3>

			{render acl=$cnACL}
				<div class="input-field">
					<input type="text" name="cn" id="cn" value="{$cn}">
					<label for="cn">{t}Device name{/t}{$must}</label>
				</div>
			{/render}

			{render acl=$descriptionACL}
				<div class="input-field">
					<input type="text" name="description" id="description" value="{$description}" maxlength=80>
					<label for="description">{t}Description{/t}</label>
				</div>
			{/render}

			{render acl=$baseACL}
					<div class="input-field ldap-tree">
						<label for="base">{t}Base{/t}{$must}</label>
						{$base}
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
