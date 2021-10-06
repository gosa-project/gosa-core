{if !$is_account}

<table style='width:100%;' summary="{t}Printer settings{/t}">
	<tr>
		<td style='width:55%;'>
      {render acl=$acl}
      <input class="center" type='checkbox' onChange="document.mainform.submit();" 
        {if $is_account} checked {/if}
        name='gotoLpdEnable_enabled'>&nbsp;{t}Enable printer settings{/t}</td>
      {/render}
		</td>
	</tr>
</table>
{else}
<table style='width:100%;' summary="{t}Generic settings{/t}">
	<tr>
		<td>
			<table summary="{t}Generic settings{/t}">
				<tr>
					<td colspan="2">
						{render acl=$acl}
						<input class="center" type='checkbox' onChange="document.mainform.submit();" 
							{if $is_account} checked {/if}
							name='gotoLpdEnable_enabled'>&nbsp;{t}Enable printer settings{/t}</td>
						{/render}
					</TD>
				</tr>
				<tr>
					<td>{t}Type{/t}</td>
					<td>	
						{render acl=$acl}
						<select name='s_Type'  onChange="document.mainform.submit();" size=1>
							{html_options options=$a_Types selected=$s_Type}
						</select>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Device{/t}</td>
					<td>	
						{render acl=$acl}
						<input type='text' name='s_Device' value='{$s_Device}'>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Port{/t}</td>
					<td>
						{render acl=$acl}
						<input type='text' name='i_Port' value='{$i_Port}'>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Options{/t}</td>
					<td>
						{render acl=$acl}
						<input type='text' name='s_Options' value='{$s_Options}'>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Write only{/t}</td>
					<td>
						{render acl=$acl}
						<input {if $s_WriteOnly == "Y"} checked {/if} type='checkbox' name='s_WriteOnly' value='Y' >
						{/render}
					</td>
				</tr>
			</table>
		</td>
		<td>
{if $s_Type == "S"}
			<table summary="{t}Generic settings{/t}">
				<tr>
					<td>{t}Bit rate{/t}</td>
					<td>
						{render acl=$acl}
						<select name='s_Speed' size=1>
							{html_options options=$a_Speeds selected=$s_Speed}
						</select>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Flow control{/t}</td>
					<td>
						{render acl=$acl}
						<select name='s_FlowControl' size=1>
							{html_options options=$a_FlowControl selected=$s_FlowControl}
						</select>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Parity{/t}</td>
					<td>
						{render acl=$acl}
						<select name='s_Parity' size=1>
							{html_options options=$a_Parities selected=$s_Parity}
						</select>
						{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Bits{/t}</td>
					<td>
						{render acl=$acl}
						<select name='i_Bit' size=1>
							{html_options options=$a_Bits selected=$i_Bit}
						</select>
						{/render}
					</td>
				</tr>
			</table>
{/if}
		</td>
	</tr>
</table>
{/if}
<input type='hidden' name="gotoLpdEnable_entry_posted" value="1">
