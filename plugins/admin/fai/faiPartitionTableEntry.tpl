<input type="hidden" name="TableEntryFrameSubmitted" value="1">
<h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Device{/t}</h2>
<table style='width:100%' summary="">
	<tr>
		<td style='width:50%;border-right:1px solid #909090;'><LABEL for="DISK_cn">
			{t}Name{/t}
			</LABEL>{$must}&nbsp;
			<input value="{$DISK_cn}" size="45" maxlength="80" name="DISK_cn" id="DISK_cn" {$cnACL}>
		</td>
		<td><LABEL for="DISK_description">
			&nbsp;{t}Description{/t}
			</LABEL>&nbsp;
			<input value="{$DISK_description}" size="45" maxlength="80" name="DISK_description" id="DISK_description" {$descriptionACL}>
		</td>
	</tr>
</table>
<br>
<p class="seperator">&nbsp;</p>
<br>
<h2><img class="center" alt="" src="images/editpaste.png" align="middle" title="{t}Partition entries{/t}">&nbsp;{t}Partition entries{/t}</h2>
{$setup}
<br>
<input type="submit" name="AddPartition" value="{t}Add partition{/t}" {$cnACL}>
<br>	
<br>
<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
<input type="submit" name="SaveDisk" value="{t}Save{/t}" {$cnACL}>
<input type="submit" name="CancelDisk" value="{t}Cancel{/t}" >
</div>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('DISK_cn');
  -->
</script>

