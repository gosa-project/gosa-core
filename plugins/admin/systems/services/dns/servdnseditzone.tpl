<h2>{t}Generic{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="width:50%;vertical-align:top;border-right:1px	solid	#b0b0b0;">
			<table summary="">
				<tr>
					<td>{t}Zone name{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="zoneName" value="{$zoneName}" {if $NotNew} disabled {/if}>
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Network address{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="ReverseZone" value="{$ReverseZone}" {if $NotNew} disabled {/if}>
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Netmask{/t}
					</td>
					<td>
{render acl=$ACLs}					
						<select name="NetworkClass" {if $NotNew} disabled {/if}>
							{html_options options=$NetworkClasses selected=$NetworkClass }
						</select>
{/render}
					</td>
				</tr>
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
{render acl=$ACLs}					
						<input type="submit" name="EditZoneEntries" value="{t}Edit{/t}" {if $AllowZoneEdit == false} disabled {/if}> 
{/render}
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
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOAprimary" value="{$sOAprimary}">
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Mail address{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOAmail" value="{$sOAmail}">
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Serial number (automatically incremented){/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOAserial" value="{$sOAserial}">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<table summary="">
				<tr>
					<td>{t}Refresh{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOArefresh" value="{$sOArefresh}">
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Retry{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOAretry" value="{$sOAretry}">
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}Expire{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOAexpire" value="{$sOAexpire}">
{/render}
					</td>
				</tr>
				<tr>
					<td>{t}TTL{/t}{$must}
					</td>
					<td>
{render acl=$ACLs}					
						<input type="text" name="sOAttl" value="{$sOAttl}">
{/render}
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
{render acl=$ACLs}					
						{$Mxrecords}
{/render}
{render acl=$ACLs}					
						<input type="text" 		name="StrMXRecord" value="">
{/render}
{render acl=$ACLs}					
						<input type="submit" 	name="AddMXRecord" value="{t}Add{/t}">
{/render}
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<h2>{t}Global zone records{/t}</h2>
{render acl=$ACLs}					
			  {$records}
{/render}
		</td>
	</tr>
</table>
<div style="text-algin:right;" align="right">
	<p>
{render acl=$ACLs}					
		<input type="submit" name="SaveZoneChanges" value="{t}Save{/t}">
{/render}
		<input type="submit" name="CancelZoneChanges" value="{t}Cancel{/t}">
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('zoneName');
  -->
</script>
