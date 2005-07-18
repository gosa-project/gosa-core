<h2><img alt="" align="middle" src="images/select_phone.png"> {t}Specific Phone settings{/t}</h2>

<table summary="" width="100%" cellpadding=10>
	<tr>
		<td colspan="3">
			<table summary="" width="100%">
			<tr>
				<td>
					<LABEL for="selected_categorie">{t}Phone type{/t}</LABEL>{$staticAddress}
					<select id="selected_categorie" size="1" name="selected_categorie" title="{t}Choose a phone type{/t}" onchange="mainform.submit();">
						{html_options options=$categories selected=$selected_categorie}
					</select>
					<input type="submit" value="{t}refresh{/t}" name="refresh">
				</td>
			</table>
	</tr>

{if $selected_categorie eq '0'}
	
	<tr>
		<td>
			<table summary="">
			 <tr>
				<td>
					<LABEL for="goFonType">{t}Mode{/t}</LABEL>{$staticAddress}

				<td>
					<select id="goFonType" size="1" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
				</td>
			 </tr>
			 <tr>
				<td >
					<LABEL for="goFonDmtfMode">{t}DTMF mode{/t}</LABEL>
				</td>
				<td>
					<select size="1" id="goFonDmtfMode" name="goFonDmtfMode" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonDmtfModeUSED}>
						{html_options options=$goFonDmtfModes selected=$goFonDmtfMode}
					</select>
				</td>
			</tr>
		</table>
	   </td>
	   <td>
		<table summary="">
			<tr>
				<td >
					<LABEL for="goFonDefaultIP">{t}Default IP{/t}</LABEL>
				</td>
                <td>
                    <select id="goFonDefaultIP" size="1" name="goFonDefaultIP" title="{t}Choose a phone type{/t}" style="width:200px;" >
                        {html_options options=$goFonDefaultIPs selected=$goFonDefaultIP}
                    </select>
                   </td>
			</tr>
			<tr>
				<td >
					<LABEL for="goFonQualify">{t}Response timeout{/t}</LABEL>
				</td>
				<td>
					<input id="goFonQualify" style="width:200px" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
				</td>
			</tr>
			</table>
		</td>
		<td>
		</td>
	 </tr>
</table>
{/if}

{if $selected_categorie eq '1'}
		
	<tr>
		<td style="vertical-align:top">
			<table summary="" width="100%">
			 <tr>
				<td>
					<LABEL for="goFonType">{t}Modus{/t}{$staticAddress}</LABEL>
				</td>
				<td >
					<select size="1" id="goFonType" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
				</td>
			 </tr>
			<tr>
				<td >
					<LABEL for="goFonDefaultIP">{t}Default IP{/t}</LABEL>
				</td>
				<td>
					<input id="goFonDefaultIP" style="width:200px" name="goFonDefaultIP" value="{$goFonDefaultIP}" {$goFonDefaultIPUSED}>
				</td>
			</tr>
			<tr>
				<td >
					<LABEL for="goFonQualify">{t}Response timeout{/t}</LABEL>
				</td>
				<td>
					<input id="goFonQualify" style="width:200px" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
				</td>
			</tr>
			<tr>
				<td colspan=2>
					&nbsp;					
				</td>
			</tr>
			<tr>
				<td>
					<LABEL for="goFonAuth">{t}Authtype{/t}</LABEL>
				</td>
				<td>
					<select size="1" id="goFonAuth" name="goFonAuth" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonAuthUSED}>
						{html_options options=$goFonAuths selected=$goFonAuth}
					</select>
				</td>
			</tr>
			<tr>
				<td>	
					 <LABEL for="goFonSecret">{t}Secret{/t}</LABEL>
				</td>
				<td>
					<input id="goFonSecret" style="width:200px" name="goFonSecret" value="{$goFonSecret}" {$goFonSecretUSED}>
				</td>
			</tr>
<!--			<tr>
				<td>
					 {t}GoFonInkeys{/t}
				</td>
				<td>
					<input style="width:200px" name="goFonInkeys" value="{$goFonInkeys}" {$goFonInkeysUSED}>
				</td>
			</tr>
			<tr>
				<td>
					 {t}GoFonOutKeys{/t}
				</td>
				<td>
					<input style="width:200px" name="goFonOutkey" value="{$goFonOutkey}" {$goFonOutkeyUSED}>
				</td>
			</tr> -->
			<tr>
                <td colspan=2>
					&nbsp;
                </td>
            </tr>
            <tr>
                <td>
                    <LABEL for="goFonAccountCode">{t}Account code{/t}</LABEL>
                </td>
                <td>
                    <input id="goFonAccountCode" style="width:200px" name="goFonAccountCode" value="{$goFonAccountCode}" {$goFonAccountCodeUSED}>
                </td>
            </tr>
            <tr>
                <td>
                    <LABEL for="goFonTrunk">{t}Trunk lines{/t}</LABEL>
                </td>
                <td>
                     <select size="1" id="goFonTrunk" name="goFonTrunk" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTrunkUSED}>
                        {html_options options=$goFonTrunks selected=$goFonTrunk}
                     </select>
                 </td>
            </tr>

			</table>
		</td>
		<td valign="top" style="vertical-align:top">
			 <table summary="" width="100%">
               <tr>
                    <td style="vertical-align:top;">
                        <LABEL for="goFonPermitS">{t}Hosts that are allowed to connect{/t}</LABEL><br>
                        <select id="goFonPermitS" style="width:300px; height:80px;" name="goFonPermitS" size=15
                            multiple title="{t}List of alternative mail addresses{/t}">
                            {html_options values=$goFonPermit output=$goFonPermit}
                            <option disabled>&nbsp;</option>
                        </select>
                        <br>
                            <input name="goFonPermitNew" size=30 align="middle" maxlength="65" value="">
                        <input type=submit value="{t}Add{/t}" name="goFonPermitAdd">
                        <input type=submit value="{t}Delete{/t}" name="goFonPermitDel">
						<br><br>
                    </td>
                </tr>
				<tr>
                    <td style="vertical-align:top;">
             	        <LABEL for="goFonDenyS">{t}Hosts that are not allowed to connect{/t}</LABEL><br>
                        <select id="goFonDenyS" style="width:300px; height:80px;" name="goFonDenyS" size=15
                            multiple title="{t}List of alternative mail addresses{/t}">
                            {html_options values=$goFonDeny output=$goFonDeny}
                            <option disabled>&nbsp;</option>
                        </select>
                        <br>
                            <input name="goFonDenyNew" size=30 align="middle" maxlength="65" value="">
                        <input type=submit value="{t}Add{/t}" name="goFonDenyAdd">
                        <input type=submit value="{t}Delete{/t}" name="goFonDenyDel">
                    </td>
                </table>
    </tr>
</table>
{/if}

{if $selected_categorie eq '2'}
	<tr>
		<td>
			<table summary="" width="100%">
				<tr>
					<td>
						<LABEL for="goFonMSN">{t}MSN{/t}</LABEL>&nbsp;
						<input id="goFonMSN" style="width:200px" name="goFonMSN" value="{$goFonMSN}" {$goFonMSNUSED}>
					</td>
				</tr>
			</table>
		</td>
		<td>
			&nbsp;
		</td>
	</tr>
</table>
{/if}
