<table width="100%" summary="">
  <tr>
    <td>
   			<table summary="">
     			<tr>
      				<td><LABEL for="cn">{t}Macro name{/t}</LABEL>{$must}</td>
      				<td><input {$disable_cn} name="cn" id="id" size=13 maxlength=13 value="{$cn}" {$cnACL} title="{t}Macro name{/t}"></td>
    			</tr>
    			<tr>
     				<td><LABEL for="displayName">{t}Display name{/t}</LABEL>{$must}</td>
     				<td>
        				<input {$displayNameACL} name="displayName" id="displayName" size=35 maxlength=60 value="{$displayName}" title="{t}Macro name to be displayed{/t}">
     				</td>
    			</tr>
    			<tr>
					<td colspan=2>
					</td>
				</tr>
    			<tr>
      				<td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
      				<td>
					   <select size="1" id="base" name="base" title="{t}Choose subtree to place macro in{/t}" {$baseACL}>
							{html_options options=$bases selected=$base_select}
							<option disabled>&nbsp;</option>
					   </select>
						{if $baseACL == ""}
							<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
						{else}
							<img src="images/folder_gray.png" class="center" title="{t}Select a base{/t}">
						{/if}
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
      					<input name="description" id="description" size=35 maxlength=80 value="{$description}"  {$descriptionACL}>
     				</td>
    			</tr>
    			<tr>
     				<td colspan=2>
      					<input type="checkbox" name="goFonMacroVisible" size=35 maxlength=60 value="1" {$goFonMacroVisibleACL} {$goFonMacroVisibleChecked}> {t}Visible for user{/t}
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
<textarea name="goFonMacroContent" style="width:99%;height:300px;font-size:14px;font-family:Arial;" {$goFonMacroContentACL}>{$goFonMacroContent}</textarea>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
