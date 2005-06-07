<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
<table style="vertical-align:top; text-align:left;" cellpadding=4 border=0 width="100%">
 <!-- Headline container -->
 <tr>
   <td colspan=2>
     <h2><img alt="" align="middle" src="images/certs.png"> {t}Certificates{/t}</h2>
   </td>

 </tr>
 <tr>
   <td width="250">
      {t}Standard certificate{/t}
   </td>
   <td>
     <input name="userCertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
     &nbsp;
     <input type=submit name="remove_userCertificate" value="{t}Remove{/t}">
   </td>
 </tr>
 <tr>
	<td>
		{t}Status{/t}
	</td>
	<td>
		{$userCertificateinfo}
	</td>
 </tr>
 </table>






 <table style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0>
 <tr>
   <td width="250">
      {t}S/MIME certificate{/t} 
   </td>
   <td >
     <input name="userSMIMECertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
     &nbsp;
     <input type=submit name="remove_userSMIMECertificate" value="{t}Remove{/t}">
   </td>
 </tr>
 <tr>
    <td>
        {t}Status{/t}
    </td>
    <td>
        {$userSMIMECertificateinfo}
    </td>
 </tr>
 </table>



<table style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0> 
 <tr>
   <td width="250">
      {t}PKCS12 certificate{/t} 
   </td>
   <td>
     <input name="userPKCS12_file" type="file" size="20" maxlength="255" accept="text/*.*">
     &nbsp;
     <input type=submit name="remove_userPKCS12" value="{t}Remove{/t}">
   </td>
 </tr>
 <tr>
    <td>
        {t}Status{/t}
    </td>
    <td>
        {$userSMIMECertificateinfo}
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

