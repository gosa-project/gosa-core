<table width="100%">
	<tr>
		<td width="50%" valign="top">
			<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
			<table summary="" cellspacing="4">
				<tr>
					<td>
						<LABEL for="cn">
						{t}Name{/t}{$must}
						</LABEL>
					</td>
					<td>
						<input value="{$cn}" id="cn" name="cn">
					</td>
				</tr>
				<tr>
					<td>
						<LABEL for="description">
						{t}Description{/t}
						</LABEL>
					</td>
					<td>
						<input value="{$description}" {$description} name="description" id="description">
					</td>
				</tr>
			</table>
		</td>
		<td valign="top" style="border-left: 1px solid rgb(160, 160, 160);">
			<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4>
				<tr>
					<td style="vertical-align:top;">
						<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}FAI classes{/t}</h2> 
						<select name="FAIclass" title="{t}Choose a priority{/t}" style="width:350px;height:150px;" multiple>
							{html_options values=$FAIclasses output=$FAIclasses}
						</select>
						<br>
						<input type="submit" name="AddClass" value="{t}Add{/t}">
						<input type="submit" name="DelClass" value="{t}Delete{/t}">
						<input type="hidden" name="edit_helper">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<!--
<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
    Das ist zu groß hier... Macht keinen Sinn - 
     <LABEL for="userlist">{t}List of used class names{/t}</LABEL> 
    </p>
  </div>
			<div class="contentboxb" style="border-top:1px solid #B0B0B0;">
				
				{$divlisthead}
			</div>
			<div style='height:4px;'>
			</div>
			<div class="contentboxb" style="border-top:1px solid #B0B0B0;">
				{$divlist}
				<input type="hidden" name="edit_helper">
			</div>
		</td>
	</tr>
</table>
-->
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.description.focus();
  -->
</script>

