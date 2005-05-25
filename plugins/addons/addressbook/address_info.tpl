<table style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;" cellpadding=4 cellspacing=0 border=0>
<tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
<td align=left width="100%">{$storage_info}</td>
<td><a href="{$url}">X</a></td>
</tr>
<tr style="background-color: white">
<td colspan=2>

<table width="100%" cellspacing=2 cellpadding=4>
<tr>
<td style="width:50%; vertical-align:top; background-color: #F0F0F0">
 <h2><img src="{$personal_image}" align=middle> {t}Personal{/t}</h2>
 
 <table width="100%">
  <tr>
   <td>{t}Name{/t}</td><td><i>{$info_sn}, {$info_givenName}</i></td>
  </tr>
  <tr>
   <td>{t}Initials{/t}</td><td><i>{$info_initials}</i></td>
  </tr>
  <tr>
   <td>{t}Personal title{/t}</td><td><i>{$info_title}</i></td>
  </tr>
 </table>

</td>

<td style="width:50%; vertical-align:top; background-color: #E8E8E8">
 <h2><img src="{$home_image}" align=middle> {t}Private{/t}</h2>

 <table width="100%">
  <tr>
   <td>{t}Address{/t}<br><br></td>
   <td style="vertical-align:top;"><i>{$info_homePostalAddress}</i></td>
  </tr>
  <tr>
   <td>{t}Phone{/t}</td><td><i>{$info_homePhone}</i></td>
  </tr>
  <tr>
   <td>{t}Mobile{/t}</td><td><i>{$info_mobile}</i></td>
  </tr>
  <tr>
   <td>{t}Email{/t}</td><td><i><a href="mailto:{$info_givenName} {$info_sn} <{$info_mail}>">{$info_mail}</a></i></td>
  </tr>
 </table>

</td>

</tr>

<tr>
<td style="width:100%; vertical-align:top; background-color: #E4E4E4" colspan=2>
 <h2><img src="{$company_image}" align=middle> {t}Organizational{/t}</h2>

 <table width="100%">
  <tr>
   <td>
  
    <table width="100%">
     <tr>
      <td>{t}Company{/t}</td><td><i>{$info_o}</i></td>
     </tr>
     <tr>
      <td>{t}Department{/t}</td><td><i>{$info_ou}</i></td>
     </tr>
     <tr>
      <td>{t}City{/t}</td><td><i>{$info_l}</i></td>
     </tr>
     <tr>
      <td>{t}Postal code{/t}</td><td><i>{$info_postalCode}</i></td>
     </tr>
     <tr>
      <td>{t}Country{/t}</td><td><i>{$info_st}</i></td>
     </tr>
    </table>
   </td>
 
   <td style="vertical-align:top">

    <table width="100%">
     <tr>
      <td>{t}Address{/t}<br><br></td><td style="vertical-align:top;"><i>{$info_postalAddress}</i></td>
     </tr>
     <tr>
      <td>{t}Phone{/t}</td><td><i>{$info_telephoneNumber}</i></td>
     </tr>
     <tr>
      <td>{t}FAX{/t}</td><td><i>{$info_facsimileTelephoneNumber}</i></td>
     </tr>
     <tr>
      <td>{t}Pager{/t}</td><td><i>{$info_pager}</i></td>
     </tr>
    </table>
   </td>
  </tr>
 </table>

</td>
</tr>
</table>

</td>
</tr>
</table>
