<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
<table style="vertical-align:top; text-align:left;" cellpadding=4 border=0>

 <!-- Headline container -->
 <tr>
   <td colspan=2>
     <h2><img align="center" src="images/certs.png"> {t}Certificates{/t}</h2>
   </td>

 </tr>

 <tr>
   <td>
      {t}Standard certificate{/t} (<i>{$userCertificate_state}</i>)
   </td>
   <td>
     <input name="userCertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
     &nbsp;
     <input type=submit name="remove_userCertificate" value="{t}Remove{/t}">
   </td>
 </tr>
 
 <tr>
   <td>
      {t}S/MIME certificate{/t} (<i>{$userSMIMECertificate_state}</i>)
   </td>
   <td>
     <input name="userSMIMECertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
     &nbsp;
     <input type=submit name="remove_userSMIMECertificate" value="{t}Remove{/t}">
   </td>
 </tr>
 
 <tr>
   <td>
      {t}PKCS12 certificate{/t} (<i>{$userPKCS12_state}</i>)
   </td>
   <td>
     <input name="userPKCS12_file" type="file" size="20" maxlength="255" accept="text/*.*">
     &nbsp;
     <input type=submit name="remove_userPKCS12" value="{t}Remove{/t}">
   </td>
 </tr>
 
 {if $governmentmode eq "true"}
 <tr>
   <td>
     {t}Certificate serial number{/t}
   </td>
   <td>
     <input name="certificateSerialNumber" size=10 maxlength=20 {$certificateSerialNumberACL}
            value="{$certificateSerialNumber}">
   </td>
 </tr>
 {/if}
</table>

<br>
<p class="plugbottom">
  <input type=submit name="cert_edit_finish" value="{t}Finish{/t}">
  &nbsp;
  <input type=submit name="cert_edit_cancel" value="{t}Cancel{/t}">
</p>

