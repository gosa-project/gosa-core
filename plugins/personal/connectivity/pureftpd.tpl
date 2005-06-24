<h2>{if $tabbed eq 1}<input type="checkbox" name="pureftpd" value="B" {$pureftpdState} {$pureftpdACL} onclick="changeState('FTPUploadBandwidth'); changeState('FTPDownloadBandwidth'); changeState('FTPQuotaFiles'); changeState('FTPQuotaMBytes'); changeState('FTPUploadRatio'); changeState('FTPDownloadRatio'); changeState('FTPStatus');" {$pureftpdACL}>{/if}{t}FTP account{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
   <td style="width:50%; vertical-align:top;">
     <table summary="" style="margin-left:4px;">
       <tr>
         <td colspan=2 style="vertical-align:top;">
           <b>{t}Bandwidth{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Upload bandwidth{/t}</td>
	 <td><input name="FTPUploadBandwidth" id="FTPUploadBandwidth" size=7 maxlength=7 {$FTPUploadBandwidthACL} value="{$FTPUploadBandwidth}" {$fstate}> {t}kb/s{/t}</td>
       </tr>
       <tr>
         <td>{t}Download bandwidth{/t}</td>
	 <td><input name="FTPDownloadBandwidth" id="FTPDownloadBandwidth" size=7 maxlength=7 {$FTPDownloadBandwidthACL} value="{$FTPDownloadBandwidth}" {$fstate}> {t}kb/s{/t}</td>
       </tr>
     </table>
   </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
     &nbsp;
   </td>
   <td>
     <table summary="">
       <tr>
         <td colspan=2 style="vertical-align:top;">
           <b>{t}Quota{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Files{/t}</td>
	 <td><input name="FTPQuotaFiles" id="FTPQuotaFiles" size=7 maxlength=10 {$FTPQuotaFilesACL} value="{$FTPQuotaFiles}" {$fstate}></td>
       </tr>
       <tr>
         <td>{t}Size{/t}</td>
	 <td><input name="FTPQuotaMBytes" id="FTPQuotaMBytes" size=7 maxlength=10 {$FTPQuotaMBytesACL} value="{$FTPQuotaMBytes}" {$fstate}> {t}MB{/t}</td>
       </tr>
     </table>
   </td>
 </tr>
 <tr>
   <td>
     <table summary="" style="margin-left:4px;">
       <tr>
         <td colspan=2 style="vertical-align:top;">
           <b>{t}Ratio{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Uploaded / downloaded files{/t}</td>
	 <td><input name="FTPUploadRatio" id="FTPUploadRatio" size=5 maxlength=20 {$FTPUploadRatioACL} value="{$FTPUploadRatio}" {$fstate}>
	 / <input name="FTPDownloadRatio" id="FTPDownloadRatio" size=5 maxlength=20 {$FTPDownloadRatioACL} value="{$FTPDownloadRatio}" {$fstate}></td>
       </tr>
     </table>
   </td>
   <td>
     <table summary="">
       <tr>
         <td colspan=2 style="vertical-align:top;">
           <b>{t}Miscellaneous{/t}</b>
         </td>
       <tr>
         <td><input type=checkbox name="FTPStatus" id="FTPStatus" value="disabled" {$use_FTPStatus} {$FTPStatusACL} title="{t}Check to disable FTP Access{/t}" {$fstate}>{t}Temporary disable FTP access{/t}</td>
       </tr>
     </table>
   </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.FTPUploadBandwidth.focus();
  -->
</script>
