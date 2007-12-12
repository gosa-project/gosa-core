<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}GLPI database information{/t}</h2>
  <table summary="">
    <tr>
     <td>{t}Logging DB user{/t}{$must}</td>
     <td>
{render acl=$goGlpiAdminACL}
      <input name="goGlpiAdmin" id="goGlpiAdmin" size=30 maxlength=60 value="{$goGlpiAdmin}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Password{/t}</td>
     <td>
{render acl=$goGlpiPasswordACL}
      <input type="password" name="goGlpiPassword" id="goGlpiPassword" size=30 maxlength=60 value="{$goGlpiPassword}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Database{/t}{$must}</td>
     <td>
{render acl=$goGlpiDatabaseACL}
      <input name="goGlpiDatabase" id="goGlpiDatabase" size=30 maxlength=60 value="{$goGlpiDatabase}">
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
<input type="hidden" name="goGlpiServer_posted" value="1">
