<table summary="" width="100%">
	<tr>
		<td valign="top" style="border-right:1px solid #A0A0A0; width:50%">
				<h2><img class="center" alt="" src="images/fai_small.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}{$must}
							</LABEL>
						</td>
						<td>
{render acl=$cnACL}
							<input value="{$cn}" size="45" maxlength="80" disabled id="cn">
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
		<td style="width:50%">
				<h2><img class="center" alt="" src="images/fai_template.png" align="middle" title="{t}Objects{/t}">&nbsp;
					{t}List of template files{/t}
				</h2>
			{$Entry_divlist}
{if $sub_object_is_addable}
				<input type="submit" name="AddSubObject"     value="{t}Add{/t}"		title="{t}Add{/t}">
{else}
				<input type="submit" name="AddSubObject"     value="{t}Add{/t}"		title="{t}Add{/t}" disabled>
{/if}
		</td>
	</tr>
</table>
<input type="hidden" value="1" name="FAItemplate_posted">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn','description');
  -->
</script>

