
{if $device_type=="pci"}

	<h2>{t}Other devices{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>

{elseif $device_type=="power"}

	<h2>{t}Power supply{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Atx{/t}
						</td>	
						<td>	
							<input type="radio" name="atx" value="Y" {if ($atx == "Y")||($atx=="")}checked {/if}>{t}Yes{/t}
							<input type="radio" name="atx" value="N" {if $atx == "N"}checked {/if}>{t}No{/t}
						</td>
					</tr>
					<tr>
						<td>{t}Power{/t}
						</td>	
						<td>	
							<input name="power" value="{$power}">
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
{elseif $device_type=="gfxcard"}

	<h2>{t}Gfxcard{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Interface{/t}
						</td>	
						<td>	
							<select name="interface">
								{html_options values=$GFXInterfaceKeys output=$GFXInterfaces selected=$interface}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Ram{/t}
						</td>	
						<td>	
							<input name="ram" value="{$ram}">
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
{elseif $device_type=="control"}

	<h2>{t}Controllers{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Interface{/t}
						</td>	
						<td>	
							<select name="interface">
								{html_options values=$HDDInterfaceKeys output=$HDDInterfaces selected=$interface}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Size{/t}
						</td>	
						<td>	
							<input type="radio" name="raid" value="Y" {if ($raid == "Y")||($raid=="")}checked {/if}>{t}Yes{/t}
							<input type="radio" name="raid" value="N" {if $raid == "N"}checked {/if}>{t}No{/t}
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>

{elseif $device_type=="drive"}

	<h2>{t}Drive{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Speed{/t}
						</td>	
						<td>	
							<input type="text" name="speed" value="{$speed}">
						</td>
					</tr>
					<tr>
						<td>{t}Interface{/t}
						</td>	
						<td>	
							<select name="interface">
								{html_options values=$HDDInterfaceKeys output=$HDDInterfaces selected=$interface}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Writeable{/t}
						</td>	
						<td>	
							<input type="radio" name="is_writer" value="Y" {if ($is_writer == "Y")||($is_writer=="")}checked {/if}>{t}Yes{/t}
							<input type="radio" name="is_writer" value="N" {if $is_writer == "N"}checked {/if}>{t}No{/t}
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>

{elseif $device_type=="hdd"}
	<h2>{t}Hdd{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Rpm{/t}
						</td>	
						<td>	
							<input type="text" name="rpm" value="{$rpm}">
						</td>
					</tr>
					<tr>
						<td>{t}Cache{/t}
						</td>	
						<td>	
							<input type="text" name="cache" value="{$cache}">
						</td>
					</tr>
					<tr>
						<td>{t}Size{/t}
						</td>	
						<td>	
							<input type="text" name="specif_default" value="{$specif_default}">
						</td>
					</tr>
					<tr>
						<td>{t}Type{/t}
						</td>	
						<td>	
							<select name="interface">
								{html_options values=$HDDInterfaceKeys output=$HDDInterfaces selected=$interface}
							</select>
						</td>
					</tr>
					
				</table>
			</td>
		</tr>
	</table>

{elseif $device_type=="ram"}

	<h2>{t}RAM{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Frequenz{/t}
						</td>	
						<td>	
							<input type="text" name="frequence" value="{$frequence}">
						</td>
					</tr>
					<tr>
						<td>{t}Size{/t}
						</td>	
						<td>	
							<input type="text" name="specif_default" value="{$specif_default}">
						</td>
					</tr>
					<tr>
						<td>{t}Type{/t}
						</td>	
						<td>	
							<select name="type">
								{html_options values=$RAMtypeKeys output=$RAMtypes selected=$type}
							</select>
						</td>
					</tr>
					
				</table>
			</td>
		</tr>
	</table>

{elseif $device_type=="sndcard"}
	<h2>{t}Soundcard{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Type{/t}
						</td>	
						<td>	
							<input type="text" name="type" value="{$type}">
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
{elseif $device_type=="iface"}
 <h2>{t}Network interface{/t}</h2>
    <table summary="" width="100%">
        <tr>
            <td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
                <table summary="" width="100%">
                    <tr>
                        <td>{t}Name{/t}
                        </td>
                        <td>
                            <input name="designation" value="{$designation}">
                        </td>
                    </tr>
                    <tr>
                        <td style="vertical-align:top;">{t}Comment{/t}
                        </td>
                        <td>
                            <textarea name="comment">{$comment}</textarea>
                        </td>
                    </tr>
                </table>
            </td>
            <td>
                <table summary="" width="100%">
                    <tr>
                        <td>{t}Manufacturer{/t}
                        </td>
                        <td>
                            <select name="FK_glpi_enterprise">
                                {html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td>{t}MAC address{/t}
                        </td>
                        <td>
                            <input type="text" name="specif_default" value="{$specif_default}">
                        </td>
                    </tr>
                    <tr>
                        <td>{t}Bandwidth{/t}
                        </td>
                        <td>
                            <input type="text" name="bandwidth" value="{$bandwidth}">
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
    </table>
{elseif $device_type=="processor"}
 <h2>{t}Processor{/t}</h2>
    <table summary="" width="100%">
        <tr>
            <td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
                <table summary="" width="100%">
                    <tr>
                        <td>{t}Name{/t}
                        </td>
                        <td>
                            <input name="designation" value="{$designation}">
                        </td>
                    </tr>
                    <tr>
                        <td style="vertical-align:top;">{t}Comment{/t}
                        </td>
                        <td>
                            <textarea name="comment">{$comment}</textarea>
                        </td>
                    </tr>
                </table>
            </td>
            <td>
                <table summary="" width="100%">
                    <tr>
                        <td>{t}Manufacturer{/t}
                        </td>
                        <td>
                            <select name="FK_glpi_enterprise">
                                {html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
                            </select>
                        </td>
                    </tr>
                    <tr>
                        <td>{t}Frequence{/t}
                        </td>
                        <td>
                            <input type="text" name="frequence" value="{$frequence}">
                        </td>
                    </tr>
                    <tr>
                        <td>{t}Default frequence{/t}
                        </td>
                        <td>
                            <input type="text" name="specif_default" value="{$specif_default}">
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
    </table>

{elseif $device_type=="moboard"}
	<h2>{t}Motherboard{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td>{t}Chipset{/t}
						</td>	
						<td>	
							<input type="text" name="chipset" value="{$chipset}">
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
{elseif $device_type=="case"}
	<h2>{t}Computer case{/t}</h2>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="designation" value="{$designation}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comment{/t}
						</td>	
						<td>	
							<textarea name="comment">{$comment}</textarea>
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
					<tr>
						<td>{t}Manufacturer{/t}
						</td>	
						<td>	
							<select name="FK_glpi_enterprise">
								{html_options values=$FK_glpi_enterpriseKeys output=$FK_glpi_enterprises selected=$FK_glpi_enterprise}
							</select>
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}format{/t}
						</td>	
						<td>	
							<select name="format">
								{html_options values=$formatKeys output=$formats selected=$format}
							</select>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>
{/if}


<p class="seperator">&nbsp;</p>
<div align="right">
	<p>
		<input type="submit" value="{t}Save{/t}" name="SaveDeviceChanges">
		<input type="submit" value="{t}Cancel{/t}" name="AbortDeviceChanges">
	</p> 
</div>
