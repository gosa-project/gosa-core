<h2><img alt="" align="middle" src="images/select_phone.png"> {t}Specific Phone settings{/t}</h2>

<table width="100%" cellpadding=10>
	<tr>
		<td colspan>
			<table width="100%">
			<tr>
				<td width="25%">
					{t}Phone category{/t}{$staticAddress}
				</td>
				<td >
					<select size="1" name="selected_categorie" title="{t}Choose a phone type{/t}" style="width:200px;" onchange="mainform.submit();">
						{html_options options=$categories selected=$selected_categorie}
					</select>
					<input type="submit" value="{t}refresh{/t}" name="refresh">
				</td>
			 </tr>
			</table>
		</td>
	</tr>
	<tr>
		<td>
			<table width="100%">
			 <tr>
				<td width="25%">
					{t}Phone type{/t}{$staticAddress}
				</td>
				<td >
					<select size="1" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
				</td>
			 </tr>
			 <tr>
				<td >
					{t}Dtmfmode What ever ...{/t}
				</td>
				<td>
					<select size="1" name="goFonDmtfMode" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonDmtfModeUSED}>
						{html_options options=$goFonDmtfModes selected=$goFonDmtfMode}
					</select>
				</td>
			</tr>
			<tr>
				<td >
					{t}Default IP{/t}
				</td>
				<td>
					<input width="200" name="goFonDefaultIP" value="{$goFonDefaultIP}" {$goFonDefaultIPUSED}>
				</td>
			</tr>
			<tr>
				<td >
					{t}Quality{/t}
				</td>
				<td>
					<input width="200" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
				</td>
			</tr>
			</table>
		</td>
		<td valign="top">
			<table width="100%">
				<tr>
					<td width="25%">
						{t}Authtype{/t}
					</td>
					<td>
						<select size="1" name="goFonAuth" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonAuthUSED}>
							{html_options options=$goFonAuths selected=$goFonAuth}
						</select>
					</td>
				</tr>
				<tr>
					<td>	
						 {t}GoFonSecret{/t}	
					</td>
					<td>
						<input width="200" name="goFonSecret" value="{$goFonSecret}" {$goFonSecretUSED}>
					</td>
				</tr>
				<tr>
					<td>
						 {t}GoFonInkeys{/t}
					</td>
					<td>
						<input width="200" name="goFonInkeys" value="{$goFonInkeys}" {$goFonInkeysUSED}>
					</td>
				</tr>
				<tr>
					<td>
						 {t}GoFonOutKeys{/t}
					</td>
					<td>
						<input width="200" name="goFonOutkey" value="{$goFonOutkey}" {$goFonOutkeyUSED}>
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
	<td>
		<table width="100%">
			<tr>
				<td width="25%">
					{t}permit / deny {/t}
				</td>
				<td>
					<input name="notsetyet">
				</td>
			</tr>
			<tr>
				<td>
					{t}AccountCode {/t}
        </td>
        <td>
					<input width="200" name="goFonAccountCode" value="{$goFonAccountCode}" {$goFonAccountCodeUSED}>
				</td>
			</tr>
		</table>
	</td>
		<td>
			<table width="100%">
				<tr>
					<td width="25%">
						{t}Trunk several calls{/t}
					</td>
					<td>
						<select size="1" name="goFonTrunk" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTrunkUSED}>
							{html_options options=$goFonTrunks selected=$goFonTrunk}
						</select>
					</td>
				</tr>
				<tr>
					<td>
						{t}MSN{/t}
					</td>
					<td>
						<input width="200" name="goFonMSN" value="{$goFonMSN}" {$goFonMSNUSED}>
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
