<table summary="" style="width:100%;">
	<tr>
		<td style="width:50%;">
			<!--Upper left-->	
			
			<h2>{t}Generic{/t}</h2>
			<table summary="">
				<tr>
					<td width="150">{t}System type{/t}
					</td>
					<td>
						<select name="type" {$typeACL}>
							{html_options values=$SystemTypeKeys output=$SystemTypes selected=$type}
						</select>
						<input type="submit" value="{t}edit{/t}" name="edit_type">	
					</td>
				</tr>
				<tr>
					<td>{t}Operating system{/t}
					</td>
					<td>
						<select name="os" {$osACL}>
							{html_options values=$OSKeys output=$OSs selected=$os}
						</select>	
						<input type="submit" value="{t}edit{/t}" name="edit_os">	
					</td>
				</tr>
				<tr>
					<td>{t}Manufacturer{/t}
					</td>
					<td>
						<select name="FK_glpi_enterprise" {$FK_glpi_enterpriseACL}>
							{html_options values=$ManufacturerKeys output=$Manufacturers selected=$FK_glpi_enterprise}
						</select>	
						<input type="submit" value="{t}edit{/t}" name="edit_manufacturer">	
					</td>
				</tr>
				<tr>
					<td>{t}Technical responsible{/t}&nbsp;
					</td>
					<td>
						<i>{$tech_num}</i>&nbsp;<input type="submit" value="{t}Choose{/t}" name="SelectTechPerson">
					</td>
				</tr>
			</table>
			<p class="seperator" >&nbsp;</p>
			<h2>{t}Installed devices{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
						<select name="InstalledDevices" {$InstalledDevicesACL} style="height:120px;width:100%;" multiple>
							{html_options values=$InstalledDevices output=$InstalledDevices}
						</select>
						<input name="AddDevice" value="{t}Add{/t}" type="submit">	
					</td>
				</tr>
			</table>
			<p class="seperator" >&nbsp;</p>
			<input type="submit" value="{t}Trading{/t}" name="Trading">	
			<input type="submit" value="{t}Software{/t}" name="Software">	
			<input type="submit" value="{t}Contracts{/t}" name="Contracts">	
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160); vertical-align: top; padding-right: 5px;">
			<!--Upper right-->
			<h2>{t}Contacts{/t}</h2>
			<table summary="" width="100%">
				<tr>
					<td>
						{t}Contact person{/t}
					</td>
					<td>
						contact person sdasd <input type="submit" value="{t}Edit{/t}"><img src="images/mailto.png" class="center" alt="{t}mailto{/t}">	
					</td>
				</tr>
				<tr>
					<td>
						{t}Comments{/t}
					</td>
					<td>
						<textarea name="comments" style="width:100%;height:110px;" >{$comments}</textarea>
					</td>
				</tr>
			</table>
			<p class="seperator" >&nbsp;</p>
			<h2>{t}Attachments{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
						<select name="Attachments" {$AttachmentsACL} style="height:120px;width:100%;" multiple>
							{html_options values=$Attachments output=$Attachments}
						</select>
						<input name="AddDevice" value="{t}Add{/t}" type="submit">	
					</td>
				</tr>
			</table>
			
		</td>
	</tr>
</table>
