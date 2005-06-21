<table width="100%">
  <tr>
    <td>
   			<table>
     			<tr>
      				<td>{t}Macro name{/t}{$must}</td>
      				<td><input name="cn" size=25 maxlength=60 value="{$cn}" title="{t}Macro name{/t}"></td>
    			</tr>
    			<tr>
     				<td>{t}Display name{/t}</td>
     				<td>
        				<input name="displayName" size=35 maxlength=60 value="{$displayName}" title="{t}Macro name to be displayed{/t}">
     				</td>
    			</tr>
    			<tr>
					<td colspan=2><div style="height:15px;">
					</td>
				</tr>
    			<tr>
      				<td>{t}Base{/t}{$must}</td>
      				<td>
					   <select size="1" name="base" title="{t}Choose subtree to place application in{/t}">
							{html_options options=$bases selected=$base_select}
					   </select>
		      		</td>
	     		</tr>
   			</table>
		</td>
		<td style="vertical-align:top;">
			<table>
    			<tr>
     				<td>{t}Description{/t}</td>
     				<td>
      					<input name="description" size=35 maxlength=60 value="{$description}">
     				</td>
    			</tr>
    			<tr>
     				<td colspan=2>
      					<input type="checkbox" name="goFonMacroVisible" size=35 maxlength=60 value="1" {$goFonMacroVisibleChecked}> {t}Visible for user{/t}
     				</td>
    			</tr>
			</table>
		</td>
	</tr>
</table>
<br>
{t}Macro text{/t}
<br>
<textarea name="goFonMacroContent" style="width:99%;height:300px;font-size:14px;font-family:Arial;">{$goFonMacroContent}</textarea>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
