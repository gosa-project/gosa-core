<h3>
	{render acl=$pureftpdACL checkbox=$multiple_support checked=$use_pureftpd}
	<input type="checkbox" name="pureftpd" value="B" {$pureftpdState} 
		onclick="{$changeState}" class="center">
	{/render}
	{t}FTP account{/t}
</h3>

<table summary="{t}FTP configuration{/t}"
  style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
   <td style='width:50%; '>

     <table style="margin-left:4px;" summary="{t}Bandwith settings{/t}">
       <tr>
         <td colspan="2">

           <b>{t}Bandwidth{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Upload bandwidth{/t}</td>
    	 <td>
          {render acl=$FTPUploadBandwidthACL checkbox=$multiple_support checked=$use_FTPUploadBandwidth}
            <input type='text' name="FTPUploadBandwidth" id="FTPUploadBandwidth" size=7 
              maxlength=7 value="{$FTPUploadBandwidth}" {$fstate} >
          {/render}
          {t}KB/s{/t}
        </td>
       </tr>
       <tr>
         <td>{t}Download bandwidth{/t}</td>
         <td>
            {render acl=$FTPDownloadBandwidthACL  checkbox=$multiple_support checked=$use_FTPDownloadBandwidth}
              <input type='text' name="FTPDownloadBandwidth" id="FTPDownloadBandwidth" size=7 
                maxlength=7 value="{$FTPDownloadBandwidth}" {$fstate} >
            {/render}
            {t}KB/s{/t}
         </td>
       </tr>
     </table>
   </td>
   <td class='left-border' rowspan="2">

     &nbsp;
   </td>
   <td>
     <table summary="{t}Quota settings{/t}">
       <tr>
         <td colspan="2">

           <b>{t}Quota{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Files{/t}</td>
           <td>
            {render acl=$FTPQuotaFilesACL checkbox=$multiple_support checked=$use_FTPQuotaFiles}
              <input type='text' name="FTPQuotaFiles" id="FTPQuotaFiles" size=7 
                maxlength=10 value="{$FTPQuotaFiles}" {$fstate} >
            {/render}
          </td>
       </tr>
       <tr>
         <td>{t}Size{/t}</td>
         <td>
          {render acl=$FTPQuotaMBytesACL checkbox=$multiple_support checked=$use_FTPQuotaMBytes}
            <input type='text' name="FTPQuotaMBytes" id="FTPQuotaMBytes" size=7 
              maxlength=10 value="{$FTPQuotaMBytes}" {$fstate} > 
          {/render}
          {t}MB{/t}
        </td>
       </tr>
     </table>
   </td>
 </tr>
 <tr>
   <td>
     <table style="margin-left:4px;" summary="{t}Ratio settings{/t}">
       <tr>
         <td colspan="2">

           <b>{t}Ratio{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Uploaded / downloaded files{/t}</td>
         <td>
            {render acl=$FTPUploadRatioACL checkbox=$multiple_support checked=$use_FTPUploadRatio}
              <input type='text' name="FTPUploadRatio" id="FTPUploadRatio" size=5 
                maxlength=20 value="{$FTPUploadRatio}" {$fstate} >
            {/render}
              / 
            {render acl=$FTPDownloadRatioACL checkbox=$multiple_support checked=$use_FTPDownloadRatio}
              <input type='text' name="FTPDownloadRatio" id="FTPDownloadRatio" size=5 
                maxlength=20 value="{$FTPDownloadRatio}" {$fstate} >
            {/render}
         </td>
       </tr>
     </table>
   </td>
   <td>
     <table summary="{t}Miscellaneous{/t}">
       <tr>
         <td colspan="2">

           <b>{t}Miscellaneous{/t}</b>
         </td>
       <tr>
         <td>
          {render acl=$FTPStatusACL checkbox=$multiple_support checked=$use_FTPStatus}
            <input type=checkbox name="FTPStatus" id="FTPStatus" value="disabled" {$FTPStatus} 
              title="{t}Check to disable FTP Access{/t}" {$fstate} class="center">
          {/render}
          {t}Temporary disable FTP access{/t}</td>
       </tr>
     </table>
   </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
	focus_field('FTPUploadBandwidth');
</script>
