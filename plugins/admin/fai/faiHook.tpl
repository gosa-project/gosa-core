<table width="100%" summary=''>
	<tr>
		<td width="50%" valign="top">
				<h2><img class="center" alt="" src="images/fai_small.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$cn}" size="45" maxlength="80" disabled id="cn" {$cnACL}>
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="description">
							{t}Description{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$description}" size="45" maxlength="80" {$descriptionACL} name="description" id="description">
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<table width="100%" summary=''>
	<tr>
		<td>
				<h2><img class="center" alt="" src="images/fai_hook.png" align="middle" title="{t}Objects{/t}">&nbsp;
					<LABEL for="SubObject">
						{t}List of hook scripts{/t}
					</LABEL>
				</h2>
				<table width="100%" summary=''>
				<tr>
					<td>
						{$Entry_divlist}
						<input type="submit" name="AddSubObject"     value="{t}Add{/t}"		title="{t}Add{/t}" {$cnACL}>
					</td>
				</tr>
				</table>
		</td>
	</tr>
</table>
<input type="hidden" value="1" name="FAIhook_posted">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.description.focus();
  -->
</script>

