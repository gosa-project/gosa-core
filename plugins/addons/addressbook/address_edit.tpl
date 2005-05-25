<table style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;" cellpadding=4 cellspacing=0 border=0>
<tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
<td align=left width="100%">{$storage_info}
 <select name="storage_base" size=1" title="{t}Choose the department to store entry in{/t}">
  {html_options options=$deplist selected=$storage_base}
 </select>
</td>
<td><a href="{$url}">X</a></td>
</tr>
<tr style="background-color: white">
<td colspan=2>

<table width="100%" cellspacing=2 cellpadding=4>
<tr>
<td style="width:50%; vertical-align:top; background-color: #F0F0F0">
 <h2><img src="{$personal_image}" align=middle>&nbsp;{t}Personal{/t}</h2>
 
 <table width="100%">
  <tr>
   <td>{t}Name{/t}{$must}, {t}Given name{/t}{$must}</td><td><input name="sn" size=10 maxlength=60 value="{$info_sn}">, <input name="givenName" size=10 maxlength=60 value="{$info_givenName}"><td>
  </tr>
  <tr>
   <td>{t}Initials{/t}</td><td><input name="initials" size=5 maxlength=20 value="{$info_initials}"></td>
  </tr>
  <tr>
   <td>{t}Personal title{/t}</td><td><input name="title" size=10 maxlength=20 value="{$info_title}"></td>
  </tr>
 </table>

</td>

<td style="width:50%; vertical-align:top; background-color: #E8E8E8">
 <h2><img src="{$home_image}" align=middle>&nbsp;{t}Private{/t}</h2>

 <table width="100%">
  <tr>
   <td>{t}Address{/t}<br><br></td>
   <td style="vertical-align:top;"><textarea name="homePostalAddress" rows=1 cols=20>{$info_homePostalAddress}</textarea></td>
  </tr>
  <tr>
   <td>{t}Phone{/t}</td><td><input name="homePhone" size=15 maxlength=60 value="{$info_homePhone}"></td>
  </tr>
  <tr>
   <td>{t}Mobile{/t}</td><td><input name="mobile" size=15 maxlength=60 value="{$info_mobile}"></td>
  </tr>
  <tr>
   <td>{t}Email{/t}</td><td><input name="mail" size=15 maxlength=60 value="{$info_mail}"></td>
  </tr>
 </table>

</td>

</tr>

<tr>
<td style="width:100%; vertical-align:top; background-color: #E4E4E4" colspan=2>
 <h2><img src="{$company_image}" align=middle>&nbsp;{t}Organizational{/t}</h2>

 <table width="100%">
  <tr>
   <td>
  
    <table width="100%">
     <tr>
      <td>{t}Company{/t}</td><td><input name="o" size=15 maxlength=60 value="{$info_o}"></td>
     </tr>
     <tr>
      <td>{t}Department{/t}</td><td><input name="ou" size=15 maxlength=60 value="{$info_ou}"></td>
     </tr>
     <tr>
      <td>{t}City{/t}</td><td><input name="l" size=15 maxlength=60 value="{$info_l}"></td>
     </tr>
     <tr>
      <td>{t}Postal code{/t}</td><td><input name="postalCode" size=15 maxlength=60 value="{$info_postalCode}"></td>
     </tr>
     <tr>
      <td>{t}Country{/t}</td><td><input name="st" size=15 maxlength=60 value="{$info_st}"></td>
     </tr>
    </table>
   </td>
 
   <td style="vertical-align:top">

    <table width="100%">
     <tr>
      <td>{t}Address{/t}<br><br></td><td style="vertical-align:top;"><textarea name="postalAddress" rows=1 cols=20>{$info_postalAddress}</textarea></td>
     </tr>
     <tr>
      <td>{t}Phone{/t}</td><td><input name="telephoneNumber" size=15 maxlength=60 value="{$info_telephoneNumber}"></td>
     </tr>
     <tr>
      <td>{t}FAX{/t}</td><td><input name="facsimileTelephoneNumber" size=15 maxlength=60 value="{$info_facsimileTelephoneNumber}"></td>
     </tr>
     <tr>
      <td>{t}Pager{/t}</td><td><input name="pager" size=15 maxlength=60 value="{$info_pager}"></td>
     </tr>
    </table>
   </td>
  </tr>
 </table>

</td>
</tr>
</table>

<p align=right>
  <input type="submit" name="save" value="{t}Save{/t}">
  <input type="submit" name="cancel" value="{t}Cancel{/t}">
</p>

</td>
</tr>
</table>

