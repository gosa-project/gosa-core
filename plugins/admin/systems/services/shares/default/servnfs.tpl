<h3>{t}Edit share{/t}</h3>
<table summary="{t}NFS setup{/t}" width="100%">
	<tr>
		<td>

		<!--Table left-top-->
			<table summary="{t}Generic settings{/t}">
				<tr>
					<td>
						{t}Name{/t} {$must}
					</td>
					<td>
{render acl=$nameACL}
						<input type="text" name="name" value="{$name}" >
{/render}
					</td>
				</tr>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
{render acl=$descriptionACL}
						<input type="text" size="40" name="description" value="{$description}">
{/render}
					</td>
				</tr>
				<tr>
					<td>
						{t}Path{/t} / {t}Volume{/t} {$must}
					</td>
					<td>
{render acl=$pathACL}
						<input type="text" size="40" name="path" value="{$path}">
{/render}
					</td>
				</tr>
				<tr>
					<td>
						<div id="vlabel">{t}Server{/t}</div>
					</td>
					<td>
{render acl=$volumeACL}
						<input type="text" id="volume" size="40" name="volume" value="{$volume}">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td class='left-border'>


		<!--Table right-top-->
			<table summary="{t}Share type{/t}">
				<tr>
					<td>


						{t}Type{/t}
					</td>
					<td>
{render acl=$typeACL}
						<select size="1" name="type" id="sharetype">
		   					{html_options options=$types selected=$type}
						</select>
{/render}
						{if $allow_mounts == TRUE}
						<br><br>
{render acl=$netatalkmountACL}
						<input type="checkbox" name="netatalk_mount" id="netatalk_mount" {$mount_checked} />
{/render}
						<label for="netatalk_mount">{t}Auto-mount share on Apple systems{/t}</label>
							<br>
							<br>
						{/if}

								{literal}
									<script type="text/javascript" language="JavaScript">
										document.getElementById('sharetype').onchange=function() {
											var foobar=this[this.selectedIndex].value;
											var box=document.getElementById('netatalk_mount');
											var volume=document.getElementById('volume');
											var vlabel=document.getElementById('vlabel');
								{/literal}
									<!-- Only add checkbox enable/disable js part if checkbox is available --> 
									{if $allow_mounts == TRUE}
										{literal}
											if(foobar=="NFS"||foobar=="netatalk"){
												box.disabled=false;
											} else {
												box.disabled=true;
												box.checked=false;
											}
										{/literal}
									{/if}
								{literal}
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
								{/literal}
					</td>
				</tr>
				<tr>
					<td>
						{t}Code page{/t}
					</td>
					<td>
{render acl=$charsetACL}
						<select size="1" name="charset">
   							{html_options options=$charsets selected=$charset}
						</select>	
{/render}
					</td>
				</tr>
				<tr>
				    <td>
					{t}Option{/t}
				    </td>
				    <td>
{render acl=$optionACL}
								<input type="text" name="option" value="{$option}">
{/render}
				    </td>
				</tr>
			</table>
		</td>
	</tr>
</table>

<hr>
<div class="plugin-actions">
			<button type='submit' name='NFSsave'>{msgPool type=saveButton}</button>
			<button type='submit' name='NFScancel'>{msgPool type=cancelButton}</button>
</div>
<input type='hidden' name='servnfs_posted' value='1'>
