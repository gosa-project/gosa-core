{if $type == FALSE || $LDIFError != FALSE}
<p style="margin-top:5px;">
  {t}The LDIF import plugin provides methods to upload a set of entries to your running LDAP directory as ldif. You may use this to add new or modify existing entries. Remember that GOsa will not check your ldifs for GOsa conformance.{/t}
</p>

<p class="seperator">&nbsp;</p>
<table summary="">
<tr>
    <td width="30%">
		{t}Import LDIF File{/t}
    </td>
    <td>
			<input type="hidden" name="ignore">
			<input type="hidden" name="MAX_FILE_SIZE" value="2097152">
			<input name="userfile" type="file" value="{t}Browse{/t}">
    </td>
</tr>
<tr>
	<td>
		&nbsp;
	</td>
	<td>
        <input type="checkbox" name="overwrite" value="1">
		{t}Modify existing attributes{/t}
	</td>
</tr>
<tr>
   	<td>
		&nbsp;
   	</td>
   	<td>
        <input type="checkbox" name="cleanup" value="1">
		{t}Overwrite existing entry{/t}
	</td>
</tr>
</table>
{else}

<br>
    <h1>{t}Import successful{/t}</h1>
<br>

<div align="right">
        <a href="">{t}Back{/t}</a>&nbsp;
</div>
		
{/if}
<p class="plugbottom">
<input type="submit" value="{t}Import{/t}" name="fileup" >
</p>
<input type="hidden" name="ignore">
