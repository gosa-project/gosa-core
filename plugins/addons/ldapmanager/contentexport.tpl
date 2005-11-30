{if $LDIFError != FALSE || $type == FALSE}
<p style="margin-top:5px;">
  {t}The LDIF export plugin provides methods to download a complete snapshot of the running LDAP directory as ldif. You may save these files for backup purpose or when initializing a new server.{/t}
</p>
<p class="seperator">&nbsp;</p>

<table summary="" style="width:100%;">
<tr>
	<td width="30%">
		<LABEL for="text" >{t}Export single entry{/t}</LABEL>
	</td>
	<td>
		<input id="text" type="text" value="" name="single">
	</td>
	<td>
		<input type="submit" value="{t}Export{/t}" name="sfrmgetsingle">
	</td>
</tr>
<tr>
	<td width="30%">
		<LABEL for="selfull">{t}Export complete LDIF for{/t}</LABEL>
	</td>
	<td>
		<select id="selfull" name="selfull" size="1" title="{t}Choose the department you want to Export{/t}">
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
		<LABEL for="selivbb">{t}Export IVBB LDIF for{/t}</LABEL>
	</td>
	<td>
		<select id="selivbb" name="selivbb" size="1" title="{t}Choose the department you want to Export{/t}">
			{html_options options=$deplist selected=$depselectivbb}
		</select>
	</td>
	<td>
		<input type="submit" value="{t}Export{/t}" name="sfrmgetivbb">
	</td>
</tr>
{/if}
</table> 


{else}


<br>
	<h1>{t}Export successful{/t}</h1>
<br>
	<div>
		<img alt="" border="0" align=middle src="images/filesaveas.png">
		<a href = "getldif.php{$para}" title="{t}Click here to save the complete LDAP database to a file{/t} ">{t}Click here to save the LDAP Export {/t}</a>


		<br>
		<br>
	</div>
	<div align="right">
		<input type="submit" value="{t}Back{/t}" name='back'>
	</div>
{/if}
<p class="plugbottom">
  &nbsp;
</p>

<input type="hidden" name="ignore">
