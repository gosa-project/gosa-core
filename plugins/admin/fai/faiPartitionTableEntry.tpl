<input type="hidden" name="TableEntryFrameSubmitted" value="1">
<br>
<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
<table>
	<tr>
		<td><LABEL for="DISK_cn">
			{t}Name{/t}
			</LABEL>&nbsp;{$must}
		</td>
		<td>
			<select name="DISK_cn" title="{t}Choose a disc name{/t}" {$DISK_cnACL} id="DISK_cn">
                {html_options values=$cns output=$cns selected=$DISK_cn}
            </select>
		</td>
	</tr>
	<tr>
		<td><LABEL for="DISK_description">
			{t}Description{/t}
			</LABEL>&nbsp;{$must}
		</td>
		<td>
			<input value="{$DISK_description}" name="DISK_description" id="DISK_description">
		</td>
	</tr>
</table>
<br>
<p class="seperator">&nbsp;</p>
<br>
<h2><img alt="" src="images/editpaste.png" align="middle" title="{t}Partition entries{/t}">&nbsp;{t}Partition entries{/t}</h2>
{$setup}
<br>
<input type="submit" name="AddPartition" value="{t}Add partition{/t}" >
<br>	
<br>
<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
<input type="submit" name="SaveDisk" value="{t}Save{/t}" >
<input type="submit" name="CancelDisk" value="{t}cancel{/t}" >
</div>
