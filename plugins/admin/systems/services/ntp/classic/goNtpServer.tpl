<h3>{t}Time server{/t}</h3>

<table summary="" style="width:100%">
 <tr>
  <td>
   {render acl=$goTimeSourceACL}
    <select style="width:100%;" id="goTimeEntry" name="goTimeSource[]" size=8 multiple>
     {html_options values=$goTimeSource output=$goTimeSource}
     <option disabled>&nbsp;</option>
    </select>
   {/render}
   <br>
   {render acl=$goTimeSourceACL}
    <input type="text" name="NewNTPExport"  id="NewNTPExportId">
   {/render}
   {render acl=$goTimeSourceACL}
    <button type='submit' name='NewNTPAdd' id="NewNTPAddId">{msgPool type=addButton}</button>
   {/render}
   {render acl=$goTimeSourceACL}
    <button type='submit' name='DelNTPEnt' id="DelNTPEntId">{msgPool type=delButton}</button>
   {/render}
  </td>
 </tr>
</table>

<hr>
<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button> 
</div>
