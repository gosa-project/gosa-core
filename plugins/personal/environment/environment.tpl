{if $is_account ne 'true'}
 <img alt="" class="center" src="images/stop.png" align="middle">
 {t}The environment extension is currently disabled.{/t}
{else}


<input type="hidden" name="iamposted" value="1">

<h2> 
 <img class="center" alt="" src="images/fai_profile.png" align="middle" /> 
 {t}Profiles{/t}
</h2>

<table summary="{t}Environment managment settings{/t}" width="100%">
 <tr>
  <td style="width:50%;border-right:1px solid #B0B0B0;vertical-align:top;">
   <table summary="{t}Profile managment{/t}">
    <tr>
     <td style="vertical-align:top">
{render acl=$gotoProfileACL}
      <input class="center" type="checkbox" value="1" {$useProfileCHK} name="useProfile" id="useProfile" onclick="changeState('gotoProfileServer');changeState('gotoProfileFlagC');changeState('gotoProfileQuota');">
{/render}
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
{render acl=$gotoProfileServerACL}

{if $gotoProfileServerWriteable}
         <select id="gotoProfileServer" name="gotoProfileServer">
          {html_options values=$gotoProfileServerKeys output=$gotoProfileServers selected=$gotoProfileServer}
          <option disabled>&nbsp;</option>
         </select>
{else}
         <select id="gotoProfileServer" name="gotoProfileServer">
          {html_options values=$gotoProfileServer output=$gotoProfileServer selected=$gotoProfileServer}
          <option disabled>&nbsp;</option>
         </select>
{/if}
{/render}
        </td>
       </tr>
       <tr>
        <td>
         <label for="gotoProfileQuota">{t}Profil quota{/t}</label> 
         <br>
         <br>
        </td>
        <td>
{render acl=$gotoProfileQuotaACL}
         <input type="text" name="gotoProfileQuota" value="{$gotoProfileQuota}" size="6" id="gotoProfileQuota">{t}MB{/t}
{/render}
         <br>
         <br>
        </td>
       </tr>
       <tr>
        <td colspan=2> 
{render acl=$gotoProfileFlagCACL}
         <input class="center" type="checkbox" name="gotoProfileFlagC" value="C" {$gotoProfileFlagCCHK} id="gotoProfileFlagC">&nbsp;
{/render}
         <label for="gotoProfileFlagC">{t}Cache profile localy{/t}</label>
        </td>
       </tr>
      </table>
     </td>
    </tr>
   </table>
  </td>
  <td style="vertical-align:top">

  <table summary="{t}Kiosk profile settings{/t}">
{if $kiosk_enabled }
    <tr>
     <td colwidth=2>
      <label for="kiosk_server">{t}Kiosk profile{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      {t}Server{/t}
      <select name='kiosk_server' id="kiosk_server" onChange='document.mainform.submit();'>
       {html_options options=$kiosk_servers selected=$kiosk_server}
      </select>
     </td>
     <td>
      {if $kiosk_server != "none"}
      {t}Profile{/t}
      <select name='kiosk_profile'>
       {html_options values=$kiosk_profiles output=$kiosk_profiles selected=$kiosk_profile}
      </select>
      {else}
        &nbsp;
      {/if}
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
{render acl=$gotoProfileFlagLACL}
      <input type="checkbox" name="gotoProfileFlagL" id="gotoProfileFlagL" value="L" {$gotoProfileFlagLCHK}>
{/render}
      <label for="gotoProfileFlagL">{t}Resolution changeable during session{/t}</label>
     </td>
    </tr>
    <tr>
     <td>
      <label for="gotoXResolution">{t}Resolution{/t}</label>
     </td>
     <td>
{render acl=$gotoXResolutionACL}
      <select name="gotoXResolution" id="gotoXResolution">
       {html_options values=$gotoXResolutionKeys output=$gotoXResolutions selected=$gotoXResolution}
      <option disabled>&nbsp;</option>
      </select>
{/render}
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
   <h2>
    <img class="center" alt="" src="images/fai_partitionTable.png" align="middle" />&nbsp;
    <label for="gotoShare">{t}Shares{/t}</label>
   </h2>
   <table summary="" style="width:100%">
    <tr>
     <td>
	
{render acl=$gotoShareACL}
      {$divlistShares}
{/render}
{render acl=$gotoShareACL}
      <input type="text" size=15 name="ShareUser" value="%u" title="{t}User used to connect to the share{/t}">@
{/render}
{render acl=$gotoShareACL}
      <select name="gotoShareSelection" title="{t}Select a share{/t}">
       {html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
       <option disabled>&nbsp;</option>
      </select>
{/render}
      :
{render acl=$gotoShareACL}
      <input type="text" size=15 name="gotoShareMountPoint" value="%H/Netzlaufwerke/%L/%S"
      title="{t}Mount path{/t}">
{/render}
{render acl=$gotoShareACL}
      <input type="submit" name="gotoShareAdd" value="{t}Add{/t}"> 
{/render}
      <!--   <input type="submit" name="gotoShareDel" value="{t}Remove{/t}"> -->
     </td>
    </tr>
   </table>
  </td>
  <td style="vertical-align:top">
   <h2>
    <img class="center" alt="" src="images/fai_script.png" align="middle" />&nbsp;
    <label for="gotoLogonScript">{t}Logon scripts{/t}</label>
   </h2>
   <table summary="{t}Logon script management{/t}" style="width:100%">
    <tr>
     <td>
{render acl=$gotoLogonScriptACL}
      <select style="width:100%;" name="gotoLogonScript" multiple size=5 id="gotoLogonScript">
       {html_options values=$gotoLogonScriptKeys output=$gotoLogonScripts }
       <option disabled>&nbsp;</option>
      </select>
{/render}

      <br>
{render acl=$gotoLogonScriptACL}
      <input type="submit" name="gotoLogonScriptNew" value="{t}Add{/t}">
{/render}
{render acl=$gotoLogonScriptACL}
      <input type="submit" name="gotoLogonScriptEdit"  value="{t}Edit{/t}" {if $gotoLogonScriptKeysCnt ==0} disabled {/if}>
{/render}
{render acl=$gotoLogonScriptACL}
      <input type="submit" name="gotoLogonScriptDel"  value="{t}Delete{/t}" {if $gotoLogonScriptKeysCnt ==0} disabled {/if}>
{/render}
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
   <h2>
    <img alt="" src="images/hotplug.png" align="middle" class="center" />&nbsp;
    <label for="gotoHotplugDevice_post">{t}Hotplug devices{/t}</label>
   </h2> 
   <table style="width:100%" summary="{t}Hotplug device settings{/t}">
    <tr>
     <td>
{render acl=$gotoHotplugDeviceACL}
      <select name="gotoHotplugDevice_post[]" size=5  style="width:100%;" id="gotoHotplugDevice_post" multiple>
       {html_options values=$gotoHotplugDeviceKeys output=$gotoHotplugDevices}
       <option disabled>&nbsp;</option>
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td>
{render acl=$gotoHotplugDeviceACL}
      <input type="submit" name="gotoHotplugDeviceUse" value="{t}Add{/t}">
{/render}
{render acl=$gotoHotplugDeviceACL}
      <input type="submit" name="gotoHotplugDeviceDel" value="{t}Delete{/t}"
      {if !$gotoHotplugDevices} disabled {/if}>
{/render}
     </td>
    </tr>
   </table>
  </td>
  <td>
   <h2>
    <img alt="" src="images/select_printer.png" align="middle" class="center" />&nbsp;
    <label for="gotoPrinter">{t}Printer{/t}</label>
   </h2>
   <table style="width:100%" summary="{t}Printer settings{/t}">
    <tr>
     <td>
{render acl=$gotoPrinterACL}
      <select style="width:100%;" name="gotoPrinterSel[]" multiple size=5 id="gotoPrinter">
       {html_options values=$gotoPrinterKeys output=$gotoPrinter}
       <option disabled>&nbsp;</option>
      </select>
{/render}
      <br>
{render acl=$gotoPrinterACL}
      <input type="submit"  name="gotoPrinterAdd"     value="{t}Add{/t}">
{/render}
{render acl=$gotoPrinterACL}
      <input type="submit" name="gotoPrinterDel"     value="{t}Delete{/t}" {if !$gotoPrinter } disabled {/if}>
{/render}
{render acl=$gotoPrinterACL}
      <input type="submit" name="gotoPrinterEdit"    value="{t}Toggle admin{/t}" {if !$gotoPrinter } disabled {/if}>
{/render}
{render acl=$gosaDefaultPrinterACL}
      <input type="submit" name="gotoPrinterDefault"    value="{t}Toggle default{/t}" {if !$gotoPrinter | $is_group} disabled {/if}>
{/render}
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

