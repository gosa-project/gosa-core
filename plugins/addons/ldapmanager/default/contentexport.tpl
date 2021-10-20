<div class="ldap-export-wrapper">
	<h2>{t}Download snapshot of LDAP-Database{/t}</h2>
	<p>
		{t}The LDIF export plug-in provides methods to download a complete snapshot of the running LDAP directory as LDIF. You may save these files for backup purpose or when initializing a new server.{/t}
	</p>

	<div class="row">
		<div class="col s12 xl6">
			<div class="input-field add">
				<input id="text" type="text" value="{$single}" name="single">
				<label for="text" >{t}Export single entry{/t}</label>
				<button class="btn-small" type="submit" name="sfrmgetsingle">{t}Export{/t}</button>
			</div>

			<div class="ldap-tree-inline">
				<div class="input-field ldap-tree">
					<label>{t}Export complete LDIF for{/t}</label>
					{$base}
				</div>
				<button class="btn-small" type="submit" name="sfrmgetfull">{t}Export{/t}</button>
			</div>
		</div>
	</div>
</div>

<input type="hidden" name="ignore">
