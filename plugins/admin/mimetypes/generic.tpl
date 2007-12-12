<table style="width:100%">
	<tr>
		<td colspan="2">
			<h2><img src='images/list_mime.png' class='center' alt='[M]'>&nbsp;{t}Generic{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%; vertical-align:top;' >
			<table>
				<tr>
					<td>
						{t}Mime type{/t}{$must}
					</td>
					<td>
{render acl=$gotoMimeGroupACL}
						<input type="text" name='cn' value="{$cn}" title='{t}Please enter a name for the mime type here{/t}'>
{/render}
					</td>
				</tr>
				<tr>
					<td>
						{t}Mime group{/t}
					</td>
					<td>
{render acl=$gotoMimeGroupACL}
						<select name='gotoMimeGroup' title='{t}Categorize this mime type{/t}'>
							{html_options output=$gotoMimeGroups values=$gotoMimeGroups selected=$gotoMimeGroup}
						</select>
{/render}
					</td>
				</tr>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
{render acl=$descriptionACL}
						<input type="text" name='description' value="{$description}" title='{t}Please specify a description{/t}'>
{/render}
					</td>
				</tr>
{if !$isReleaseMimeType} 
				<tr>
				  	<td><LABEL for="base">{t}Base{/t}{$must}</LABEL></td>
				  	<td>
	{render acl=$baseACL}
		<select size="1" id="base" name="base" title="{t}Choose subtree to place application in{/t}">
			{html_options options=$bases selected=$base_select}
		</select>
	{/render}
	{if !$isReleaseMimeType}
		{render acl=$baseACL disable_picture='images/folder_gray.png'}
			<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
		{/render}
	{/if}
				  	</td>
				 </tr>

{/if}
			</table>
			
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			<table summary="">
				<tr>
					<td>
						<LABEL for="picture_file">{t}Icon{/t}</LABEL><br>
						<img src="{$gotoMimeIcon}" border=1 alt='{t}Mime icon{/t}' 
							style="width:48px; height:48; background-color:white; vertical-align:bottom;">
					</td>
					<td style="vertical-align:top">
						&nbsp;<br>
						<input type="hidden" name="MAX_FILE_SIZE" value="100000">
{render acl=$gotoMimeIconACL}
						<input name="picture_file" type="file" size="20" maxlength="255" 
							accept="image/*.png" id="picture_file">
{/render}
{render acl=$gotoMimeIconACL}
						<input type="submit" name="update_icon" value="{t}Update{/t}" 
							title="{t}Update mime type icon{/t}">
{/render}
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<h2><img src='images/mime_leftclick.png' class='center' alt='{t}Left click{/t}'>&nbsp;{t}Left click{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%; vertical-align:top;' >
			{t}File patterns{/t}{$must}
{render acl=$gotoMimeFilePatternACL}	
			{$gotoMimeFilePatterns}	
{/render}

{render acl=$gotoMimeFilePatternACL}	
			<input type='text'	 name='NewFilePattern'	  value='' title='{t}Please specify a new file pattern{/t}'>
{/render}
{render acl=$gotoMimeFilePatternACL}	
			<input type='submit' name='AddNewFilePattern' value='{t}Add{/t}' title='{t}Add a new file pattern{/t}'>
{/render}
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			{t}Applications{/t}
{render acl=$gotoMimeApplicationACL}	
			{$gotoMimeApplications}		
{/render}

{render acl=$gotoMimeApplicationACL}	
			<select name="NewApplicationSelect">
				<option value="">-</option>
				{html_options options=$ApplicationList}
			</select>
{/render}
{render acl=$gotoMimeApplicationACL}	
			<input type='text'	 name='NewApplication'	  value='' title='{t}Enter an application name here{/t}'>
{/render}
{render acl=$gotoMimeApplicationACL}	
			<input type='submit' name='AddNewApplication' value='{t}Add{/t}' title='{t}Add application{/t}'>
{/render}
		</td>
	</tr>
	<tr>	
		<td colspan="2">
			<p class='seperator'>&nbsp;</p>
		</td>
	</tr>
	<tr>	
		<td colspan="2">
			<h2><img src='images/mime_embedding.png' class='center' alt='{t}Embedding{/t}'>&nbsp;{t}Embedding{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%; vertical-align:top;' >
				
			<table>
				<tr>
					<td style='vertical-align:top; '>
{render acl=$gotoMimeLeftClickActionACL}
						<input type='radio' name='gotoMimeLeftClickAction_IE' value='I' 
							{if $gotoMimeLeftClickAction_I} checked {/if}>
{/render}
						{t}Show file in embedded viewer{/t}
						<br>

{render acl=$gotoMimeLeftClickActionACL}
						<input type='radio' name='gotoMimeLeftClickAction_IE' value='E' 
							{if $gotoMimeLeftClickAction_E} checked {/if}>
{/render}
						{t}Show file in external viewer{/t}
						<br>

{render acl=$gotoMimeLeftClickActionACL}
						<input type='checkbox' name='gotoMimeLeftClickAction_Q' value='1' 
							{if $gotoMimeLeftClickAction_Q} checked {/if}>
{/render}
						{t}Ask whether to save to local disk{/t}
					</td>
				</tr>
			</table>
			
		</td>
		<td style="border-left:1px solid #A0A0A0;vertical-align:top;">
			{t}Applications{/t}
{render acl=$gotoMimeEmbeddedApplicationACL}
			{$gotoMimeEmbeddedApplications}		
{/render}
{render acl=$gotoMimeEmbeddedApplicationACL}
			<select name="NewEmbeddedApplicationSelect">
				<option value="">-</option>
				{html_options options=$ApplicationList}
			</select>
{/render}
{render acl=$gotoMimeEmbeddedApplicationACL}
			<input type='text'	 name='NewEmbeddedApplication'	  value='' 
				title='{t}Enter an application name here{/t}'>
{/render}
{render acl=$gotoMimeEmbeddedApplicationACL}
			<input type='submit' name='AddNewEmbeddedApplication' value='{t}Add{/t}' 
				title='{t}Add application{/t}'>
{/render}
		</td>
	</tr>
</table>
<input type="hidden" name="MimeGeneric" value="1">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
