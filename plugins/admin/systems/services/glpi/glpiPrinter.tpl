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
{render acl=$typeACL}
						<select name="type" >
							{html_options values=$PrinterTypeKeys output=$PrinterTypes selected=$type}
						</select>
{/render}
{render acl=$typeACL}
						<input type="submit" value="{t}edit{/t}" name="edit_type" >	
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Manufacturer{/t}
					</td>
					<td>
{render acl=$FKglpienterpriseACL}
						<select name="FK_glpi_enterprise" >
							{html_options values=$ManufacturerKeys output=$Manufacturers selected=$FK_glpi_enterprise}
						</select>	
{/render}
{render acl=$FKglpienterpriseACL}
						<input type="submit" value="{t}edit{/t}" name="edit_manufacturer" >	
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160); vertical-align: top; padding-right: 5px;">
			<h2>{t}Supported interfaces{/t}</h2>
			<table summary="" width="100%">
				<tr>
					<td width="20">
{render acl=$flags_serialACL}
						<input type="checkbox" name="flags_serial" {if $flags_serial=="1"} checked {/if} value="1" >
{/render}
					</td>
					<td>
						{t}Serial{/t}
					</td>
				</tr>
				<tr>
					<td width="20">
{render acl=$flags_parACL}
						<input type="checkbox" name="flags_par" {if $flags_par=="1"} checked {/if} value="1" >
{/render}
					</td>
					<td>
						{t}Parallel{/t}
					</td>
				</tr>
				<tr>
					<td width="20">
{render acl=$flags_usbACL}
						<input type="checkbox" name="flags_usb" {if $flags_usb=="1"} checked {/if} value="1" >
{/render}
					</td>
					<td>
						{t}USB{/t}
					</td>
				</tr>
			</table>
			<input name="glpiPrinterFlagsPosted" value="1" type="hidden">
		</td>
	</tr>
</table>
<p class="seperator" >&nbsp;</p>
<table summary="" style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align: top;">
			<h2>{t}Contacts{/t}</h2>
			<table summary="" cellpadding=5>
				<tr>
					<td>{t}Technical responsible{/t}&nbsp;
					</td>
					<td>
						<i>{$tech_num}&nbsp; </i>&nbsp;
{render acl=$tech_numACL}
						<input type="submit" value="{t}Edit{/t}" name="SelectTechPerson" >
{/render}
					</td>
				</tr>
				<tr>
					<td>
						{t}Contact person{/t}
					</td>
					<td>
						<i>{$contact_num}&nbsp; </i>&nbsp;
{render acl=$contact_numACL}
						<input type="submit" name="SelectContactPerson" value="{t}Edit{/t}">
{/render}

					</td>
				</tr>
			</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160); vertical-align: top; padding-right: 5px;">
			<h2>{t}Attachments{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
{render acl=$AttachmentsACL}
						{$AttachmentsDiv}
{/render}
{render acl=$AttachmentsACL}
						<input name="AddAttachment" 	value="{t}Add{/t}" type="submit" >	
{/render}
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<p class="seperator" >&nbsp;</p>
<table summary="" style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align: top;">
			<h2>{t}Information{/t}</h2>
			<table summary="" width="100%">
				<tr>
					<td width="100%">
{render acl=$commentsACL}
						<textarea name="comments" style="width:100%;height:100px;" >{$comments}</textarea>
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160); vertical-align: top; padding-right: 5px;">
			<h2>{t}Installed cartridges{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
{render acl=$CartridgesACL}
						<select name="Cartridges[]"  style="height:100px;width:100%;" multiple>
							{html_options values=$CartridgeKeys output=$Cartridges}
						</select>
{/render}
{render acl=$CartridgesACL}
						<input name="AddCartridge" 	value="{t}Add{/t}" type="submit" >	
{/render}
{render acl=$CartridgesACL}
						<input name="RemoveCartridge" 	value="{t}Remove{/t}" type="submit" >	
{/render}
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
