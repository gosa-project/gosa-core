
{if $type == "FOLDER"}
<h3>{$entry.NAME}</h3>

<table summary="{t}Edit application image{/t}" >
	<tr>
		<td>
			{t}Folder image{/t}
		</td>
		<td>
			{if $image_set}
				<img src="getbin.php?{$rand}" alt='{t}Could not load image.{/t}'>
			{else}
				<i>{t}None{/t}</i>
			{/if}
		</td>
	</tr>
	<tr>
		<td colspan=2>
			&nbsp;
		</td>
	</tr>
	<tr>
		<td>{t}Upload image{/t}
		</td>
		<td>
			<input type="FILE" name="folder_image">
			<button type='submit' name='folder_image_upload'>{t}Upload{/t}</button>

		</td>
	</tr>
	<tr>
		<td>{t}Reset image{/t}</td>
		<td><button type='submit' name='edit_reset_image'>{t}Reset{/t}</button>
</td>
	</tr>
</table>
<br>
{/if}

{if $type == "ENTRY"}
<h3>{t}Application settings{/t}</h3>
<table summary="{t}Edit application settings{/t}">
	<tr>
		<td>{t}Name{/t}</td>
		<td>{$entry.NAME}</td>
	</tr>
	<tr>
		<td colspan="2">
			&nbsp;
		</td>
	</tr>
	<tr>
		<td colspan="2">
			<b>{t}Application options{/t}</b>
		</td>
	</tr>
{foreach from=$paras item=item key=key}
	<tr>
		<td>{$key}&nbsp;</td>
		<td><input style='width:200px;' type='text' name="parameter_{$key}" value="{$item}"></td>
	</tr>
{/foreach}
</table>
{/if}
<p class="seperator">
</p>
<div style="width:100%; text-align:right; padding:3px;">
	<button type='submit' name='app_entry_save'>{msgPool type=saveButton}</button>

	&nbsp;
	<button type='submit' name='app_entry_cancel'>{msgPool type=cancelButton}</button>

</div>
