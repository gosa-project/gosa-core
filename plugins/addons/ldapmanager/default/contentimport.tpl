<div class="ldap-import-wrapper">
	<div class="row">
		<span style="color:red">Temporary disabled</span>
	</div>
</div>

<!--
{if $type == FALSE || $LDIFError != FALSE}
	<h2>LDAP-Datenbank Import</h2>
	<p>{t}The LDIF import plug-in provides methods to upload a set of entries to your running LDAP directory as LDIF. You may use this to add new or modify existing entries. Remember that GOsa will not check your LDIF for GOsa conformance.{/t}</p>

	<div class="ldap-database-import">
		<div class="row">
			<div class="col s12 xl6">
				<h3>{t}Import LDIF File{/t}:</h3>

				<div class="database-import-action" id="database-import">
					<div class="file-field input-field">
						<div class="btn-small">
							<span>{t}Browse{/t}</span>
							<input type="hidden" name="ignore">
							<input type="hidden" name="MAX_FILE_SIZE" value="2097152">
							<input name="userfile" id="userfile" type="file" value="{t}Browse{/t}">
						</div>
						<div class="file-path-wrapper">
							<input type="text" class="file-path validate">
						</div>
					</div>
				</div>
			</div>

			<div class="col s12 xl6">
				<label>
					<input type="radio" name="overwrite" id="modify" value="1" checked/>
					<span>{t}Modify existing objects, keep untouched attributes{/t}</span>
				</label>

				<label>
					<input type="radio" name="overwrite" id="overwrite" value="0"/>
					<span>{t}Overwrite existing objects, all not listed attributes will be removed{/t}</span>
				</label>

				<label>
					<input type="checkbox" name="cleanup" id="cleanup" value="1"/>
					<span for="cleanup">{t}Remove existing entries first{/t}</span>
				</label>
			</div>
		</div>
	</div>

	<div class="plugin-actions card-action">
		<button class="btn-small primary" type="submit" name="fileup">{t}Import{/t}</button>
	</div>
{else}
	<h3>{t}Import successful{/t}</h3>

	<div class="plugin-actions card-action">
		<button class="btn-small primary" type="submit" name="back">{msgPool type=backButton}</button>
	</div>
{/if}

<input type="hidden" name="ignore">
-->
