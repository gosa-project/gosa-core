<table width="100%" summary="">
	<tr>
		<td width="50%" valign="top">
			<h2><img class="center" alt="" src="images/forward.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
			<table summary="" cellspacing="4">
				<tr>
					<td>
						<LABEL for="cn">
						{t}Name{/t}{$must}
						</LABEL>
					</td>
					<td>
{render acl=$cnACL}
						<input value="{$cn}" size="45" maxlength="80" id="cn" name="cn">
{/render}
					</td>
				</tr>
				<tr>
					<td>
						<LABEL for="description">
						{t}Description{/t}
						</LABEL>
					</td>
					<td>
{render acl=$descriptionACL}
						<input value="{$description}" size="45" maxlength="80" name="description" id="description">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td valign="top" style="border-left: 1px solid rgb(160, 160, 160);">
			<table summary="" style="vertical-align:top; text-align:left;" cellpadding=4 width="100%">
				<tr>
					<td style="vertical-align:top;">
						<h2><img class="center" alt="" src="images/fai_template.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}FAI classes{/t}</h2> 
<!--						<select name="FAIclass" title="{t}Choose a priority{/t}" size="20" style="width:100%;" multiple>
							{html_options values=$FAIclasses output=$FAIclasses}
						</select>-->
						{$divlist}
						<br>
{render acl=$FAIclassACL}
						<input type="submit" name="AddClass" value="{t}Add{/t}">
{/render}
						<input type="hidden" name="edit_helper">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn','description');
  -->
</script>

