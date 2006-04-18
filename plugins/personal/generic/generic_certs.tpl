<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
 <!-- Headline container -->
<h2><img alt="" class="center" align="middle" src="images/certs.png"> {t}Certificates{/t}</h2>

<table summary="" style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0>
 <tr>
   <td width="12%" style="vertical-align:top">
      {t}Standard certificate{/t}
   </td>
   <td style="vertical-align:top">
	 <LABEL for="userCertificate_file">{$userCertificateinfo}</LABEL>
	 </td>
	 <td style="vertical-align:top;text-align:right">
	 {if $userCertificate_state ne "true"}
     <input id="userCertificate_file" name="userCertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
     {else}
     <input type=submit name="remove_userCertificate" value="{t}Remove{/t}">
	 {/if}
   </td>
 </tr>
 </table>

<br>

 <table summary="" style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0>
 <tr>
   <td width="12%" style="vertical-align:top">
      {t}S/MIME certificate{/t} 
   </td>
   <td style="vertical-align:top">
     <LABEL for="userSMIMECertificate_file">{$userSMIMECertificateinfo}</LABEL>
   </td>
   <td style="vertical-align:top;text-align:right">
	 {if $userSMIMECertificate_state ne "true"}
     <input id="userSMIMECertificate_file" name="userSMIMECertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
     {else}
     <input type=submit name="remove_userSMIMECertificate" value="{t}Remove{/t}">
	 {/if}
   </td>
 </tr>
 </table>

<br>

<table summary="" style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0> 
 <tr>
   <td width="12%" style="vertical-align:top">
      {t}PKCS12 certificate{/t} 
   </td>
   <td style="vertical-align:top">
     <LABEL for="userPKCS12_file">{$userPKCS12info}</LABEL>
   </td>
   <td style="vertical-align:top;text-align:right">
	 {if $userPKCS12_state ne "true"}
     <input id="userPKCS12_file" name="userPKCS12_file" type="file" size="20" maxlength="255" accept="text/*.*">
	 {else}
     <input type=submit name="remove_userPKCS12" value="{t}Remove{/t}">
	 {/if}
   </td>
 </tr>
 
 {if $governmentmode eq "true"}
 <tr>
   <td>
     <LABEL for="certificateSerialNumber">{t}Certificate serial number{/t}</LABEL>
   </td>
   <td>
     <input id="certificateSerialNumber" name="certificateSerialNumber" size=10 maxlength=20 {$certificateSerialNumberACL}
            value="{$certificateSerialNumber}">
   </td>
 </tr>
 {/if}
</table>

<p class="plugbottom">
  <input type=submit name="cert_edit_finish" value="{t}Save{/t}">
  &nbsp;
  <input type=submit name="cert_edit_cancel" value="{t}Cancel{/t}">
</p>

