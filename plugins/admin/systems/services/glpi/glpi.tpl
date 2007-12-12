<table summary="" style="width:100%;">
	<tr>
		<td style="width:50%;vertical-align:top;" >
			<!--Upper left-->	
			
			<h2>{t}Generic{/t}</h2>
			<table summary="" cellpadding=5>
				<tr>
					<td >{t}System type{/t}
					</td>
					<td>
{render acl=$typeACL}
						<select name="type">
							{html_options values=$SystemTypeKeys output=$SystemTypes selected=$type}
						</select>
{/render}
{render acl=$typeACL}
						<input type="submit" value="{t}edit{/t}" name="edit_type">	
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Operating system{/t}
					</td>
					<td>
{render acl=$osACL}
						<select name="os">
							{html_options values=$OSKeys output=$OSs selected=$os}
						</select>	
{/render}
{render acl=$osACL}
						<input type="submit" value="{t}edit{/t}" name="edit_os">	
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Manufacturer{/t}
					</td>
					<td>
{render acl=$FK_glpi_enterpriseACL}
						<select name="FK_glpi_enterprise">
							{html_options values=$ManufacturerKeys output=$Manufacturers selected=$FK_glpi_enterprise}
						</select>	
{/render}
{render acl=$FK_glpi_enterpriseACL}
						<input type="submit" value="{t}edit{/t}" name="edit_manufacturer">	
{/render}
					</td>
				</tr>
			<!--</table>
			<p class="seperator">&nbsp;</p>
			<h2>{t}Contacts{/t}</h2>
			<table summary="" width="100%">-->
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
				<tr>
					<td>{t}Technical responsible{/t}&nbsp;
					</td>
					<td>
						<i>{$tech_num}&nbsp; </i>&nbsp;
{render acl=$tech_numACL}
						<input type="submit" value="{t}Edit{/t}" name="SelectTechPerson">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160); vertical-align: top; padding-right: 5px;">
			<!--Upper right-->
			<h2>{t}Comment{/t}</h2>
			<table summary="" width="100%">
				<tr>
					<td valign="top" colspan="2">
{render acl=$commentsACL}
						<textarea name="comments" style="width:100%;height:180px;">{$comments}</textarea>
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
		<td style="width:50%;">
			<h2>{t}Installed devices{/t}</h2>
			<table summary="" width="100%">	
				<tr>
					<td>
{render acl=$DevicesACL}
						<select name="InstalledDevices" style="height:130px;width:100%;" multiple>
							{html_options values=$InstalledDeviceKeys output=$InstalledDevices}
						</select>
{/render}
{render acl=$DevicesACL}
						<input name="AddDevice" value="{t}Edit{/t}" type="submit">	
{/render}
					</td>
				</tr>
			</table>
<!--			<p class="seperator" >&nbsp;</p>
			<input type="submit" value="{t}Trading{/t}" name="Trading">	
			<input type="submit" value="{t}Software{/t}" name="Software">	
			<input type="submit" value="{t}Contracts{/t}" name="Contracts">	-->
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
						<input name="AddAttachment" 	value="{t}Add{/t}" type="submit">	
{/render}
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<input type="hidden" name="glpi_tpl_posted" value="1">
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('type');
  -->
</script><p style="text-align:right">
