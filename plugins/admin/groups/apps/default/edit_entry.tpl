<div class="startmenu-edit-page">
	{if $type == "FOLDER"}
		<h3>{$entry.NAME}</h3>
		<div class="folder-image">
			<p><b>{t}Folder image{/t}:</b></p>

			{if $image_set}
				<img src="getbin.php?{$rand}" alt='{t}Could not load image.{/t}'>
			{else}
				<i class="material-icons">image</i>
				<p>Kein Verzeichnisbild vorhanden.</p>
			{/if}
		</div>

		<div class="image-action">
			<button class="btn-small" type='submit' name='edit_reset_image'>{t}Reset{/t}</button>

			<div class="file-field input-field">
				<div class="btn-small">
					<span>{t}Browse{/t}</span>
					<input id="folder_image" name="folder_image" type="file">
				</div>
				<div class="file-path-wrapper">
					<input placeholder='{t}No picture selected{/t}' type="text" class="file-path validate">
				</div>
			</div>
			<button class="btn-small" type='submit' name='folder_image_upload'>{t}Upload{/t}</button>
		</div>
	{/if}

	{if $type == "ENTRY"}
		<h3>{t}Application settings{/t}</h3>

		<div class="input-field">
			<input type="text" name="menu-name" value="{$entry.NAME}" id="menu-name" readonly style='width:200px;'>
			<label for="menu-name">{t}Name{/t}</label>
		</div>

		<p class="app-opt-headline">{t}Application options{/t}:</p>

		<ul class="application-options">
			{foreach from=$paras item=item key=key}
				<li>
					<div class="input-field">
						<input type="text" name="parameter_{$key}" value="{$item}" style='width:200px;'>
						<label for="parameter_{$key}">{$key}</label>
					</div>
				</li>
			{/foreach}
		</ul>
	{/if}
</div>

<div class="card-action">
	<button class="btn-small primary" type='submit' name='app_entry_save'>{msgPool type=saveButton}</button>

	<button class="btn-small primary" type='submit' name='app_entry_cancel'>{msgPool type=cancelButton}</button>
</div>
