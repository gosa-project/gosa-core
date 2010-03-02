<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
 <!-- Headline container -->
<h2>{image path="images/lists/key.png"} {t}Certificates{/t}
</h2>

<table summary="" style="vertical-align:top; text-align:left;border-top: 1px solid rgb(176,176,176);" width="100%" cellpadding=4 border=0>
 <tr>
   <td width="12%" style="vertical-align:top">
      {t}Standard certificate{/t}
   </td>
  
   <td style="vertical-align:top">
	 <LABEL for="userCertificate_file">{if $Certificate_readable}{$userCertificateinfo}{/if}</LABEL>
	 </td>
	 <td style="vertical-align:top;text-align:right">
	 {if $userCertificate_state ne "true"}
	{render acl=$CertificateACL}
     <input id="userCertificate_file" name="userCertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
	{/render}
     {else}
	{render acl=$CertificateACL}
     <button type='submit' name='remove_userCertificate'>{t}Remove{/t}</button>

	{/render}
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
     <LABEL for="userSMIMECertificate_file">{if $Certificate_readable}{$userSMIMECertificateinfo}{/if}</LABEL>
   </td>
   <td style="vertical-align:top;text-align:right">
	 {if $userSMIMECertificate_state ne "true"}
	{render acl=$CertificateACL}
     <input id="userSMIMECertificate_file" name="userSMIMECertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
	{/render}
     {else}
	{render acl=$CertificateACL}
     <button type='submit' name='remove_userSMIMECertificate'>{t}Remove{/t}</button>

	{/render}
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
     <LABEL for="userPKCS12_file">{if $Certificate_readable}{$userPKCS12info}{/if}</LABEL>
   </td>
   <td style="vertical-align:top;text-align:right">
	 {if $userPKCS12_state ne "true"}
	{render acl=$CertificateACL}
     <input id="userPKCS12_file" name="userPKCS12_file" type="file" size="20" maxlength="255" accept="text/*.*">
	{/render}
	 {else}
	{render acl=$CertificateACL}
     <button type='submit' name='remove_userPKCS12'>{t}Remove{/t}</button>

	{/render}
	 {/if}
   </td>
 </tr>
 
 {if $governmentmode eq "true"}
 <tr>
   <td>
     <LABEL for="certificateSerialNumber">{t}Certificate serial number{/t}</LABEL>
   </td>
   <td>
	{render acl=$CertificateACL}
     <input type='text' id="certificateSerialNumber" name="certificateSerialNumber" size=10 maxlength=20 value="{$certificateSerialNumber}">
	{/render}
   </td>
 </tr>
 {/if}
</table>

<p class="plugbottom">
	{render acl=$CertificateACL}
  <button type='submit' name='cert_edit_finish'>{msgPool type=saveButton}</button>

	{/render}
  &nbsp;
  <button type='submit' name='cert_edit_cancel'>{msgPool type=cancelButton}</button>

</p>

