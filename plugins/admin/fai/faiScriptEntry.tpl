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
							<input value="{$cn}" name="cn">
						</td>
					</tr>
					<tr>
						<td>
							{t}Description{/t}&nbsp;
						</td>
						<td>
							<input size=40 value="{$description}" name="description">
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
           &nbsp;
        </td>
		<td valign="top">
				<h2><img alt="" src="images/fai_script.png" align="middle" title="{t}Properties{/t}">&nbsp;{t}Script attributes{/t}</h2>
				<table width="100%">
					<tr>
						<td>
							<LABEL for="FAIpriority">
							{t}Priority{/t}
							</LABEL>
						</td>
						<td>
						    <select name="FAIpriority" title="{t}Choose a priority{/t}" FAIpriority>
                				{html_options values=$FAIprioritys output=$FAIprioritys selected=$FAIpriority}
            				</select>
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<h2><img alt="" src="images/fai_script.png" align="middle" title="{t}Script attributes{/t}">&nbsp;
	<LABEL for="FAIscript">
		{t}Script{/t}
	</LABEL>
	</h2>
<table width="99%">
    <tr>
        <td>
            <textarea name="FAIscript" style="width:100%;height:300px;" id="FAIscript">{$FAIscript}</textarea>
        </td>
    </tr>
</table>
<br>
<div>
	<input type="file" name="ImportFile">&nbsp;
	<input type="submit" name="ImportUpload" value="{t}Import script{/t}">
</div>
<br>
<div style="align:right;" align="right">
	<input type="submit" value="{t}Save{/t}" 	name="SaveSubObject">&nbsp;
	<input type="submit" value="{t}Cancel{/t}" 	name="CancelSubObject">
</div>

