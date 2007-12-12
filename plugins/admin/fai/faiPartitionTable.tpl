<table width="100%" summary="">
	<tr>
		<td width="50%" valign="top">
				<h2><img class="center" alt="" src="images/fai_small.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}{$must}
							</LABEL>
						</td>
						<td>
{render acl=$cnACL}
							<input value="{$cn}" size="45" maxlength="80" id='cn' disabled >
{/render}
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="description">
							{t}Description{/t}
							</LABEL>
						</td>
						<td>
{render acl=$descriptionACL}
							<input value="{$description}" size="45" maxlength="80" name="description" id="description">
{/render}
						</td>
					</tr>
				</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);">
		   &nbsp;
	 	</td>
		<td>
				<h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle" title="{t}Objects{/t}">&nbsp;
					<LABEL for="SubObject">
						{t}Discs{/t}
					</LABEL>
				</h2>
				<table width="100%" summary="">
				<tr>
					<td>
						<select name="disks[]" title="{t}Choose a disk to delete or edit{/t}" style="width:100%" size="20" id="SubObject" multiple>
							{html_options options=$disks}
							<option>&nbsp;</option>
						</select><br>
{if $sub_object_is_addable}
						<input type="submit" name="AddDisk"     value="{t}Add{/t}"     title="{t}Add{/t}">
{else}
						<input type="submit" name="AddDisk"     value="{t}Add{/t}"     title="{t}Add{/t}" disabled >
{/if}

						<input type="submit" name="EditDisk"    value="{t}Edit{/t}"    title="{t}Edit{/t}">
						<input type="submit" name="DelDisk"     value="{t}Delete{/t}"  title="{t}Delete{/t}">
					</td>
				</tr>
				</table>
		</td>
	</tr>
</table>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn','description');
  -->
</script>

