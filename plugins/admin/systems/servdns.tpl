{if $editMode == false}
<h2>{t}Zones{/t}</h2>
<table summary="" width="100%">
<tr>
	<td style="width:100%;vertical-align:top;">
		{$ZoneList}
		<input type="submit" name="AddZone" value="{t}Add{/t}">
		<input type="submit" name="EditZone" value="{t}Edit{/t}">
		<input type="submit" name="RemoveZone" value="{t}Remove{/t}">
	</td>
</tr>
</table>
{else}
<h2>{t}Zone settings{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="width:50%;vertical-align:top;">
			<table summary="">
				<tr>
					<td>{t}Zone name{/t}
					</td>
					<td><input type="text" name="zoneName" value="{$zoneName}">
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<table summary="">
				<tr>
					<td>{t}Subnet{/t}
					</td>
					<td><input type="text" name="subNet" value="">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<h2>{t}SOA record for this zone{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="vertical-align:top;width:50%">
			<table summary="">
				<tr>
					<td>{t}Primary dns server for this zone{/t}
					</td>
					<td><input type="text" name="sOAprimary" value="{$sOAprimary}">
					</td>
				</tr>
				<tr>
					<td>{t}Mail address{/t}
					</td>
					<td><input type="text" name="sOAmail" value="{$sOAmail}">
					</td>
				</tr>
				<tr>
					<td>{t}Serialnumber (automatically generated){/t}
					</td>
					<td><input type="text" name="sOAserial" value="{$sOAserial}">
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<table summary="">
				<tr>
					<td>{t}Refresh{/t}
					</td>
					<td><input type="text" name="sOArefresh" value="{$sOArefresh}">
					</td>
				</tr>
				<tr>
					<td>{t}Retry{/t}
					</td>
					<td><input type="text" name="sOAretry" value="{$sOAretry}">
					</td>
				</tr>
				<tr>
					<td>{t}Expire{/t}
					</td>
					<td><input type="text" name="sOAexpire" value="{$sOAexpire}">
					</td>
				</tr>
				<tr>
					<td>{t}TTL{/t}
					</td>
					<td><input type="text" name="sOAttl" value="{$sOAttl}">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

<p class="seperator">&nbsp;</p>
<table summary="" width="100%">
	<tr>
		<td style="vertical-align:top;width:50%;">
			<h2>{t}Parameter{/t}</h2>
			<table summary="">
				<tr>
					<td>{t}DNS TTL{/t}
					</td>
					<td>
						<input type="text" name="dNSTTL" value="{$dNSTTL}">
					</td>
				</tr>
				<tr>
					<td>{t}DNS Class{/t}
					</td>
					<td>
						<input type="text" name="dNSClass" value="{$dNSClass}">
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<h2>{t}Records{/t}</h2>
			<table summary="">
			  {$records}
			</table>
		</td>
	</tr>
</table>
{/if}
