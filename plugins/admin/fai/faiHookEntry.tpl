<input type="hidden" name="SubObjectFormSubmitted" value="1">
<table width="100%" summary="">
	<tr>
		<td valign="top" width="50%">
			<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table>
					<tr>
						<td>
							{t}Name{/t}&nbsp;{$must}
						</td>
						<td>
							<input value="{$Object_cn}" name="Object_cn">
						</td>
					</tr>
					<tr>
						<td>
							{t}Description{/t}&nbsp;{$must}
						</td>
						<td>
							<input value="{$Object_description}" name="Object_description">
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
           &nbsp;
        </td>
		<td>
				<h2><img alt="" src="images/fai_hook.png" align="middle" title="{t}Hook attributes{/t}">&nbsp;{t}Hook attributes{/t}</h2>
				<table width="100%">
					<tr>
						<td>
							<LABEL for="Object_FAIscript">
							{t}Script{/t}&nbsp;{$must}
							</LABEL>
						</td>
						<td>
							<input type="text" name="Object_FAIscript" value="{$Object_FAIscript}" id="Object_FAIscript" style="width:280px;">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="Object_FAItask">
							{t}Task{/t}&nbsp;{$must}
							</LABEL>
						</td>
						<td>
							<input type="text" name="Object_FAItask" value="{$Object_FAItask}" id="Object_FAItask" style="width:280px;">
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
	<input type="submit" value="{t}Abort{/t}" 	name="CancelSubObject">
</div>

