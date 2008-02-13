<h2>{t}Generic{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="width:50%;vertical-align:top;border-right:1px	solid	#b0b0b0;">
			<table summary="">
				<tr>
					<td>{t}Zone name{/t}{$must}
					</td>
					<td><input type="text" name="zoneName" value="{$zoneName}" {if $NotNew | $Zone_is_used} disabled {/if}>
					</td>
				</tr>
				<tr>
					<td>{t}Network address{/t}{$must}
					</td>
					<td><input type="text" name="ReverseZone" value="{$ReverseZone}" {if $NotNew | $Zone_is_used} disabled {/if}>
					</td>
				</tr>
				<tr>
					<td>{t}Netmask{/t}
					</td>
					<td>
						<select name="NetworkClass" {if $NotNew | $Zone_is_used} disabled {/if}>
							{html_options options=$NetworkClasses selected=$NetworkClass }
						</select>
					</td>
				</tr>
				{if $Zone_is_used}
				<tr>
				   <td colspan="2">
					   <i>{t}Zone is in use, network settings can't be modified.{/t}</i>
				   </td>
				</tr>
				{/if}
			</table>
		</td>
		<td style="vertical-align:top;">
			<table summary="">
				<tr>
					<td>
						{t}Zone records{/t}
						<br>
						{if $AllowZoneEdit == false}
							<i>{t}Can't be edited because the zone wasn't saved right now.{/t}</i>
						{/if}
					</td>
					<td>
						<input type="submit" name="EditZoneEntries" value="{t}Edit{/t}" {if $AllowZoneEdit == false} disabled {/if}> 
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<h2>{t}SOA record{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="vertical-align:top;width:50%;border-right:1px	solid	#b0b0b0;">
			<table summary="">
				<tr>
					<td>{t}Primary dns server for this zone{/t}{$must}
					</td>
					<td><input type="text" name="sOAprimary" value="{$sOAprimary}">
					</td>
				</tr>
				<tr>
					<td>{t}Mail address{/t}{$must}
					</td>
					<td><input type="text" name="sOAmail" value="{$sOAmail}">
					</td>
				</tr>
				<tr>
					<td>{t}Serial number (automatically incremented){/t}{$must}
					</td>
					<td><input type="text" name="sOAserial" value="{$sOAserial}">
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<table summary="">
				<tr>
					<td>{t}Refresh{/t}{$must}
					</td>
					<td><input type="text" name="sOArefresh" value="{$sOArefresh}">
					</td>
				</tr>
				<tr>
					<td>{t}Retry{/t}{$must}
					</td>
					<td><input type="text" name="sOAretry" value="{$sOAretry}">
					</td>
				</tr>
				<tr>
					<td>{t}Expire{/t}{$must}
					</td>
					<td><input type="text" name="sOAexpire" value="{$sOAexpire}">
					</td>
				</tr>
				<tr>
					<td>{t}TTL{/t}{$must}
					</td>
					<td><input type="text" name="sOAttl" value="{$sOAttl}">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

<p class="seperator">&nbsp;</p>
<br>
<table summary="" width="100%">
	<tr>
		<td style="vertical-align:top;width:50%;border-right:1px	solid	#b0b0b0;">
			<h2>{t}MxRecords{/t}</h2>
			<table width="100%">	
				<tr>
					<td>
						{$Mxrecords}
						<input type="text" 		name="StrMXRecord" value="">
						<input type="submit" 	name="AddMXRecord" value="{t}Add{/t}">
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<h2>{t}Global zone records{/t}</h2>
			  {$records}
		</td>
	</tr>
</table>
<div style="text-algin:right;" align="right">
	<p>
		<input type="submit" name="SaveZoneChanges" value="{t}Save{/t}">
		<input type="submit" name="CancelZoneChanges" value="{t}Cancel{/t}">
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('zoneName');
  -->
</script>
