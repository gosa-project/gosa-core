<h1>{t}Setup continued...{/t}</h1>
{t}The following fields allow the basic configuration of GOsa's behaviour and affect various properties in your main configuration.{/t}

<p class="seperator">&nbsp;</p>

<p>
 {t}Enter a description for the location you're configuring here{/t}:
</p>
<table>
 <tr>
  <td>{t}Location name{/t}</td>
  <td><input type='text' name='location' maxlength='80' size='25' value='{$location}'></td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>
<p>
 {t}GOsa always acts as admin and manages access rights internally. This is a workaround till OpenLDAP's in directory ACI's are fully implemented. For this to work, we need the admin DN and the corresponding password.{/t}
</p>

<table>
 <tr>
  <td>{t}Admin DN{/t}</td>
  <td><input type='text' name='admin' maxlength='80' size='40' value='{$admin}'></td>
 </tr>
 <tr>
  <td>{t}Admin password{/t}</td>
  <td><input type='password' name='password' maxlength='20' size='20' value='{$password}'></td>
 </tr>
</table>
<p class="seperator">&nbsp;</p>

<p>
 {t}Some basic LDAP parameters are tunable and affect the locations where GOsa saves people and groups, including the way accounts get created. Check the values below if the fit your needs.{/t}
</p>

<table>
 <tr>
  <td>{t}People storage ou{/t}</td>
  <td><input type='text' name='peopleou' maxlength='40' size='20' value='{$peopleou}'></td>
 </tr>
 <tr>
  <td>{t}People dn attribute{/t}</td>
  <td>
   <select size="1" name="peopledn">
    {html_options values=$peopledns output=$peopledns selected=$peopledn}
   </select>
  </td>
 </tr>
 <tr>
  <td>{t}Group storage ou{/t}</td>
  <td><input type='text' name='groupou' maxlength='40' size='20' value='{$groupou}'></td>
 </tr>
 <tr>
  <td>{t}ID base for users/groups{/t}</td>
  <td><input type='text' name='uidbase' maxlength='20' size='10' value='{$uidbase}'></td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<p>
 {t}GOsa supports several encryption types for your passwords. Normally this is adjustable via user templates, but you can specify a default method to be used here, too.{/t}
</p>

<table>
 <tr>
  <td>{t}Encryption algorithm{/t}</td>
  <td>
	 <select name="arr_crypts" size="1" title="">
        {html_options options=$arr_cryptkeys selected=$arr_crypts}
     </select>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<p>
 {t}GOsa has modular support for several mail methods. These methods provide interfaces to users mailboxes and general handling for quotas. You can choose the dummy plugin to leave all your mail settings untouched.{/t}
</p>

<table>
 <tr>
  <td>{t}Mail method{/t}</td>
  <td>
     <select name="mail" size="1" title="">
        {html_options options=$mail_methods selected=$mail}
     </select>
 </tr>
</table>

<p class="plugbottom">
 <input type="hidden" name="next" value="5">
 <input type='submit' name='back' value='{t}Back{/t}'> 
 <input type='submit' name='check' value='{t}Check{/t}'>
</p>
