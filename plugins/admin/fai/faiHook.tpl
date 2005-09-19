<table width="100%">
	<tr>
		<td width="50%" valign="top">
				<h2><img alt="" src="images/fai_small.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$cn}" disabled id="cn">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="description">
							{t}Description{/t}{$must}
							</LABEL>
						</td>
						<td>
							<input value="{$description}" {$description} name="description" id="description">
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
		   &nbsp;
	 	</td>
		<td>
				<h2><img alt="" src="images/fai_hook.png" align="middle" title="{t}Objects{/t}">&nbsp;
					<LABEL for="SubObject">
						{t}List of hook scripts{/t}
					</LABEL>
				</h2>
				<table width="100%">
				<tr>
					<td>
						<select name="SubObject" title="{t}Choose a hook to delete or edit{/t}" style="width:100%" size=15 id="SubObject">
							{html_options values=$SubObjectKeys output=$SubObjects}
						</select><br>
						<input type="submit" name="AddSubObject"     value="{t}Add{/t}"		title="{t}Add{/t}">
						<input type="submit" name="EditSubObject"    value="{t}Edit{/t}"    title="{t}Edit{/t}">
						<input type="submit" name="DelSubObject"     value="{t}Delete{/t}"  title="{t}Delete{/t}">
					</td>
				</tr>
				</table>
		</td>
	</tr>
</table>
