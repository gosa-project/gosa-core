<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Kerberos kadmin access{/t}</h2>
  <table summary="">
    <tr>
     <td>{t}Kerberos Realm{/t}{$must}</td>
     <td>
{render acl=$goKrbRealmACL}
      <input name="goKrbRealm" id="goKrbRealm" size=30 maxlength=60  value="{$goKrbRealm}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Admin user{/t}{$must}</td>
     <td>
{render acl=$goKrbAdminACL}
      <input name="goKrbAdmin" id="goKrbAdmin" size=30 maxlength=60 value="{$goKrbAdmin}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Password{/t}{$must}</td>
     <td>
{render acl=$goKrbPasswordACL}
      <input type=password name="goKrbPassword" id="goKrbPassword" size=30 maxlength=60 value="{$goKrbPassword}">
{/render}
     </td>
    </tr>
   </table>
<p class='seperator'>&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
<input type="hidden" name="goKrbServerPosted" value="1">
