<h2>{t}FAI partition table entry{/t}
<input type="hidden" name="TableEntryFrameSubmitted" value="1">
<br><br>
<table>
	<tr>
		<td>{t}Name{/t}
		</td>
		<td>
	        <select name="DISK_cn" title="{t}Choose a disc name{/t}" {$DISK_cnACL}>
                {html_options values=$cns output=$cns selected=$DISK_cn}
            </select><br>
		</td>
	</tr>
	<tr>
		<td>{t}Description{/t}
		</td>
		<td><input name="DISK_description" value="{$DISK_description}">
		</td>
	</tr>
</table>	
<br>
<p class="seperator">&nbsp;</p>
<br>
{$setup}
<br>
<input type="submit" name="AddPartition" value="{t}Add partition{/t}" >
<br>	
<br>
<p class="seperator">&nbsp;</p>
<div style="align:right;" align="right">
<input type="submit" name="SaveDisk" value="{t}Save{/t}" >
<input type="submit" name="CancelDisk" value="{t}cancel{/t}" >
</div>
