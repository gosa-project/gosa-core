<div class="servnfs-wrapper">
    <div class="row">
        <div class="col s12 xl6">
			<h3>{t}Edit share{/t}</h3>

            {render acl=$nameACL}
				<div class="input-field">
					<input type='text' id="name" name="name" value="{$name}">
					<label for="name">{t}Name{/t}{$must}</label>
				</div>
            {/render}

            {render acl=$descriptionACL}
				<div class="input-field">
					<input type='text' id="description" name="description" value="{$description}">
					<label for="description">{t}Description{/t}</label>
				</div>
            {/render}

            {render acl=$pathACL}
				<div class="input-field">
					<input type='text' id="path" name="path" value="{$path}">
					<label for="path">{t}Path{/t} / {t}Volume{/t} {$must}</label>
				</div>
            {/render}

            {render acl=$volumeACL}
				<div class="input-field">
					<input type='text' id="volume" name="volume" value="{$volume}">
					<label for="volume">{t}Server{/t}</label>
				</div>
            {/render}
		</div>

		<div class="col s12 xl6">
			{render acl=$typeACL}
				<div class="input-field">
					<select id="sharetype" name="type" size="1">
		   					{html_options options=$types selected=$type}
					</select>
					<label>{t}Type{/t}</label>
				</div>
			{/render}

			{if $allow_mounts == TRUE}
				{render acl=$netatalkmountACL}
					<label>
						<p>
							<input type="checkbox" name="netatalk_mount" id="netatalk_mount" {$mount_checked}>
							<span>{t}Auto-mount share on Apple systems{/t}</span>
						</p>
					</label>
				{/render}
			{/if}

			<script type="text/javascript" language="JavaScript">
				document.getElementById('sharetype').onchange=function() {
					var foobar=this[this.selectedIndex].value;
					var box=document.getElementById('netatalk_mount');
					var volume=document.getElementById('volume');
					var vlabel=document.getElementById('vlabel');
					<!-- Only add checkbox enable/disable js part if checkbox is available -->
					{if $allow_mounts == TRUE}
						if(foobar=="NFS"||foobar=="netatalk"){
							box.disabled=false;
						} else {
							box.disabled=true;
							box.checked=false;
						}
					{/if}
					if(foobar=="NCP"){
						volume.style.visibility="visible";
						vlabel.style.visibility="visible";
					} else {
						volume.style.visibility="hidden";
						vlabel.style.visibility="hidden";
					}
				};
				document.getElementById('sharetype').onchange();
			</script>

			{render acl=$charsetACL}
				<div class="input-field">
					<select id="charset" size="1" name="charset">
						{html_options options=$charsets selected=$charset}
					</select>
					<label>{t}Code page{/t}</label>
				</div>
			{/render}

            {render acl=$optionACL}
				<div class="input-field">
					<input type='text' id="option" name="option" value="{$option}">
					<label for="option">{t}Option{/t}</label>
				</div>
            {/render}
		</div>
	</div>
</div>

<div class="plugin-actions card-action">
	<button class="btn-small primary" type='submit' name='NFSsave'>{msgPool type=saveButton}</button>
	<button class="btn-small primary" type='submit' name='NFScancel'>{msgPool type=cancelButton}</button>
</div>
<input type='hidden' name='servnfs_posted' value='1'>
