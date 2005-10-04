<input type="hidden" name="SubObjectFormSubmitted" value="1">
<table width="100%" summary="">
	<tr>
		<td valign="top" width="50%">
			<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table>
					<tr>
						<td>
							{t}Name{/t}{$must}&nbsp;
						</td>
						<td>
							<input value="{$Object_cn}" name="Object_cn">
						</td>
					</tr>
					<tr>
						<td>
							{t}Description{/t}&nbsp;
						</td>
						<td>
							<input size=40 value="{$Object_description}" name="Object_description">
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
           &nbsp;
        </td>
		<td>
				<h2><img alt="" src="images/fai_template.png" align="middle" title="{t}Template attributes{/t}">&nbsp;{t}Template attributes{/t}</h2>
				<table width="100%">
					<tr>
						<td>
							<LABEL for="Object_FAItemplateFile">
							{t}File{/t}{$must}&nbsp;
							</LABEL>
						</td>
						<td>
							<input type="text" name="Object_FAItemplateFile" value="{$Object_FAItemplateFile}" id="Object_FAItemplateFile">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="Object_FAItemplatePath">
							{t}Destination path{/t}{$must}&nbsp;
							</LABEL>
						</td>
						<td>
							<input type="text" name="Object_FAItemplatePath" value="{$Object_FAItemplatePath}" id="Object_FAItemplatePath">
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
	<input type="submit" value="{t}Save{/t}" 	name="SaveSubObject">&nbsp;
	<input type="submit" value="{t}Cancel{/t}" 	name="CancelSubObject">
</div>

