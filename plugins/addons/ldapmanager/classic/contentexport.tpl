<p style="margin-top:5px;">
  {t}The LDIF export plug-in provides methods to download a complete snapshot of the running LDAP directory as LDIF. You may save these files for backup purpose or when initializing a new server.{/t}
</p>
<hr>

<table summary="" style="width:100%;">
<tr>
	<td width="30%">
		<LABEL for="text" >{t}Export single entry{/t}</LABEL>
	</td>
	<td>
		<input id="text" type="text" value="{$single}" name="single">
	</td>
	<td>
		<button type='submit' name='sfrmgetsingle'>{t}Export{/t}</button>

	</td>
</tr>
<tr>
	<td width="30%">
		<LABEL for="selfull">{t}Export complete LDIF for{/t}</LABEL>
	</td>
	<td>
        {$base}
	</td>
	<td>
		<button type='submit' name='sfrmgetfull'>{t}Export{/t}</button>

	</td>
</tr>
</table> 

<hr>

<input type="hidden" name="ignore">
