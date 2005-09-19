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
							<input size=40 value="{$description}" {$description} name="description" id="description">
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
		   &nbsp;
	 	</td>
		<td>
				<h2><img alt="" src="images/fai_partitionTable.png" align="middle" title="{t}Objects{/t}">&nbsp;
					<LABEL for="SubObject">
						{t}Discs{/t}
					</LABEL>
				</h2>
				<table width="100%">
				<tr>
					<td>
						<select name="disks" title="{t}Choose a disk to delete or edit{/t}" style="width:100%" size=15 id="SubObject">
							{html_options values=$diskKeys output=$disks}
						</select><br>
						<input type="submit" name="AddDisk"     value="{t}Add{/t}"		title="{t}Add{/t}">
						<input type="submit" name="EditDisk"    value="{t}Edit{/t}"    title="{t}Edit{/t}">
						<input type="submit" name="DelDisk"     value="{t}Delete{/t}"  title="{t}Delete{/t}">
					</td>
				</tr>
				</table>
		</td>
	</tr>
</table>
