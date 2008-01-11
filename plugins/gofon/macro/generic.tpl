<table width="100%" summary="">
  <tr>
    <td>
   			<table summary="">
     			<tr>
      				<td><LABEL for="cn">{t}Macro name{/t}</LABEL>{$must}</td>
      				<td>
{render acl=$cnACL}
					<input name="cn" id="id" size=13 maxlength=13 value="{$cn}" title="{t}Macro name{/t}">
{/render}
				</td>
    			</tr>
    			<tr>
     				<td><LABEL for="displayName">{t}Display name{/t}</LABEL>{$must}</td>
     				<td>
{render acl=$displayNameACL}
        				<input name="displayName" id="displayName" size=35 maxlength=60 value="{$displayName}" 
						title="{t}Macro name to be displayed{/t}">
{/render}
     				</td>
    			</tr>
    			<tr>
					<td colspan=2>
					</td>
				</tr>
    			<tr>
      				<td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
      				<td>
					{render acl=$baseACL}
					      <select id="base" size="1" name="base" title="{t}Choose subtree to place group in{/t}">
					       {html_options options=$bases selected=$base_select}
					      </select>
					{/render}

					{render acl=$baseACL disable_picture='images/folder_gray.png'}
						<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
					{/render}

		      		</td>
	     		</tr>
   			</table>
		</td>
		<td style="border-left:1px solid #A0A0A0">
		     &nbsp;
		</td>
		<td style="vertical-align:top;">
			<table summary="">
    			<tr>
     				<td><LABEL for="description">{t}Description{/t}</LABEL></td>
     				<td>
{render acl=$descriptionACL}
      					<input name="description" id="description" size=35 maxlength=80 value="{$description}">
{/render}
     				</td>
    			</tr>
    			<tr>
     				<td colspan=2>
{render acl=$goFonMacroVisibleACL}
      					<input type="checkbox" name="goFonMacroVisible" size=35 maxlength=60 value="1" 
						{$goFonMacroVisibleChecked}> {t}Visible for user{/t}
{/render}
     				</td>
    			</tr>
			</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<br>
{t}Macro text{/t}
<br>
{render acl=$goFonMacroContentACL}
<textarea name="goFonMacroContent" style="width:99%;height:300px;font-size:14px;font-family:Arial;">{$goFonMacroContent}</textarea>
{/render}

<input type='hidden' name='gofonMacroGenericPosted' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
