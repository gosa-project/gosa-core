<table summary="" style="width:100%;">
	<tr>
		<td style="width:50%;">
			<!--Upper left-->	
			
			<h2>{t}Generic{/t}</h2>
			<table summary="">
				<tr>
					<td>{t}System type{/t}
					</td>
					<td>
						<select name="SystemType" {$SystemTypeACL}>
							{html_options values=$SystemTypes output=$SystemTypes selected=$SystemType}
						</select>	
					</td>
				</tr>
				<tr>
					<td>{t}Operating system{/t}
					</td>
					<td>
						<select name="OS" {$OSACL}>
							{html_options values=$OSs output=$OSs selected=$OS}
						</select>	
					</td>
				</tr>
				<tr>
					<td>{t}Manufacturer{/t}
					</td>
					<td>
						<select name="Manufacturer" {$ManufacturerACL}>
							{html_options values=$Manufacturers output=$Manufacturers selected=$Manufacturer}
						</select>	
					</td>
				</tr>
				<tr>
					<td>{t}Technicalresposible{/t}
					</td>
					<td>
						<select name="TechnicalResponsible" {$TechnicalResponsibleACL}>
							{html_options values=$TechnicalResponsibles output=$TechnicalResponsibles selected=$TechnicalResponsible}
						</select>	
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
						<textarea name="Comments" style="width:100%;height:110px;" ></textarea>
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
