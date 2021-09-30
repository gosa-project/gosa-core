<div class="ldapxls-export-wrapper">
	<div class="row">
		{if $LDIFError != FALSE || $type == FALSE}
		<h2>
		  {t}The XLS export plug-in provides methods to download a complete snapshot of the running LDAP directory as XLS. You may save these files for documentation.{/t}
		</h2>
		<hr class="divider">

		<div class="col s12 xl6">
			<div class="input-field add">
				{*<input id="text" type="text" value="" name="single">*}
				<select id="single" name="single" size="1" title="{t}Choose the data you want to export{/t}">
					{html_options options=$choicelist selected=0}
				</select>
				<label for="single" >{t}Export single entry{/t}</label>
				<button class="btn-small" type='submit' name='sfrmgetsingle'>{t}Export{/t}</button>
			</div>

			<p>{t}Export complete XLS for{/t}</p>
			<div class="valign-wrapper">
				<div class="input-field ldap-tree" style="flex-grow: 1;margin-right: 20px">
					{$base}
				</div>
				<button class="btn-small" type='submit' name='sfrmgetfull'>{t}Export{/t}</button>
			</div>
			
			{if $govmode ne "true"}
			<div class="input-field add">
				<select id="selivbb" name="selivbb" size="1" title="{t}Choose the department you want to export{/t}">
					{html_options options=$deplist selected=$depselectivbb}
				</select>
				<label for="selivbb">{t}Export IVBB LDIF for{/t}</label>
				<button class="btn-small" type='submit' name='sfrmgetivbb'>{t}Export{/t}</button>
			</div>
			{/if}
		</div>

		{else}

		<h2>{t}Export successful{/t}</h2>
		<hr class="divider">
		<div class="valign-wrapper">
			<a href="plugins/ldapmanager/getxls.php{$para}" title="{t}Click here to save the complete LDAP database to a XLS file{/t} ">
				{image path="<i class='material-icons'>save</i>"}
			</a>
			{t}Click here to save the LDAP export to a XLS file{/t}
		</div>
		<div class="card-action">
			<button class="btn-small" type='submit' name='back'>{msgPool type=backButton}</button>
		</div>
		{/if}
	</div>
</div>

<input type="hidden" name="ignore">
