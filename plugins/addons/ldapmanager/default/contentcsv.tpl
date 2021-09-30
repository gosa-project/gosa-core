<div class="ldap-import-wrapper">
	<div class="row">
		<h2>
		  {t}The CSV import plug-in provides methods to generate user accounts from a file containing Comma Separated Values. The administrator can decide which columns should be transfered to which attribute. Note that you must have at least the UID, GIVENNAME and SURNAME set.{/t}
		</h2>
		<hr class="divider">
		
		{if $fileup != TRUE}

		<div class="col s12 xl6">
			<div class="file-field input-field">
				<input type="hidden" name="MAX_FILE_SIZE" value="2097152">
				<div class="btn-small">
				  <span>{t}Browse{/t}</span>
				  <input id="userfile" name="userfile" type="file">
				</div>
				<div class="file-path-wrapper">
				  <input class="file-path validate" type="text" placeholder="{t}Select CSV file to import{/t}">
				</div>
			</div>

			<div class="input-field">
				<select id="template" name="template" size="1" title="">
					{html_options options=$templates selected=""}	
				</select>
				<label for="template">{t}Select template{/t}</label>
			</div>
		</div>

		{elseif $sorted != FALSE}
		<div class="col s12 xl12">
			{if $error == FALSE}
			<b>{t}All entries have been written to the LDAP database successfully.{/t}</b>
			{else}
			<b style="color:red">{t}There was an error during the import of your data.{/t}</b>
			{/if}
			
			<b>{t}Here is the status report for the import:{/t} </b>
			<table summary="{t}Status report{/t}" cellspacing="1" border=0 cellpadding="4"  bgcolor="#FEFEFE">
				<tr>
					{foreach from=$head item=h}
					<td bgcolor="#BBBBBB">
						<b>{$h}</b>
					</td>
					{/foreach}
				</tr>
				{if $pointsbefore == TRUE}
				<tr>
					<td colspan={$i} bgcolor = "#EEEEEE">
						...	
					</td>
				</tr>
				{/if}
				{foreach from=$data item=row key=key}	
				<tr>
					{foreach from=$data[$key] item=col key=key2}
					<td bgcolor="#EEEEEE">
						{$data[$key][$key2]}
					</td>
					{/foreach}
				</tr>
				{/foreach}
				{if $pointsafter == TRUE}
				<tr>
					<td colspan={$i} bgcolor = "#EEEEEE">
						...
					</td>
				</tr>
				{/if}
			</table>
		</div>
		{else}

		<div class="col s12 xl6">
			<br><b>{t}Selected Template{/t}:</b> {$tpl}
			<br>
			<br>
			<table summary="{t}Template selection{/t}" cellspacing="1" border=0 cellpadding="4" bgcolor="#FEFEFE">
				<tr>
					{foreach from=$data[0] item=item key=key}
					<td bgcolor="#BBBBBB">
						<select name="row{$key}" size="1" title="">
							{html_options options=$attrs selected=$selectedattrs[$key]}
						</select>
					</td>
					{/foreach}
				</tr>
				{foreach from=$data item=val key=key}
				<tr>
					{foreach from=$data[$key] item=val2 key=key2}
					<td bgcolor="#EEEEEE">
						{$data[$key][$key2]}&nbsp;
					</td>
					{/foreach}
				</tr>
				{/foreach}
				
			</table>
			< ! - - {html_table loop=$data cols=$anz table_attr='border="1"'}- - >
		</div>
		{/if}
	</div>
</div>


<div class="card-action">
  {if $fileup != TRUE}
    <button class="btn-small" type='submit' name='fileup'>{t}Import{/t}</button>
  {else}
    {if $sorted == FALSE}
      <input name="sorted" value="{t}Import{/t}" type ="submit">
    {else}
      <button class="btn-small" type='submit' name='back{$plug}'>{msgPool type=backButton}</button>
    {/if}
  {/if}
</div>

<input type="hidden" name="ignore">
