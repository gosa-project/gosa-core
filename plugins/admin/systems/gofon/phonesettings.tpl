<h2><img class="center" alt="" align="middle" src="images/select_phone.png"> {t}Advanced phone settings{/t}</h2>

<table summary="" style="width:100%" border=0>
	<tr>
		<td colspan="2">
		<LABEL for="selected_categorie">{t}Phone type{/t}</LABEL>{$staticAddress}
{render acl=$categorieACL}
		<select id="selected_categorie" size="1" name="selected_categorie" title="{t}Choose a phone type{/t}" onchange="mainform.submit();">
			{html_options options=$categories selected=$selected_categorie}
		</select>
{/render}
		{if $javascript eq 'false'}
			<input type="submit" value="{t}refresh{/t}" name="refresh">
		{/if}
		<br>
		<br>
	</td>
	</tr>
{if $selected_categorie eq '0'}
	<tr>
		<td style="width:50%; border-right:1px solid #A0A0A0">
			<table summary="" border=0>
			 <tr>
				<td>
					<LABEL for="goFonType">{t}Mode{/t}</LABEL>{$staticAddress}
					
				</td>
				<td>
{render acl=$goFonTypeACL}
					<select id="goFonType" size="1" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
{/render}
				</td>
			 </tr>
			 <tr>
				<td >
					<LABEL for="goFonDmtfMode">{t}DTMF mode{/t}</LABEL>
				</td>
				<td>
{render acl=$goFonDmtfModeACL}
					<select size="1" id="goFonDmtfMode" name="goFonDmtfMode" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonDmtfModeUSED}>
						{html_options options=$goFonDmtfModes selected=$goFonDmtfMode}
					</select>
{/render}
				</td>
			</tr>
		</table>
	   </td>
	   <td>
		<table summary="" border=0>
			<tr>
				<td >
					<LABEL for="goFonDefaultIP">{t}Default IP{/t}</LABEL>
				</td>
                <td>
{render acl=$goFonDefaultIPACL}
                    <select id="goFonDefaultIP" size="1" name="goFonDefaultIP" title="{t}Choose a phone type{/t}" style="width:200px;" >
                        {html_options options=$goFonDefaultIPs selected=$goFonDefaultIP}
                    </select>
{/render}
                   </td>
			</tr>
			<tr>
				<td >
					<LABEL for="goFonQualify">{t}Response timeout{/t}</LABEL>
				</td>
				<td>
{render acl=$goFonQualifyACL}
					<input id="goFonQualify" style="width:200px" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
{/render}
				</td>
			</tr>
			</table>
		</td>
	 </tr>
</table>
{/if}

{if $selected_categorie eq '1'}
		
	<tr>
		<td style="vertical-align:top; width:50%; border-right:1px solid #A0A0A0">
			<table summary="" width="100%">
			 <tr>
				<td>
					<LABEL for="goFonType">{t}Modus{/t}{$staticAddress}</LABEL>
				</td>
				<td >
{render acl=$goFonTypeACL}
					<select size="1" id="goFonType" name="goFonType" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonTypeUSED}>
						{html_options options=$goFonTypes selected=$goFonType}
					</select>
{/render}
				</td>
			 </tr>
			<tr>
				<td >
					<LABEL for="goFonDefaultIP">{t}Default IP{/t}</LABEL>
				</td>
				<td>
{render acl=$goFonDefaultIPACL}
					<input id="goFonDefaultIP" style="width:200px" name="goFonDefaultIP" value="{$goFonDefaultIP}" {$goFonDefaultIPUSED}>
{/render}
				</td>
			</tr>
			<tr>
				<td >
					<LABEL for="goFonQualify">{t}Response timeout{/t}</LABEL>
				</td>
				<td>
{render acl=$goFonQualifyACL}
					<input id="goFonQualify" style="width:200px" name="goFonQualify" value="{$goFonQualify}" {$goFonQualifyUSED}>
{/render}
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
{render acl=$goFonAuthACL}
					<select size="1" id="goFonAuth" name="goFonAuth" title="{t}Choose a phone type{/t}" style="width:200px;" {$goFonAuthUSED}>
						{html_options options=$goFonAuths selected=$goFonAuth}
					</select>
{/render}
				</td>
			</tr>
			<tr>
				<td>	
					 <LABEL for="goFonSecret">{t}Secret{/t}</LABEL>
				</td>
				<td>
{render acl=$goFonSecretACL}
					<input id="goFonSecret" style="width:200px" name="goFonSecret" value="{$goFonSecret}" {$goFonSecretUSED}>
{/render}
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
{render acl=$goFonAccountCodeACL}
                    <input id="goFonAccountCode" style="width:200px" name="goFonAccountCode" value="{$goFonAccountCode}" {$goFonAccountCodeUSED}>
{/render}
                </td>
            </tr>
            <tr>
                <td>
                    <LABEL for="goFonTrunk">{t}Trunk lines{/t}</LABEL>
                </td>
                <td>
{render acl=$goFonTrunkACL}
                     <select size="1" id="goFonTrunk" name="goFonTrunk" title="{t}Choose a phone type{/t}" {$goFonTrunkUSED}>
                        {html_options options=$goFonTrunks selected=$goFonTrunk}
                     </select>
{/render}
                 </td>
            </tr>

			</table>
		</td>
		<td valign="top" style="vertical-align:top">
			 <table summary="" width="100%">
               <tr>
                    <td style="vertical-align:top;">
                        <LABEL for="goFonPermitS">{t}Hosts that are allowed to connect{/t}</LABEL><br>
{render acl=$goFonPermitACL}
                        <select id="goFonPermitS" style="width:100%; height:80px;" name="goFonPermitS" size=15
                            multiple title="{t}List of alternative mail addresses{/t}">
                            {html_options values=$goFonPermit output=$goFonPermit}
                            <option disabled>&nbsp;</option>
                        </select>
{/render}
                        <br>
{render acl=$goFonPermitACL}
                            <input name="goFonPermitNew" size=30 align="middle" maxlength="65" value="">
{/render}
{render acl=$goFonPermitACL}
                        <input type=submit value="{t}Add{/t}" name="goFonPermitAdd">
{/render}
{render acl=$goFonPermitACL}
                        <input type=submit value="{t}Delete{/t}" name="goFonPermitDel">
{/render}
						<br><br>
                    </td>
                </tr>
				<tr>
                    <td style="vertical-align:top;">
             	        <LABEL for="goFonDenyS">{t}Hosts that are not allowed to connect{/t}</LABEL><br>
{render acl=$goFonDenyACL}
                        <select id="goFonDenyS" style="width:100%; height:80px;" name="goFonDenyS" size=15
                            multiple title="{t}List of alternative mail addresses{/t}">
                            {html_options values=$goFonDeny output=$goFonDeny}
                            <option disabled>&nbsp;</option>
                        </select>
{/render}
                        <br>
{render acl=$goFonDenyACL}
                            <input name="goFonDenyNew" size=30 align="middle" maxlength="65" value="">
{/render}
{render acl=$goFonDenyACL}
                        <input type=submit value="{t}Add{/t}" name="goFonDenyAdd">
{/render}
{render acl=$goFonDenyACL}
                        <input type=submit value="{t}Delete{/t}" name="goFonDenyDel">
{/render}
                    </td>
                </table>
    </tr>
</table>
{/if}

{if $selected_categorie eq '2'}
	<tr>
		<td style="width:50%">
			<table summary="" width="100%">
				<tr>
					<td>
						<LABEL for="goFonMSN">{t}MSN{/t}</LABEL>&nbsp;
{render acl=$goFonMSNACL}
						<input id="goFonMSN" style="width:200px" name="goFonMSN" value="{$goFonMSN}" {$goFonMSNUSED}>
{/render}
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
