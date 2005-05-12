<h1>{t}Setup continued...{/t}</h1>

<p>
 {t}Enter a description for the location you're configuring here{/t}:
</p>
<table>
 <tr>
  <td><b>{t}Location name{/t}</b></td>
  <td><input type='text' name='location' maxlength='80' size='25' value='{$location}'></td>
 </tr>
</table>

<p>
 {t}Finally, you need to specify parameters to access the LDAP server. GOsa always acts as admin and manages access rights internally. This is a workaround till OpenLDAP's in directory ACI's are fully implemented. For this to work, we need the admin DN and the corresponding password.{/t}
</p>

<table>
 <tr>
  <td><b>{t}Admin DN{/t}</b></td>
  <td><input type='text' name='admin' maxlength='80' size='40' value='{$admin}'></td>
 </tr>
 <tr>
  <td><b>{t}Admin password{/t}</b></td>
  <td><input type='password' name='password' maxlength='20' size='20' value='{$password}'></td>
 </tr>
</table>

<p>
 {t}Some basic LDAP parameters are tunable and affect the locations where GOsa saves people and groups, including the way accounts get created. Check the values below if the fit your needs.{/t}
</p>

<table>
 <tr>
  <td><b>{t}People storage ou{/t}</b></td>
  <td><input type='text' name='peopleou' maxlength='40' size='20' value='{$peopleou}'></td>
 </tr>
 <tr>
  <td><b>{t}People dn attribute{/t}</b></td>
  <td>
   <select size="1" name="peopledn">
    {html_options values=$peopledns output=$peopledns selected=$peopledn}
   </select>
  </td>
 </tr>
 <tr>
  <td><b>{t}Group storage ou{/t}</b></td>
  <td><input type='text' name='groupou' maxlength='40' size='20' value='{$groupou}'></td>
 </tr>
 <tr>
  <td><b>{t}ID base for users/groups{/t}</b></td>
  <td><input type='text' name='uidbase' maxlength='20' size='10' value='{$uidbase}'></td>
 </tr>
</table>

<div style="width:99%; text-align:right;">
 <input type='submit' name='check' value='{t}Check{/t}' {$mode}>
</div>
