<h2>{t}Zone settings{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="width:50%;vertical-align:top;border-right:1px	solid	#b0b0b0;">
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
					<td><input type="text" name="ReverseZone" value="{$ReverseZone}">
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
<div style="text-algin:right;" align="right">
	<p>
		<input type="submit" name="SaveZoneChanges" value="{t}Save{/t}">
		<input type="submit" name="CancelZoneChanges" value="{t}Cancel{/t}">
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.zoneName.focus();
  -->
</script>
