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
		<td valign="top">
				<h2><img alt="" src="images/fai_script.png" align="middle" title="{t}Properties{/t}">&nbsp;{t}Script attributes{/t}</h2>
				<table width="100%">
					<tr>
						<td>
							<LABEL for="Object_FAIpriority">
							{t}Priority{/t}
							</LABEL>
						</td>
						<td>
						    <select name="Object_FAIpriority" title="{t}Choose a priority{/t}" Object_FAIpriority>
                				{html_options values=$Object_FAIprioritys output=$Object_FAIprioritys selected=$Object_FAIpriority}
            				</select>
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<h2><img alt="" src="images/fai_script.png" align="middle" title="{t}Script attributes{/t}">&nbsp;
	<LABEL for="Object_FAIscript">
		{t}Script{/t}
	</LABEL>
	</h2>
<table width="99%">
    <tr>
        <td>
            <textarea name="Object_FAIscript" style="width:100%;height:300px;" id="Object_FAIscript">{$Object_FAIscript}</textarea>
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

