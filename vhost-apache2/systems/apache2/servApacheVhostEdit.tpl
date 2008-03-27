<h2>{t}Generic{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td style="width:50%;vertical-align:top;border-right:1px	solid	#b0b0b0;">
			<table summary="">
				<tr>
					<td>{t}Virtual Host Name{/t}{$must}
					</td>
					<td><input type="text" name="apacheServerName" value="{$apacheServerName}" {if $NotNew} disabled {/if}>
					</td>
				</tr>
				<tr>
					<td>{t}Virtual Host Document Root{/t}{$must}
					</td>
					<td><input type="text" name="apacheDocumentRoot" value="{$apacheDocumentRoot}">
					</td>
				</tr>
				<tr>
					<td>{t}Admin Mail address{/t}{$must}
					</td>
					<td><input type="text" name="apacheServerAdmin" value="{$apacheServerAdmin}">
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
			<table summary="" width="100%">
				<tr>
					<td style="vertical-align:top;width:100%;border-right:1px	solid	#b0b0b0;">
						<h2>{t}Server Alias{/t}</h2>
						<table width="100%">	
							<tr>
								<td>
									{$apacheServerAlias}
								</td>
							</tr>
							<tr>
								<td>
									<table width="100%">
										<tr>
											<td  style="vertical-align:top;width:30%;">
												<h2>{t}URL Alias{/t}</h2>
											</td>
											<td>
												<h2>{t}Directory Path{/t}</h2>
											</td>
										</tr>
										<tr>
											<td style="vertical-align:top;width:30%;">
												<input type="text" 		name="StrSAAlias" value="">
											</td>
											<td>
												<input type="text" 		name="StrSADir" value="">
												<input type="submit" 	name="AddSARecord" value="{t}Add{/t}">
											</td>
										</tr>
									</table>
								</td>
							</tr>
						</table>
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
			<h2>{t}Script Alias{/t}</h2>
			<table width="100%">	
				<tr>
					<td>
						{$apacheScriptAlias}
					</td>
				</tr>
				<tr>
					<td>
						<table width="100%">
							<tr>
								<td  style="vertical-align:top;width:30%;">
									<h2>{t}Alias Directory{/t}</h2>
								</td>
								<td>
									<h2>{t}Script Directory{/t}</h2>
								</td>
							</tr>
							<tr>
								<td style="vertical-align:top;width:30%;">
									<input type="text" 		name="StrSCAlias" value="">
								</td>
								<td>
									<input type="text" 		name="StrSCDir" value="">
									<input type="submit" 	name="AddSCRecord" value="{t}Add{/t}">
								</td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</td>
		<td style="vertical-align:top;">
		</td>
	</tr>
</table>
<div style="text-align:right;" align="right">
	<p>
		<input type="submit" name="SaveVhostChanges" value="{t}Save{/t}">
		<input type="submit" name="CancelVhostChanges" value="{t}Cancel{/t}">
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.apacheServerName.focus();
  -->
</script>
