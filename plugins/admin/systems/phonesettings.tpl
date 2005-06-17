<h2><img alt="" align="middle" src="images/select_phone.png"> {t}Specific Phone settings{/t}</h2>

<table width="100%" cellpadding=10>
	<tr>
		<td width="40%" colspan="3">
			<table width="100%">
			<tr>
				<td>
					{t}Phone type{/t}{$staticAddress}
					<select size="1" name="selected_categorie" title="{t}Choose a phone type{/t}" onchange="mainform.submit();">
						{html_options options=$categories selected=$selected_categorie}
					</select>
					<input type="submit" value="{t}refresh{/t}" name="refresh">
				</td>
			</table>
	</tr>

{if $selected_categorie eq '0'}
	
	<tr>
		<td>
			<table>
			 <tr>
				<td>
					{t}Mode{/t}{$staticAddress}
				</td>
				<td>
					<select size="1" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
				</td>
			 </tr>
			 <tr>
				<td >
					{t}DTMF mode{/t}
				</td>
				<td>
					<select size="1" name="goFonDmtfMode" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonDmtfModeUSED}>
						{html_options options=$goFonDmtfModes selected=$goFonDmtfMode}
					</select>
				</td>
			</tr>
		</table>
	   </td>
	   <td>
		<table>
			<tr>
				<td >
					{t}Default IP{/t}
				</td>
				<td>
					<input style="width:200px" name="goFonDefaultIP" value="{$goFonDefaultIP}" {$goFonDefaultIPUSED}>
				</td>
			</tr>
			<tr>
				<td >
					{t}Response timeout{/t}
				</td>
				<td>
					<input style="width:200px" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
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
			<table width="100%">
			 <tr>
				<td width="25%">
					{t}Modus{/t}{$staticAddress}
				</td>
				<td >
					<select size="1" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
				</td>
			 </tr>
			<tr>
				<td >
					{t}Default IP{/t}
				</td>
				<td>
					<input style="width:200px" name="goFonDefaultIP" value="{$goFonDefaultIP}" {$goFonDefaultIPUSED}>
				</td>
			</tr>
			<tr>
				<td >
					{t}Response timeout{/t}
				</td>
				<td>
					<input style="width:200px" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
				</td>
			</tr>
			<tr>
				<td colspan=2>
					&nbsp;					
				</td>
			</tr>
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
					 {t}Secret{/t}	
				</td>
				<td>
					<input style="width:200px" name="goFonSecret" value="{$goFonSecret}" {$goFonSecretUSED}>
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
                    {t}Account code{/t}
                </td>
                <td>
                    <input style="width:200px" name="goFonAccountCode" value="{$goFonAccountCode}" {$goFonAccountCodeUSED}>
                </td>
            </tr>
            <tr>
                <td width="25%">
                    {t}Trunk lines{/t}
                </td>
                <td>
                     <select size="1" name="goFonTrunk" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTrunkUSED}>
                        {html_options options=$goFonTrunks selected=$goFonTrunk}
                     </select>
                 </td>
            </tr>

			</table>
		</td>
		<td valign="top" style="vertical-align:top">
			 <table width="100%">
               <tr>
                    <td style="vertical-align:top;">
                        {t}Hosts that are allowed to connect{/t}<br>
                        <select style="width:300px; height:80px;" name="goFonPermitS" size=15
                            multiple title="{t}List of alternative mail addresses{/t}">
                            {html_options values=$goFonPermit output=$goFonPermit}
                            <option disabled></option>
                        </select>
                        <br>
                            <input name="goFonPermitNew" size=30 align="middle" maxlength="65" value="">
                        <input type=submit value="{t}Add{/t}" name="goFonPermitAdd"
                        <input type=submit value="{t}Delete{/t}" name="goFonPermitDel"
						<br><br>
                    </td>
                </tr>
				<tr>
                    <td style="vertical-align:top;">
                        {t}Hosts that are not allowed to connect{/t}<br>
                        <select style="width:300px; height:80px;" name="goFonDenyS" size=15
                            multiple title="{t}List of alternative mail addresses{/t}">
                            {html_options values=$goFonDeny output=$goFonDeny}
                            <option disabled></option>
                        </select>
                        <br>
                            <input name="goFonDenyNew" size=30 align="middle" maxlength="65" value="">
                        <input type=submit value="{t}Add{/t}" name="goFonDenyAdd"
                        <input type=submit value="{t}Delete{/t}" name="goFonDenyDel"
                    </td>
                </table>
		</td>
    </td>
    </tr>
</table>
{/if}

{if $selected_categorie eq '2'}
	<tr>
		<td>
			<table width="100%">
				<tr>
					<td width="25%">
						{t}MSN{/t}&nbsp;
						<input style="width:200px" name="goFonMSN" value="{$goFonMSN}" {$goFonMSNUSED}>
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
