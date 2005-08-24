{if $is_account ne 'true'}
	<img alt="" src="images/stop.png" align="middle">
	{t}The environment extension is currently disabled.{/t}
{else}
<table summary="" width="100%">
	<tr>
		<td valign="top" width="50%">
			<h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Profiles{/t}</h2>
			<table summary="">
				<tr>
					<td>
						<input type="checkbox" value="1" {$useProfileCHK} name="useProfile">
					</td>
					<td valign="top">
						{t}Use profile managment{/t}
					</td>
				</tr>
				<tr>
					<td>&nbsp;
					</td>
					<td>
						<table summary="">
							<tr>
								<td>	
									{t}Profil server{/t}
								</td>
								<td>
									<select style="width:350px;" name="goProfileServer">
										{html_options values=$goProfileServers output=$goProfileServer}
										<option disabled>&nbsp;</option>
									</select>
								</td>
							</tr>
							<tr>
								<td>	
									{t}Use local caching{/t}
								</td>
								<td>
									<input type="checkbox" name="gotoProfileFlag_C" value="C" {$gotoProfileFlagsACL} {$gotoProfileFlag_CCHK}>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				</table>
				<table>
				<tr>
					<td>{t}Quota{/t} 
					</td>
					<td>
						<input type="text" name="gotoIDon'tKnow">
					</td>
				</tr>
				<tr>
					<td>
						{t}Resolution{/t}
					</td>
					<td>
						<select name="gotoXResolution" {$gotoXResolutionACL}>
                        	{html_options values=$gotoXResolutions output=$gotoXResolutions}
                            <option disabled>&nbsp;</option>
                    	</select>
					</td>
				</tr>
				<tr>
					<td>
						<input type="checkbox" name="gotoProfileFlag_L" {$gotoProfileFlagsACL} {$gotoProfileFlag_LCHK}>
					</td>
					<td>
						{t}Resolution changeable on runtime {/t}
					</td>
				</tr>
			</table>

			<p class="seperator">&nbsp;</p>
            <h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Kiosk profile{/t}</h2>	
			<table summary="">
				<tr>
					<td>
						{t}Kiosk profile{/t}	
					</td>
					<td>
						<select name="gotoKioskProfile" {$gotoKioskProfileACL}>
                        {html_options values=$gotoKioskProfiles output=$gotoKioskProfiles}
                        <option disabled>&nbsp;</option>
					</td>
				</tr>
				<tr>
					<td>
						&nbsp;
					</td>
					<td>
						<input type="submit" name="KioskManagementDialog" value="{t}Profile management{/t}">
					</td>
				</tr>
			</table>	
			<p class="seperator">&nbsp;</p>
			<h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Login scripts{/t}</h2>
			<table summary="">
				<tr>
					<td>
			        	<select style="width:350px;" name="gotoLoginScript" multiple size=4 {$gotoLoginScriptACL}>
                        {html_options values=$gotoLoginScripts output=$gotoLoginScripts}
                        <option disabled>&nbsp;</option>
						</select>
			
						<br>
						<input type="submit" {gotoLoginScriptACL} name="gotoLoginScriptNew"		value="{t}Add{/t}">
						<input type="submit" {gotoLoginScriptACL} name="gotoLoginScriptDel" 	value="{t}Delete{/t}"> 
						<input type="submit" {gotoLoginScriptACL} name="gotoLoginScriptEditl" 	value="{t}Edit{/t}"> 
					</td>
				</tr>
			</table>	
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);"  valign="top">
			<h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Attach share{/t}</h2>
			<table summary="">
				<tr>
					<td>
			        	<select style="width:350px;" name="gotoShare" multiple size=4 {$gotoShareACL}>
                        {html_options values=$gotoShares output=$gotoShares}
                        <option disabled>&nbsp;</option>
						</select>
			
						<br>
							
			        	<select name="gotoShareSelection" {$gotoShareACL}>
                        {html_options values=$gotoShareSelections output=$gotoShareSelections}
                        <option disabled>&nbsp;</option>
						</select>
						<input type="text"	 {$gotoShareACL} name="gotoShareMountPoint"	value="">
						<input type="submit" {$gotoShareACL} name="gotoShareAdd" 		value="+"> 
						<input type="submit" {$gotoShareACL} name="gotoShareDel" 		value="-"> 
					</td>
				</tr>
			</table>
			<p class="seperator">&nbsp;</p>
            <h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Hotplug devices{/t}</h2>	
			<table summary="">
				<tr>
					<td>
						<select name="gotoHotplugDevice" {$gotoHotplugDeviceACL} size=4  style="width:350px;">
                        {html_options values=$gotoHotplugDevices output=$gotoHotplugDevices}
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
            <h2><img alt="" src="images/banana.png" align="middle">&nbsp;{t}Printer{/t}</h2>
            <table summary="">
                <tr>
                    <td>
                        <select style="width:350px;" name="gotoPrinter" multiple size=4 {$gotoPrinterACL}>
                        {html_options values=$gotoPrinters output=$gotoPrinters}
                        <option disabled>&nbsp;</option>
                        </select>

                        <br>
                        <input type="submit" {$gotoLoginScriptACL} name="gotoLoginScriptNew"     value="{t}Add{/t}">
                        <input type="submit" {$gotoLoginScriptACL} name="gotoLoginScriptDel"     value="{t}Delete{/t}">
                        <input type="submit" {$gotoLoginScriptACL} name="gotoLoginScriptEditl"   value="{t}Admin Toggle{/t}">
                    </td>
                </tr>
            </table>
		</td>
	</tr>
</table>	
	
		


	

{/if}
