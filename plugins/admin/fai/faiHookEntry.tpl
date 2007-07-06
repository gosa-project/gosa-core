<input type="hidden" name="SubObjectFormSubmitted" value="1">
<table width="100%" summary="">
	<tr>
		<td valign="top" width="50%">
			<h2><img class="center" alt="" src="images/forward.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="">
					<tr>
						<td>
							{t}Name{/t}{$must}&nbsp;
						</td>
						<td>
							<input value="{$cn}" size="45" name="cn" {$cnACL}>
						</td>
					</tr>
					<tr>
						<td>
							{t}Description{/t}&nbsp;
						</td>
						<td>
							<input value="{$description}" size="45" name="description" {$descriptionACL}>
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
           &nbsp;
        </td>
		<td style="vertical-align:top">
				<h2><img class="center" alt="" src="images/fai_hook.png" align="middle" title="{t}Hook attributes{/t}">&nbsp;{t}Hook attributes{/t}</h2>
				<table width="100%" summary="">
					<tr>
						<td>
							<LABEL for="FAItask">
							{t}Task{/t}&nbsp;
							</LABEL>
							<select name="FAItask" title="{t}Choose an existing FAI task{/t}" {$FAItaskACL}>
								{html_options values=$tasks output=$tasks selected=$FAItask}
							</select>
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>


<p class="seperator">&nbsp;</p>
<h2><img class="center" alt="" src="images/fai_hook.png" align="middle" title="{t}Hook attributes{/t}">&nbsp;
    <LABEL for="FAIscript">
        {t}Script{/t}
    </LABEL>
    </h2>
<table width="99%" summary="">
    <tr>
        <td>
            <textarea name="FAIscript" style="width:100%;height:300px;" id="FAIscript" >{$FAIscript}</textarea>
        </td>
    </tr>
</table>
<br>
<div>
    <input type="file" name="ImportFile" {$FAIscriptACL}>&nbsp;
    <input type="submit" name="ImportUpload" value="{t}Import script{/t}" {$FAIscriptACL}>
	{$DownMe}
</div>

<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
	<input type="submit" value="{t}Apply{/t}" 	name="SaveSubObject" {$cnACL}>&nbsp;
	<input type="submit" value="{t}Cancel{/t}" 	name="CancelSubObject">
</div>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn','description');
  -->
</script>

