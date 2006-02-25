{if $device_type=="monitor"}
	<h2><img alt="" align="middle" src="images/monitor.png" class="center">&nbsp;{t}Add/Edit monitor{/t}</h2>
	<p class="seperator">&nbsp;</p>
	<br>
	<table summary="" width="100%">
		<tr>
			<td width="50%" style="vertical-align:top; border-right:1px solid #A0A0A0; padding-right:5px;">
				<table summary="" width="100%">
					<tr>
						<td>{t}Name{/t}
						</td>	
						<td>	
							<input name="name" value="{$name}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Comments{/t}
						</td>	
						<td>	
							<textarea name="comments">{$comments}</textarea>
						</td>
					</tr>
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
						<td style="vertical-align:top;">{t}Monitor size{/t}
						</td>	
						<td>	
							<input name="size" value="{$size}"> {t}Inch{/t}
						</td>
					</tr>
				</table>
			</td>
			<td>
				<table summary="" width="100%">
  					<tr>
                        <td>{t}Integrated microphone{/t}
                        </td>
                        <td>
                            <input type="radio" name="flags_micro" value="1" {if $flags_micro == "1"}checked {/if}>{t}Yes{/t}
                            <input type="radio" name="flags_micro" value="0" {if ($flags_micro == "0")||($flags_micro=="")}checked {/if}>{t}No{/t}
                        </td>
                    </tr>
                    <tr>
                        <td>{t}Integrated speakers{/t}
                        </td>
                        <td>
                            <input type="radio" name="flags_speaker" value="1" {if $flags_speaker == "1"}checked {/if}>{t}Yes{/t}
                            <input type="radio" name="flags_speaker" value="0" {if ($flags_speaker == "0")||($flags_speaker=="")}checked {/if}>{t}No{/t}
                        </td>
                    </tr>
                    <tr>
                        <td>{t}Sub-D{/t}
                        </td>
                        <td>
                            <input type="radio" name="flags_subd" value="1" {if $flags_subd == "1"}checked {/if}>{t}Yes{/t}
                            <input type="radio" name="flags_subd" value="0" {if ($flags_subd == "0")||($flags_subd=="")}checked {/if}>{t}No{/t}
                        </td>
                    </tr>
                    <tr>
                        <td>{t}BNC{/t}
                        </td>
                        <td>
                            <input type="radio" name="flags_bnc" value="1" {if $flags_bnc == "1"}checked {/if}>{t}Yes{/t}
                            <input type="radio" name="flags_bnc" value="0" {if ($flags_bnc == "0")||($flags_bnc=="")}checked {/if}>{t}No{/t}
                        </td>
                    </tr>
					<tr>
						<td style="vertical-align:top;">{t}Serial number{/t}
						</td>	
						<td>	
							<input name="serial" value="{$serial}">
						</td>
					</tr>
					<tr>
						<td style="vertical-align:top;">{t}Additional serial number{/t}
						</td>	
						<td>	
							<input name="otherserial" value="{$otherserial}">
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</table>


{elseif $device_type=="pci"}

        <h2><img alt="" align="middle" src="images/fai_template.png" class="center">&nbsp;{t}Add/Edit other device{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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

        <h2><img alt="" align="middle" src="images/power.png" class="center">&nbsp;{t}Add/Edit power supply{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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

        <h2><img alt="" align="middle" src="images/gfx_hardware.png" class="center">&nbsp;{t}Add/Edit graphic card{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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

        <h2><img alt="" align="middle" src="images/hardware.png" class="center">&nbsp;{t}Add/Edit controller{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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

        <h2><img alt="" align="middle" src="images/drives.png" class="center">&nbsp;{t}Add/Edit drive{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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
        <h2><img alt="" align="middle" src="images/fai_partitionTable.png" class="center">&nbsp;{t}Add/Edit harddisk{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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

        <h2><img alt="" align="middle" src="images/memory.png" class="center">&nbsp;{t}Add/Edit memory{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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
        <h2><img alt="" align="middle" src="images/snd_hardware.png" class="center">&nbsp;{t}Add/Edit sound card{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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
    <h2><img alt="" align="middle" src="images/net_hardware.png" class="center">&nbsp;{t}Add/Edit network interface{/t}</h2>
    <p class="seperator">&nbsp;</p>
    <br>

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
    <h2><img alt="" align="middle" src="images/processor.png" class="center">&nbsp;{t}Add/Edit processor{/t}</h2>
    <p class="seperator">&nbsp;</p>
    <br>
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
        <h2><img alt="" align="middle" src="images/mainboard.png" class="center">&nbsp;{t}Add/Edit motherboard{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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
        <h2><img alt="" align="middle" src="images/server.png" class="center">&nbsp;{t}Add/Edit computer case{/t}</h2>
        <p class="seperator">&nbsp;</p>
        <br>
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
