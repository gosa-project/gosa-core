{if $is_account ne 'true'}
	<img alt="" class="center" src="images/stop.png" align="middle">
	{t}The environment extension is currently disabled.{/t}
{else}
<input type="hidden" name="iamposted" value="1">

<h2><img class="center" alt="" src="images/fai_profile.png" align="middle" /> {t}Profiles{/t}</h2>
<table summary="{t}Environment managment settings{/t}" width="100%">
  <tr>
   <td style="width:50%;border-right:1px solid #B0B0B0;vertical-align:top;">
			<table summary="{t}Profile managment{/t}">
				<tr>
					<td style="vertical-align:top">
						<input class="center" type="checkbox" value="1" {$useProfileCHK} name="useProfile" {$useProfileACL} id="useProfile"
									onclick="changeState('gotoProfileServer');changeState('gotoProfileFlag_C');changeState('gotoProfileQuota');">
					</td>
					<td style="vertical-align:middle;">
						<label for="useProfile">{t}Use profile managment{/t}</label>
					</td>
				</tr>
				<tr>
					<td>&nbsp;
					</td>
					<td>
						<table summary="{t}Profile server managment{/t}">
							<tr>
								<td>	
									<label for="gotoProfileServer">{t}Profil path{/t}</label>
								</td>
								<td>
									<select id="gotoProfileServer" 
											name="gotoProfileServer" {$gotoProfileServerACL}>
										{html_options values=$gotoProfileServerKeys output=$gotoProfileServers selected=$gotoProfileServer}
										<option disabled>&nbsp;</option>
									</select>
								</td>
							</tr>
							<tr>
								<td>
									<label for="gotoProfileQuota">{t}Profil quota{/t}</label> 
									<br>
									<br>
								</td>
								<td>
									<input type="text" name="gotoProfileQuota" value="{$gotoProfileQuota}" size="6"	{$gotoProfileQuotaACL} id="gotoProfileQuota">{t}MB{/t}
									<br>
									<br>
								</td>
							</tr>
							<tr>
								<td colspan=2>	
									<input class="center" type="checkbox" name="gotoProfileFlag_C" value="C" {$gotoProfileFlag_CCHK}
										id="gotoProfileFlag_C" {$gotoProfileFlag_CACL}>&nbsp;
									<label for="gotoProfileFlag_C">{t}Cache profile localy{/t}</label>
								</td>
							</tr>
						</table>
					</td>
				</tr>
				</table>
		</td>
		<td style="vertical-align:top">

			<table summary="{t}Kiosk profile settings{/t}">
			{if $kiosk_enabled}
				<tr>
					<td>
						<label for="gotoKioskProfile">{t}Kiosk profile{/t}</label>
					</td>
					<td>
						<select name="gotoKioskProfile" {$gotoKioskProfileACL} id="gotoKioskProfile">
                        {html_options values=$gotoKioskProfileKeys output=$gotoKioskProfiles selected=$gotoKioskProfile}
                        <option disabled>&nbsp;</option></select>&nbsp;
						<input type="submit" name="KioskManagementDialog" value="{t}Manage{/t}" {$gotoKioskProfileACL}>
					</td>
				</tr>
			{else}
				<tr>
					<td>
						<label for="dummy1">{t}Kiosk profile{/t}</label>
					</td>
					<td>
						<select name="dummy1" disabled id="dummy1"><option disabled>&nbsp;</option></select>
						<input type="button" disabled name="dummy2" value="{t}Manage{/t}">
					</td>
				</tr>
			{/if}
				<tr>
					<td colspan=2>
						<br>
						<input type="checkbox" name="gotoProfileFlag_L" id="gotoProfileFlag_L" value="L"  {$gotoProfileFlag_LCHK}
							{if $gotoProfileFlagsACL=="disabled"}
								disabled 
							{/if}>
						<label for="gotoProfileFlag_L">{t}Resolution changeable during session{/t}</label>
					</td>
				</tr>
				<tr>
					<td>
						<label for="gotoXResolution">{t}Resolution{/t}</label>
					</td>
					<td>
						<select name="gotoXResolution" {$gotoXResolutionACL} id="gotoXResolution">
                        	{html_options values=$gotoXResolutionKeys output=$gotoXResolutions selected=$gotoXResolution}
                            <option disabled>&nbsp;</option>
                    	</select>
					</td>
				</tr>
			</table>	
   </td>
  </tr>
</table>

<p class="seperator">&nbsp;</p>

<table summary="{t}Environment managment settings{/t}" width="100%">
  <tr>
   <td style="width:50%;border-right:1px solid #B0B0B0">
	<h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle" />&nbsp;<label for="gotoShare">{t}Shares{/t}</label></h2>
	<table summary="" style="width:100%">
		<tr>
			<td>
				{$divlistShares}
				<input type="text" size=15 {$gotoShareACL} name="ShareUser" value="%u" title="{t}User used to connect to the share{/t}">@
				<select name="gotoShareSelection" {$gotoShareACL} title="{t}Select a share{/t}">
					{html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
					<option disabled>&nbsp;</option>
				</select>
				:
				<input type="text" size=15 {$gotoShareACL} name="gotoShareMountPoint" value="%H/Netzlaufwerke/%L/%S"
                  title="{t}Mount path{/t}">
				<input type="submit" {$gotoShareACL} name="gotoShareAdd" value="{t}Add{/t}"> 
	<!--			<input type="submit" {$gotoShareACL} name="gotoShareDel" value="{t}Remove{/t}"> -->
			</td>
		</tr>
	</table>
   </td>
   <td style="vertical-align:top">
	<h2><img class="center" alt="" src="images/fai_script.png" align="middle" />&nbsp;<label for="gotoLogonScript">{t}Logon scripts{/t}</label></h2>
	<table summary="{t}Logon script management{/t}" style="width:100%">
		<tr>
			<td>
			<select style="width:100%;" name="gotoLogonScript" multiple size=5 {$gotoLogonScriptACL} id="gotoLogonScript">
	{html_options values=$gotoLogonScriptKeys output=$gotoLogonScripts }
	<option disabled>&nbsp;</option>
				</select>
	
				<br>
				<input type="submit" {$gotoLogonScriptACL} name="gotoLogonScriptNew"	value="{t}Add{/t}">
				<input type="submit" {$gotoLogonScriptACL} name="gotoLogonScriptDel" 	value="{t}Delete{/t}"> 
				<input type="submit" {$gotoLogonScriptACL} name="gotoLogonScriptEdit" 	value="{t}Edit{/t}"> 
			</td>
		</tr>
	</table>	
   </td>
  </tr>
</table>

<p class="seperator">&nbsp;</p>

<table summary="{t}Environment managment settings{/t}" width="100%">
  <tr>
   <td style="border-right:1px solid #B0B0B0; width:50%; vertical-align:top">
	<h2><img alt="" src="images/hotplug.png" align="middle" class="center" />&nbsp;<label for="gotoHotplugDevice">{t}Hotplug devices{/t}</label></h2>	
	<table style="width:100%" summary="{t}Hotplug device settings{/t}">
		<tr>
			<td>
				<select name="gotoHotplugDevice[]" {$gotoHotplugDeviceACL} size=5  style="width:100%;" id="gotoHotplugDevice" multiple>
					{html_options values=$gotoHotplugDeviceKeys output=$gotoHotplugDevices}
					<option disabled>&nbsp;</option>
				</select>
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
   </td>
   <td>
    <h2><img alt="" src="images/select_printer.png" align="middle" class="center" />&nbsp;<label for="gotoPrinter">{t}Printer{/t}</label></h2>
    <table style="width:100%" summary="{t}Printer settings{/t}">
	<tr>
	    <td>
		<select style="width:100%;" name="gotoPrinterSel[]" multiple size=5 {$gotoPrinterACL} id="gotoPrinter">
		{html_options values=$gotoPrinterKeys output=$gotoPrinter}
		<option disabled>&nbsp;</option>
		</select>
		<br>
		<input type="submit" {$gotoPrinterACL} name="gotoPrinterAdd"     value="{t}Add{/t}">
		<input type="submit" {$gotoPrinterACL} name="gotoPrinterDel"     value="{t}Delete{/t}">
		<input type="submit" {$gotoPrinterToggleACL} name="gotoPrinterEdit"    value="{t}Toggle admin{/t}">
{if $is_group }
		<input type="submit" {$gotoPrinterToggleACL} name="gotoPrinterDefault"    value="{t}Toggle default{/t}" disabled>
{else}
		<input type="submit" {$gotoPrinterToggleACL} name="gotoPrinterDefault"    value="{t}Toggle default{/t}">
{/if}
	    </td>
	</tr>
    </table>
   </td>
  </tr>
</table>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('useProfile');
  -->
</script>
{/if}

