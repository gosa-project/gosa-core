{if $RestoreMode}

<h3>{t}Restoring object snapshots{/t}</h3>
<hr>
<br>
{t}This procedure will restore a snapshot of the selected object. It will replace the existing object after pressing the restore button.{/t}
<br>
<br>
{t}DNS configuration and some database entries cannot be restored. They need to be recreated manually.{/t}
<br>
<br>
{t}Don't forget to check references to other objects, for example does the selected printer still exists ?{/t}
<br>
<hr>
<br>
<table summary="" style="width:100%">
	{if !$restore_deleted}
	<tr>
		<td>
		<b>{t}Object{/t}</b>&nbsp;
		{$CurrentDN}
		</td>
	</tr>
	{/if}
	<tr>
		<td>
			<br>
			{if $CountSnapShots==0}
				{t}There is no snapshot available that can be restored{/t}
			{else}
				{t}Choose a snapshot and click the folder image, to restore the snapshot{/t}
			{/if}
		</td>
	</tr>
	<tr>
		<td>
			{$SnapShotList}
		</td>
	</tr>
</table>

<hr>
<div class="plugin-actions">
    <button type='submit' name='CancelSnapshot'>{t}Cancel{/t}</button>

</div>

{else}

<h2>{t}Creating object snapshots{/t}</h2>
<hr>
<br>
{t}This procedure will create a snapshot of the selected object. It will be stored inside a special branch of your directory system and can be restored later on.{/t}
<br>
<br>
{t}Remember that database entries, DNS configurations and possibly created zones in server extensions will not be stored in the snapshot.{/t}
<br>
<hr>
<br>
<table summary="" style="width:100%">
	<tr>
		<td>
			<b>{t}Object{/t}</b>
		</td>
		<td style="width:95%"> 
		   {$CurrentDN}
		</td>
	</tr>
	<tr>
		<td>
			<b>{t}Time stamp{/t}</b> 
		</td>
		<td> 
		   {$CurrentDate}
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<br>
			{t}Reason for generating this snapshot{/t}<br> 
			<textarea name="CurrentDescription" style="width:100%;height:160px;" rows=10 cols=100>{$CurrentDescription}</textarea>
		</td>
	</tr>
</table>

<hr>
<div class="plugin-actions">
    <button type='submit' name='CreateSnapshot'>{t}Continue{/t}</button>

    <button type='submit' name='CancelSnapshot'>{t}Cancel{/t}</button>

</div>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.CurrentDescription.focus();
  -->
</script>
{/if}
