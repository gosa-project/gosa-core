<input type="hidden" name="TableEntryFrameSubmitted" value="1">
<h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Device{/t}</h2>
<table style='width:100%' summary="">
	<tr>
		<td style='width:50%;border-right:1px solid #909090;'><LABEL for="DISKcn">
			{t}Name{/t}
			</LABEL>{$must}&nbsp;
{render acl=$DISKcnACL}
			<input value="{$DISKcn}" size="45" maxlength="80" name="DISKcn" id="DISKcn">
{/render}
		</td>
		<td><LABEL for="DISKdescription">
			&nbsp;{t}Description{/t}
			</LABEL>&nbsp;
{render acl=$DISKdescriptionACL}
			<input value="{$DISKdescription}" size="45" maxlength="80" name="DISKdescription" id="DISKdescription">
{/render}
		</td>
	</tr>
</table>
<br>
<p class="seperator">&nbsp;</p>
<br>
<h2><img class="center" alt="" src="images/editpaste.png" align="middle" title="{t}Partition entries{/t}">&nbsp;{t}Partition entries{/t}</h2>
{$setup}
<br>
{if $sub_object_is_createable}
	<input type="submit" name="AddPartition" value="{t}Add partition{/t}">
{else}
	<input type="submit" name="restricted" value="{t}Add partition{/t}" disabled>
{/if}
<br>	
<br>
<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
{render acl=$allowSave}
<input type="submit" name="SaveDisk" value="{t}Save{/t}">
{/render}
<input type="submit" name="CancelDisk" value="{t}Cancel{/t}" >
</div>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('DISK_cn');
  -->
</script>

