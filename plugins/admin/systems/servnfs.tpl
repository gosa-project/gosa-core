<h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle">&nbsp;{t}Edit share{/t}</h2>
<table summary="{t}NFS setup{/t}" width="100%">
	<tr>
		<td width="45%" style="vertical-align:top">
		<!--Table left-top-->
			<table summary="">
				<tr>
					<td>
						{t}Name{/t} {$must}
					</td>
					<td>
						<input type="text" name="name" value="{$name}" {$nameACL}>
					</td>
				</tr>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
						<input type="text" size="40" name="description" value="{$description}">
					</td>
				</tr>
				<tr>
					<td>
						{t}Path{/t}
					</td>
					<td>
						<input type="text" size="40" name="path" value="{$path}">
					</td>
				</tr>
				<tr>
					<td>
						<div id="vlabel">{t}Volume{/t}</div>
					</td>
					<td>
						<input type="text" id="volume" size="40" name="volume" value="{$volume}">
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left:1px solid #A0A0A0">

		<!--Table right-top-->
			<table summary="">
				<tr>
					<td style="vertical-align:top">
						{t}Type{/t}
					</td>
					<td>
						<select size="1" name="type" id="sharetype">
		   					{html_options options=$types selected=$type}
						</select>
						{if $allow_mounts == TRUE}
							<br><br><input type="checkbox" name="netatalk_mount" id="netatalk_mount" {$mount_checked} {$appleMountsACL}/><label for="netatalk_mount">{t}Auto-mount share on Apple systems{/t}</label>
								{literal}
									<script language="JavaScript">
										document.getElementById('sharetype').onchange=function() {
											var foobar=this[this.selectedIndex].value;
											var box=document.getElementById('netatalk_mount');
											var volume=document.getElementById('volume');
											var vlabel=document.getElementById('vlabel');
											if(foobar=="NFS"||foobar=="netatalk"){
												box.disabled=false;
											} else {
												box.disabled=true;
												box.checked=false;
											}
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
							<br>
							<br>
						{/if}
					</td>
				</tr>
				<tr>
					<td>
						{t}Codepage{/t}
					</td>
					<td>
						<select size="1" name="charset">
   							{html_options options=$charsets selected=$charset}
						</select>	
					</td>
				</tr>
				<tr>
				    <td>
					{t}Option{/t}
				    </td>
				    <td>
								<input type="text" name="option" value="{$option}">
				    </td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan=2>
			<p class="seperator">&nbsp;</p>
		</td>
	</tr>
	<tr>
		<td colspan=2 style="text-align:right">
			<input type="submit" name="NFSsave" value="{t}Save{/t}">
			<input type="submit" name="NFScancel" value="{t}Cancel{/t}">
		</td>
	</tr>
</table>
