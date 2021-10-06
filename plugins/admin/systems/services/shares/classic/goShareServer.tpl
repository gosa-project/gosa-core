<h3>{t}Shares{/t}</h3>

<table summary="{t}Shares{/t}" style="width:100%">
 <tr>
  <td>
   {render acl=$goExportEntryACL  mode=read_active}
    <select style="width:100%" id="goExportEntry" name="goExportEntryList[]" size=12 multiple >
      {html_options values=$goExportEntry output=$goExportEntryKeys}
      <option disabled>&nbsp;</option>
    </select>
   {/render}
   <br>
   {render acl=$goExportEntryACL}
    <button type='submit' name='NewNfsAdd' id="NewNfsAddId" 
      {if !$createable} disabled {/if}>{msgPool type=addButton}</button>
   {/render}
   {render acl=$goExportEntryACL mode=read_active}
    <button type='submit' name='NewNfsEdit' id="NewNfsEditId">{t}Edit{/t}</button>
   {/render}
   {render acl=$goExportEntryACL}
    <button type='submit' name='DelNfsEnt' id="DelNfsEntId" 
      {if !$removeable} disabled {/if}>{msgPool type=delButton}</button>
   {/render}
  </td>
 </tr>
</table>

<hr>
<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
<input type="hidden" name="goShareServerPosted" value="1">
