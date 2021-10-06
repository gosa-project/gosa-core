<h3>{t}FAX database information{/t}</h3>
 <table summary="{t}Fax database{/t}">
  <tr>
   <td>{t}FAX DB user{/t}{$must}</td>
   <td>
    {render acl=$goFaxAdminACL}
     <input type='text' name="goFaxAdmin" id="goFaxAdmin" value="{$goFaxAdmin}" >
    {/render}
   </td>
  </tr>
  <tr>
   <td>{t}Password{/t}{$must}</td>
   <td>
    {render acl=$goFaxPasswordACL}
     <input type=password name="goFaxPassword" id="goFaxPassword" value="{$goFaxPassword}" >
    {/render}
  </td>
 </tr>
</table>

<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
<input type="hidden" name="goFaxServerPosted" value="1">
