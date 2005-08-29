{if $is_account ne 'true'}
	<img alt="" src="images/stop.png" align="middle">
	{t}The environment extension is currently disabled.{/t}
{else}
<input type="hidden" name="iamposted" value="1">
<table summary="{t}Environment managment settings{/t}" width="100%">
	<tr>
		<td valign="top" width="50%">
			<h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Profiles{/t}</h2>
			<table summary="{t}Profile managment{/t}">
				<tr>
					<td>
						<input type="checkbox" value="1" {$useProfileCHK} name="useProfile" {$useProfileACL} id="useProfile"
									onclick="changeState('gotoProfileServer');changeState('gotoProfileFlag_C');">
					</td>
					<td valign="top">
						<LABEL for="useProfile">{t}Use profile managment{/t}</LABEL>
					</td>
				</tr>
				<tr>
					<td>&nbsp;
					</td>
					<td>
						<table summary="{t}Profile server managment{/t}">
							<tr>
								<td>	
									<LABEL for="gotoProfileServer">{t}Profil server{/t}</LABEL>
								</td>
								<td>
									<select style="width:350px;" id="gotoProfileServer" 
											name="gotoProfileServer" {$gotoProfileServerACL} id="gotoProfileServer">
										{html_options values=$gotoProfileServerKeys output=$gotoProfileServers selected=$gotoProfileServer}
										<option disabled>&nbsp;</option>
									</select>
								</td>
							</tr>
							<tr>
								<td>	
									<LABEL for="gotoProfileFlag_C">{t}Use local caching{/t}</LABEL>
								</td>
								<td>
									<input type="checkbox" name="gotoProfileFlag_C" value="C" 
										id="gotoProfileFlag_C" {$gotoProfileFlag_CACL} {$gotoProfileFlag_CCHK}>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				</table>
				<table>
				<tr>
					<td>
						<LABEL for="gotoProfileQuota">{t}Quota{/t}</LABEL> 
					</td>
					<td>
						<input type="text" name="gotoProfileQuota" value="{$gotoProfileQuota}" {$gotoProfileQuotaACL} id="gotoProfileQuota">
					</td>
				</tr>
				<tr>
					<td>
						<LABEL for="gotoXResolution">{t}Resolution{/t}</LABEL>
					</td>
					<td>
						<select name="gotoXResolution" {$gotoXResolutionACL} id="gotoXResolution">
                        	{html_options values=$gotoXResolutions output=$gotoXResolutions selected=$gotoXResolution}
                            <option disabled>&nbsp;</option>
                    	</select>
					</td>
				</tr>
				<tr>
					<td>
						<input type="checkbox" name="gotoProfileFlag_L" id="gotoProfileFlag_L" value="L" {$gotoProfileFlagsACL} {$gotoProfileFlag_LCHK}>
					</td>
					<td>
						<LABEL for="gotoProfileFlag_L">{t}Resolution changeable on runtime {/t}</LABEL>
					</td>
				</tr>
			</table>

			<p class="seperator">&nbsp;</p>
            <h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Kiosk profile{/t}</h2>	
			<table summary="{t}Kiosk profile settings{/t}">
				<tr>
					<td>
						<LABEL for="gotoKioskProfile">{t}Kiosk profile{/t}</LABEL>
					</td>
					<td>
						<select name="gotoKioskProfile" {$gotoKioskProfileACL} id="gotoKioskProfile">
                        {html_options values=$gotoKioskProfiles output=$gotoKioskProfiles selected=$gotoKioskProfile}
                        <option disabled>&nbsp;</option>
					</td>
				</tr>
				<tr>
					<td>
						&nbsp;
					</td>
					<td>
						<input type="submit" name="KioskManagementDialog" value="{t}Profile management{/t}" {$gotoKioskProfileACL}>
					</td>
				</tr>
			</table>	
			<p class="seperator">&nbsp;</p>
			<h2><img alt="" src="images/banana.png" align="middle">&nbsp;<LABEL for="gotoLogonScript">{t}Logon scripts{/t}</LABEL></h2>
			<table summary="{t}Logon script management{/t}">
				<tr>
					<td>
			        	<select style="width:350px;" name="gotoLogonScript" multiple size=4 {$gotoLogonScriptACL} id="gotoLogonScript">
                        {html_options values=$gotoLogonScriptKeys output=$gotoLogonScripts }
                        <option disabled>&nbsp;</option>
						</select>
			
						<br>
						<input type="submit" {$gotoLogonScriptACL} name="gotoLogonScriptNew"		value="{t}Add{/t}">
						<input type="submit" {$gotoLogonScriptACL} name="gotoLogonScriptDel" 	value="{t}Delete{/t}"> 
						<input type="submit" {$gotoLogonScriptACL} name="gotoLogonScriptEdit" 	value="{t}Edit{/t}"> 
					</td>
				</tr>
			</table>	
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);"  valign="top">
			<h2><img alt="" src="images/banana.png" align="middle">&nbsp;<LABEL for="gotoShare">{t}Attach share{/t}</LABEL></h2>
			<table summary="">
				<tr>
					<td>
			        	<select style="width:350px;" name="gotoShare" multiple size=4 {$gotoShareACL} id="gotoShare">
                        {html_options values=$gotoShareKeys output=$gotoShares}
                        <option disabled>&nbsp;</option>
						</select>
						<br>
			        	<select name="gotoShareSelection" {$gotoShareACL}>
                        {html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
                        <option disabled>&nbsp;</option>
						</select>
						<input type="text"	 {$gotoShareACL} name="gotoShareMountPoint"	value="">
						<input type="submit" {$gotoShareACL} name="gotoShareAdd" 		value="+"> 
						<input type="submit" {$gotoShareACL} name="gotoShareDel" 		value="-"> 
					</td>
				</tr>
			</table>
			<p class="seperator">&nbsp;</p>
            <h2><img alt="" src="images/banana.png" align="middle">&nbsp;<LABEL for="gotoHotplugDevice">{t}Hotplug devices{/t}</LABEL></h2>	
			<table summary="{t}Hotplug device settings{/t}">
				<tr>
					<td>
						<select name="gotoHotplugDevice" {$gotoHotplugDeviceACL} size=4  style="width:350px;" id="gotoHotplugDevice">
                        {html_options values=$gotoHotplugDeviceKeys output=$gotoHotplugDevices}
                        <option disabled>&nbsp;</option>
					</td>
				</tr>
				<tr>
					<td>
						<input type="submit" {$gotoHotplugDeviceACL} name="gotoHotplugDeviceNew" value="{t}New{/t}">
						<input type="submit" {$gotoHotplugDeviceACL} name="gotoHotplugDeviceUse" value="{t}Existing{/t}">
						<input type="submit" {$gotoHotplugDeviceACL} name="gotoHotplugDeviceDel" value="{t}Delete{/t}">
					</td>
				</tr>
			</table>
		 	<p class="seperator">&nbsp;</p>
            <h2><img alt="" src="images/banana.png" align="middle">&nbsp;<LABEL for="gotoPrinter">{t}Printer{/t}</LABEL></h2>
            <table summary="{t}Printer settings{/t}">
                <tr>
                    <td>
                        <select style="width:350px;" name="gotoPrinter" multiple size=4 {$gotoPrinterACL} id="gotoPrinter">
                        {html_options values=$gotoPrinterKeys output=$gotoPrinters}
                        <option disabled>&nbsp;</option>
                        </select>
                        <br>
                        <input type="submit" {$gotoPrinterACL} name="gotoPrinterAdd"     value="{t}Add{/t}">
                        <input type="submit" {$gotoPrinterACL} name="gotoPrinterDel"     value="{t}Delete{/t}">
                        <input type="submit" {$gotoPrinterACL} name="gotoPrinterEdit"    value="{t}Admin Toggle{/t}">
                    </td>
                </tr>
            </table>
		</td>
	</tr>
</table>	
	
		


	

{/if}
