<table summary="" title="" style="width: 100%;">
	<tr>
		<td>
			<h2>{t}Generic{/t}</h2>
			<table summary="" title="" width="100%" style="vertical-align:top;">	
				<tr>
					<td>{t}Name{/t}
					</td>
					<td>
						<input type="text" name="name" value="{$name}">
					</td>
				</tr>
				<tr>
					<td>{t}Reference{/t}
					</td>
					<td>
						<input type="text" name="ref" value="{$ref}">
					</td>
				</tr>
			</table>
			<p class="seperator">&nbsp;</p>
			<h2>{t}Comments{/t}</h2>
			<table summary="" title="" width="100%" style="vertical-align:top;">	
				<tr>
					<td>{t}Comment{/t}
					</td>
					<td>
						<textarea name="comments" style="width:100%;">{$comments}</textarea>	
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;border-left: 1px solid rgb(160, 160, 160);padding-right: 5px;">
			<h2>{t}Generic{/t}</h2>
            <table summary="" title="" width="100%" style="vertical-align:top;">
                <tr>
                    <td>{t}Type{/t}
                    </td>
                    <td>
						<select name="type" >
                            {html_options values=$typeKeys output=$types selected=$type}
                        </select>
                        <input type="submit" value="{t}edit{/t}" name="edit_type_cartridge">
                    </td>
	    		</tr>
                <tr>
                    <td>{t}Manufacturer{/t}
                    </td>
                    <td>
						<select name="FK_glpi_enterprise">
                            {html_options values=$ManufacturerKeys output=$Manufacturers selected=$FK_glpi_enterprise}
                        </select>
                        <input type="submit" value="{t}edit{/t}" name="edit_manufacturer_cartridges">
                    </td>
	    		</tr>
				<tr>
                    <td>{t}Technical responsible{/t}&nbsp;
                    </td>
                    <td>
                        <i>{$tech_num}&nbsp; </i>&nbsp;
                        <input type="submit" value="{t}Choose{/t}" name="SelectCartridgeTechPerson">
                    </td>
                </tr>
			</table>
			<p class="seperator">&nbsp;</p>
			{$PrinterTypeMatrix}
		</td>
	</tr>
</table>

<p class="seperator">&nbsp;</p>
<div style="text-align:right;">
	<p>
		<input type="submit" name="SaveCartridge" 	value="{t}Save{/t}">&nbsp;
		<input type="submit" name="CancelCartridge" value="{t}Cancel{/t}"><br>
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('name');
  -->
</script>

