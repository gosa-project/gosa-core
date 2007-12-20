<h2>
	{render acl=$pureftpdACL checkbox=$multiple_support checked=$use_pureftpd}
	<input type="checkbox" name="pureftpd" value="B" {$pureftpdState} 
		onclick="{$changeState}" class="center">
	{/render}
	{t}FTP account{/t}
</h2>

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
	 <td>
{render acl=$FTPUploadBandwidthACL checkbox=$multiple_support checked=$use_FTPUploadBandwidth}
<input name="FTPUploadBandwidth" id="FTPUploadBandwidth" size=7 maxlength=7 value="{$FTPUploadBandwidth}" {$fstate} >
{/render}
	 {t}kb/s{/t}</td>
       </tr>
       <tr>
         <td>{t}Download bandwidth{/t}</td>
	 <td>
{render acl=$FTPDownloadBandwidthACL  checkbox=$multiple_support checked=$use_FTPDownloadBandwidth}
<input name="FTPDownloadBandwidth" id="FTPDownloadBandwidth" size=7 maxlength=7 value="{$FTPDownloadBandwidth}" {$fstate} >
{/render}
	 {t}kb/s{/t}</td>
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
	 <td>
{render acl=$FTPQuotaFilesACL checkbox=$multiple_support checked=$use_FTPQuotaFiles}
<input name="FTPQuotaFiles" id="FTPQuotaFiles" size=7 maxlength=10 value="{$FTPQuotaFiles}" {$fstate} >
{/render}
	</td>
       </tr>
       <tr>
         <td>{t}Size{/t}</td>
	 <td>
{render acl=$FTPQuotaMBytesACL checkbox=$multiple_support checked=$use_FTPQuotaMBytes}
<input name="FTPQuotaMBytes" id="FTPQuotaMBytes" size=7 maxlength=10 value="{$FTPQuotaMBytes}" {$fstate} > 
{/render}
	{t}MB{/t}
	</td>
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
	 <td>

{render acl=$FTPUploadRatioACL checkbox=$multiple_support checked=$use_FTPUploadRatio}
<input name="FTPUploadRatio" id="FTPUploadRatio" size=5 maxlength=20 value="{$FTPUploadRatio}" {$fstate} >
{/render}
	 / 
{render acl=$FTPDownloadRatioACL checkbox=$multiple_support checked=$use_FTPDownloadRatio}
<input name="FTPDownloadRatio" id="FTPDownloadRatio" size=5 maxlength=20 value="{$FTPDownloadRatio}" {$fstate} >
{/render}
	</td>
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
         <td>
{render acl=$FTPStatusACL checkbox=$multiple_support checked=$use_FTPStatus}
<input type=checkbox name="FTPStatus" id="FTPStatus" value="disabled" {$FTPStatus} title="{t}Check to disable FTP Access{/t}" {$fstate} class="center">
{/render}
{t}Temporary disable FTP access{/t}</td>
       </tr>
     </table>
   </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('FTPUploadBandwidth');
  -->
</script>
