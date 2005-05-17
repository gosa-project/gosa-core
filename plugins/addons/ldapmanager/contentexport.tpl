{if $LDIFError != FALSE || $type == FALSE}
<p style="margin-top:5px;">
  {t}The LDIF export plugin provides methods to download a complete snapshot of the running LDAP directory as ldif. You may save these files for backup purpose or when initializing a new server.{/t}
</p>
<p class="seperator">&nbsp;</p>

<table style="width:100%;">
<div>

<table>
<tr>
	<td width="30%">
		{t}Export single entry{/t}
	</td>
	<td>
		<input type="text" value="" name="single">
	</td>
	<td>
		<input type="submit" value="{t}Export{/t}" name="sfrmgetsingle">
	</td>
</tr>
<tr>
	<td width="30%">
		{t}Export complete LDIF for{/t}
	</td>
	<td>
		<select name="selfull" size="1" title="{t}Choose the department you want to Export{/t}">
			{html_options options=$deplist selected=0}
		</select>
	</td>
	<td>
		<input type="submit" value="{t}Export{/t}" name="sfrmgetfull">
	</td>
</tr>
	
{if $govmode eq "true"}
<tr>
	<td width="30%">
		{t}Export IVBB LDIF for{/t}
	</td>
	<td>
		<select name="selivbb" size="1" title="{t}Choose the department you want to Export{/t}">
			{html_options options=$deplist selected=$depselectivbb}
		</select>
	</td>
	<td>
		<input type="submit" value="{t}Export{/t}" name="sfrmgetivbb">
	</td>
</tr>
{/if}
</table> 
</div>


{else}


<br>
	<h1>{t}Export successful{/t}</h1>
<br>
	<div>
		<img border="0" align=center src="images/filesaveas.png">
		<a href = "getldif.php{$para}" title="{t}Click here to save the complete LDAP database to a file{/t} ">{t}Click here to save the LDAP Export {/t}</a>


		<br>
		<br>
	</div>
	<div align="right">
		<a href="">{t}Back{/t}</a>&nbsp;
	</div>
{/if}
<p class="plugbottom">
  &nbsp;
</p>

<input type="hidden" name="ignore">
