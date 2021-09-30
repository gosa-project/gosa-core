<div class="ldap-export-wrapper">
	<div class="row">
		<h2>
			{t}The LDIF export plug-in provides methods to download a complete snapshot of the running LDAP directory as LDIF. You may save these files for backup purpose or when initializing a new server.{/t}
		</h2>
		
		<hr class="divider">

		<div class="col s12 xl6">
			<div class="input-field add">
				<input id="text" type="text" value="{$single}" name="single">
				<label for="text" >{t}Export single entry{/t}</label>
				<button class="btn-small" type='submit' name='sfrmgetsingle'>{t}Export{/t}</button>
			</div>

			<p>{t}Export complete LDIF for{/t}</p>
			<div class="valign-wrapper">
				<div class="input-field ldap-tree" style="flex-grow: 1;margin-right: 20px">
					{$base}
				</div>
				<button class="btn-small" type='submit' name='sfrmgetfull'>{t}Export{/t}</button>
			</div>
		</div>
	</div>
</div>

<input type="hidden" name="ignore">
