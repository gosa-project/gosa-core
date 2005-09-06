<h2>{t}FAI partition table{/t}
<br><br>
<table>
	<tr>
		<td>
			{t}Name{/t}
		</td>
		<td>
			<input value="{$cn}" disabled>
		</td>
	</tr>
	<tr>
		<td>
			{t}Description{/t}
		</td>
		<td>
			<input value="{$description}" {$description} name="description">
		</td>
	</tr>
</table>
<br>
<br>
<h2>{t}Already defined disk{/t}</h2>
<table width="100%">
	<tr>
		<td>
	    	<select name="disks" title="{t}Choose a disk to delete or edit{/t}" style="width:450px;height:250px;" multiple>
       			{html_options values=$diskKeys output=$disks}
      		</select><br>
			<input type="submit" name="AddDisk" 	value="{t}Add{/t}">
			<input type="submit" name="EditDisk" 	value="{t}Edit{/t}">
			<input type="submit" name="DelDisk" 	value="{t}Delete{/t}">
		</td>
	</tr>
</table>
	
