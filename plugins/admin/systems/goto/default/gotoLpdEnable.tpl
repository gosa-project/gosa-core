{if !$is_account}
	{render acl=$acl}
		<label>
			<p>
				<input type="checkbox" name="gotoLpdEnable_enabled" onChange="cdocument.mainform.submit();" {if $is_account} checked {/if}>
				<span>{t}Enable printer settings{/t}</span>
			</p>
		</label>
	{/render}
{else}
	<div class="system-goto-ldap-enable">
		<div class="row">
			{render acl=$acl}
				<div class="col s12 xl6">
					<label>
						<p>
							<input type="checkbox" name="gotoLpdEnable_enabled" onChange="document.mainform.submit();" {if $is_account} checked {/if}>
							<span>{t}Enable printer settings{/t}</span>
						</p>
					</label>

					<div class="input-field">
						<select name="s_Type" id="s_Type" onChange="document.mainform.submit();" size=1>
							{html_options options=$a_Types selected=$s_Type}
						</select>
						<label for="s_Type">{t}Type{/t}</label>
					</div>

					<div class="input-field">
						<input type="text" name="s_Device" id="s_Device" value="{$s_Device}">
						<label for="s_Device">{t}Device{/t}</label>
					</div>

					<div class="input-field">
						<input type="text" name="i_Port" id="i_Port" value="{$i_Port}">
						<label for="i_Port">{t}Port{/t}</label>
					</div>

					<div class="input-field">
						<input type="text" name="s_Options" id="s_Options" value="{$s_Options}">
						<label for="s_Options">{t}Options{/t}</label>
					</div>

					<label>
						<p>
							<input type="checkbox" name="s_WriteOnly" value="Y" {if $s_WriteOnly == "Y"} checked {/if}>
							<span>{t}Write only{/t}</span>
						</p>
					</label>
				</div>
			{/render}

			{if $s_Type == "S"}
				{render acl=$acl}
					<div class="col s12 xl6">
						<div class="input-field">
							<select name="s_Speed" id="s_Speed" size=1>
								{html_options options=$a_Speeds selected=$s_Speed}
							</select>
							<label for="s_Speed">{t}Bit rate{/t}</label>
						</div>

						<div class="input-field">
							<select name="s_FlowControl" id="s_FlowControl" size=1>
								{html_options options=$a_FlowControl selected=$s_FlowControl}
							</select>
							<label for="s_FlowControl">{t}Flow control{/t}</label>
						</div>

						<div class="input-field">
							<select name="s_Parity" id="s_Parity" size=1>
								{html_options options=$a_Parities selected=$s_Parity}
							</select>
							<label for="s_Parity">{t}Parity{/t}</label>
						</div>

						<div class="input-field">
							<select name="i_Bit" id="i_Bit" size=1>
								{html_options options=$a_Bits selected=$i_Bit}
							</select>
							<label for="i_Bit">{t}Bits{/t}</label>
						</div>
					</div>
				{/render}
			{/if}
		</div>
	</div>
{/if}
<input type="hidden" name="gotoLpdEnable_entry_posted" value="1">
