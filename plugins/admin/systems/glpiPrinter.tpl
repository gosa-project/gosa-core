<table summary="" style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align: top;">
			<!--Upper left-->	
			
			<h2>{t}Generic{/t}</h2>
			<table summary="" cellpadding=5>
				<tr>
					<td width="150">{t}Printer type{/t}
					</td>
					<td>
						<select name="type" {$typeACL}>
							{html_options values=$PrinterTypeKeys output=$PrinterTypes selected=$type}
						</select>
						<input type="submit" value="{t}edit{/t}" name="edit_type" {$typeACL}>	
					</td>
				</tr>
				<tr>
					<td>{t}Manufacturer{/t}
					</td>
					<td>
						<select name="FK_glpi_enterprise" {$FK_glpi_enterpriseACL}>
							{html_options values=$ManufacturerKeys output=$Manufacturers selected=$FK_glpi_enterprise}
						</select>	
						<input type="submit" value="{t}edit{/t}" name="edit_manufacturer" {$FK_glpi_enterpriseACL}>	
					</td>
				</tr>
			</table>
			<p class="seperator" >&nbsp;</p>
			<h2>{t}Contacts{/t}</h2>
			<table summary="" cellpadding=5>
				<tr>
					<td>{t}Technical responsible{/t}&nbsp;
					</td>
					<td>
						<i>{$tech_num}&nbsp; </i>&nbsp;
						<input type="submit" value="{t}Choose{/t}" name="SelectTechPerson" {$tech_numACL}>
					</td>
				</tr>
				<tr>
					<td>
						{t}Contact person{/t}
					</td>
					<td>
						<i>{$contact_num}&nbsp; </i>&nbsp;
						<input {$contact_numACL} type="submit" name="SelectContactPerson" value="{t}Edit{/t}">
					</td>
				</tr>
			</table>
<br>
			<p class="seperator" >&nbsp;</p>
			<h2>{t}Information{/t}</h2>
			<table summary="" width="100%">
				<tr>
					<td width="100%">
						<textarea name="comments" style="width:100%;height:100px;" {$commentsACL}>{$comments}</textarea>
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160); vertical-align: top; padding-right: 5px;">
			<h2>{t}Supported interfaces{/t}</h2>
			<table summary="" width="100%">
				<tr>
					<td width="20">
						<input type="checkbox" name="flags_serial" {if $flags_serial=="1"} checked {/if} value="1" {$flags_serialACL}>
					</td>
					<td>
						{t}Serial{/t}
					</td>
				</tr>
				<tr>
					<td width="20">
						<input type="checkbox" name="flags_par" {if $flags_par=="1"} checked {/if} value="1" {$flags_parACL}>
					</td>
					<td>
						{t}Parallel{/t}
					</td>
				</tr>
				<tr>
					<td width="20">
						<input type="checkbox" name="flags_usb" {if $flags_usb=="1"} checked {/if} value="1" {$flags_usbACL}>
					</td>
					<td>
						{t}USB{/t}
					</td>
				</tr>
			</table>
			<input name="glpiPrinterFlagsPosted" value="1" type="hidden">
			<p class="seperator" >&nbsp;</p>
			<h2>{t}Attachments{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
						<select name="Attachments" {$AttachmentsACL} style="height:100px;width:100%;" multiple>
							{html_options values=$AttachmentKeys output=$Attachments}
						</select>
						<input name="AddAttachment" 	value="{t}Add{/t}" type="submit">	
						<input name="RemoveAttachment" 	value="{t}Remove{/t}" type="submit">	
					</td>
				</tr>
			</table>
			<p class="seperator" >&nbsp;</p>
			<h2>{t}Installed cartridges{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
						<select name="Cartridges" {$CartridgesACL} style="height:100px;width:100%;" multiple>
							{html_options values=$CartridgeKeys output=$Cartridges}
						</select>
						<input name="AddCartridge" 	value="{t}Add{/t}" type="submit">	
						<input name="RemoveCartridge" 	value="{t}Remove{/t}" type="submit">	
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
