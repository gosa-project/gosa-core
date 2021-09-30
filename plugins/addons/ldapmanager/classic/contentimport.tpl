<font color='red'>Temporary disabled</font>
<!--





{if $type == FALSE || $LDIFError != FALSE}
<p style="margin-top:5px;">
  {t}The LDIF import plug-in provides methods to upload a set of entries to your running LDAP directory as LDIF. You may use this to add new or modify existing entries. Remember that GOsa will not check your LDIF for GOsa conformance.{/t}
</p>

<hr>
<table summary="" width="100%">
<tr>
    <td width="180">
		<LABEL for="userfile">{t}Import LDIF File{/t}</LABEL>
    </td>
    <td>
			<input type="hidden" name="ignore">
			<input type="hidden" name="MAX_FILE_SIZE" value="2097152">
			<input name="userfile" id="userfile" type="file" value="{t}Browse{/t}">
    </td>
</tr>
<tr>
	<td>
		&nbsp;
	</td>
	<td>
    <input type="checkbox" name="overwrite" value="1" id="overwrite"> - - >
		<input type="radio" name="overwrite" value="1" checked>{t}Modify existing objects, keep untouched attributes{/t}<br>
		<input type="radio" name="overwrite" value="0">{t}Overwrite existing objects, all not listed attributes will be removed{/t}
	</td>
</tr>
<tr>
   	<td>
		&nbsp;
   	</td>
   	<td>
        <input type="checkbox" name="cleanup" value="1" id="cleanup">
		<LABEL for="cleanup">{t}Remove existing entries first{/t}</LABEL>
	</td>
</tr>
</table>
{else}

<br>
    <h3>{t}Import successful{/t}</h3>
<br>

<div align="right">
		<button type='submit' name='back'>{msgPool type=backButton}</button>

</div>
		
{/if}

<hr>
<div class="plugin-actions">
  <button type='submit' name='fileup'>{t}Import{/t}</button>

</div>

<input type="hidden" name="ignore">
-->
