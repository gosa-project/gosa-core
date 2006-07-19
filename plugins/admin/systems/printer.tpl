
<table summary="" width="100%">
	<tr>
		<td style="vertical-align:top; width:50%; border-right:1px solid #B0B0B0">
{if $StandAlone }
			<h2>{t}General{/t}</h2>
			<table summary="">
			 <tr>
			  <td><LABEL for="cn" >{t}Printer name{/t}</LABEL>{$must}</td>
			  <td><input name="cn" id="cn" size=20 maxlength=60 value="{$cn}" {$cnACL}></td>
			 </tr>
			 <tr>
			  <td><LABEL for="description">{t}Description{/t}</LABEL></td>
			  <td><input id="description" name="description" size=25 maxlength=80 value="{$description}"></td>
			 </tr>
			 <tr>
				  <td colspan=2>&nbsp;</td>
			 </tr>
			 <tr>
			  <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
			  <td>
			   <select size="1" name="base" title="{t}Choose subtree to place terminal in{/t}" {$baseACL}>
				{html_options options=$bases selected=$base_select}
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
		<td>
{/if}
			<h2>{t}Details{/t}</h2>
			<table summary="">
			 	<tr>
			  		<td><LABEL for="l">{t}Printer location{/t}</LABEL></td>
			  		<td><input id="l" name="l" size=25 maxlength=80 value="{$l}"></td>
			 	</tr>
			 	<tr>
			  		<td><LABEL for="labeledURI">{t}Printer URL{/t}</LABEL>{$must}</td>
			  		<td><input id="labeledURI" name="labeledURI" size=25 maxlength=80 value="{$labeledURI}"></td>
			 	</tr>
			</table>
			<table summary="">
				<tr>	
					<td>
						<br>
						{t}Driver{/t}: <i>{$driverInfo}</i>&nbsp;
							<input type="submit" name="EditDriver" value="{t}Edit{/t}">
							<input type="submit" name="RemoveDriver" value="{t}Remove{/t}">
					</td>
				</tr>
			</table>
  </td>
 </tr>
</table>

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/closedlock.png"> {t}Permissions{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="border-right:1px solid #B0B0B0; width:50%">
			<table style="width:100%" summary=''>
				<tr>
					<td>
						{t}Users which are allowed to use this printer{/t}<br>
		     				<select size="1" name="UserMember[]" title="{t}Users{/t}" style="width:100%;height:120px;"  multiple>
                				{html_options options=$UserMembers values=$UserMemberKeys}
               				</select><br>
							<input type="submit" value="{t}Add user{/t}"  name="AddUser">
							<input type="submit" value="{t}Add group{/t}" name="AddGroup">
							<input type="submit" value="{t}Delete{/t}" name="DelUser">
					</td>
				</tr>
			</table>	
	
		</td>
		<td>
			<table style="width:100%" summary=''>
				<tr>
					<td>
						{t}Users which are allowed to administrate this printer{/t}<br>
		     				<select size="1" name="AdminMember[]" title="{t}Admins{/t}" style="width:100%;height:120px;"  multiple>
                				{html_options options=$AdminMembers values=$AdminMemberKeys}
               				</select><br>
							<input type="submit" value="{t}Add user{/t}"  name="AddAdminUser">
							<input type="submit" value="{t}Add group{/t}" name="AddAdminGroup">
							<input type="submit" value="{t}Delete{/t}" name="DelAdmin">
		
					</td>
				</tr>
			</table>
			
		</td>
	</tr>
</table>


<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>
{$netconfig}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
