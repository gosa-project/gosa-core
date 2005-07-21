<p>
  {t}The CSV import plugin provides methods to generate user accounts from a file containing Comma Seperated Values. The administrator can decide which columns should be transfered to which attribute. Note that you must have at least the UID, GIVENNAME and SURENAME set.{/t}
</p>
<p class="seperator">&nbsp;</p>

{if $fileup != TRUE}
<table summary="">
	<tr>
		<td>
			<LABEL for="userfile">{t}Select CSV file to import{/t}<LABEL>
		</td>
		<td>
		<input type="hidden" name="MAX_FILE_SIZE" value="2097152">
		<input id="userfile" name="userfile" type="file" value="{t}Browse{/t}">
		</td>
	</tr>
	<tr>
		<td>
		<LABEL for="template">{t}Select template{/t}</LABEL>
		</td>
		<td>
		<select id="template" name="template" size="1" title="">
			{html_options options=$templates selected=""}	
		</select>
		</td>
		
	</tr>
</table>
{elseif $sorted != FALSE}


<br>
    {if $error == FALSE}
    	 <b>{t}All entries have been written to the LDAP database successfully.{/t}</b>
    {else}
    	 <b style="color:red">{t}Oups. There was an error during the import of your data.{/t}</b>
	{/if}

<b>{t}Here is the status report for the import:{/t}</b>
<br>
<br>


	<table summary="" cellspacing="1" border=0 cellpadding="4"  bgcolor="#FEFEFE">
		<tr>
			{section name=head loop=$head}
			<td bgcolor="#BBBBBB">
				<b>{$head[$i]}</b>
			</td>
			<!--{$i++}-->
			{/section}
		</tr>
		{if $pointsbefore == TRUE}
		<tr>
			<td colspan={$i} bgcolor = "#EEEEEE">
				...	
			</td>
		</tr>
		{/if}
		
		
		
		
		{section name=row loop=$rows}
		
		<tr>
			{section name=coll loop=$cols}
			<td bgcolor="#EEEEEE">
				{$data[$ie]}
			</td>
			<!--{$ie++}-->
			{/section}
			
		</tr>
		{/section}
	    {if $pointsafter == TRUE}
	    <tr>
	        <td colspan={$i} bgcolor = "#EEEEEE">
	        ...
	        </td>
	    </tr>
	    {/if}
																		   
	</table>

{else}
<br><b>{t}Selected Template{/t}:</b> {$tpl}
<br>
<br>
	<table summary="" cellspacing="1" border=0 cellpadding="4" bgcolor="#FEFEFE">
		<tr>
			{section name=attr loop=$anz}
			<td bgcolor="#BBBBBB">
				<select name="row{$ia++}" size="1" title="">
		    		 {html_options options=$attrs selected=$selectedattrs[$ia]}
				</select>
			</td>
			{/section}
		</tr>
		{section name=row loop=$rows}
		<tr>
			{section name=coll loop=$anz}
			<td bgcolor="#EEEEEE">
				{$data[$i]}&nbsp;
			</td>
			<!--{$i++}-->
			{/section}
		</tr>
		{/section}
		
	</table>

<!-- {html_table loop=$data cols=$anz table_attr='border="1"'}-->
{/if}
<p class="plugbottom">
{if $fileup != TRUE}
  <input type="submit" value="{t}Import{/t}" name="fileup" >
{else}
{if $sorted == FALSE}
  <input name="sorted" value="{t}Import{/t}" type ="submit">
{else}
  <input type=submit name="back{$plug}" value="{t}Back{/t}">
{/if}
{/if}
</p>
<input type="hidden" name="ignore">
