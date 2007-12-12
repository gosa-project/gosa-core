<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Shares{/t}</h2>
   <table summary="" style="width:100%">
    <tr>
     <td>
{render acl=$nameACL  mode=read_active}
        <select style="width:100%" id="goExportEntry" name="goExportEntryList[]" size=12 multiple >
            {html_options values=$goExportEntry output=$goExportEntryKeys}
            <option disabled>&nbsp;</option>
        </select>
{/render}
    <br>
	
{render acl=$nameACL}
        <input type="submit"    value="{t}Add{/t}"  	name="NewNfsAdd"   id="NewNfsAddId" {if !$createable} disabled {/if}>
{/render}
{render acl=$nameACL mode=read_active}
        <input type="submit"    value="{t}Edit{/t}"     name="NewNfsEdit"  id="NewNfsEditId">
{/render}
{render acl=$nameACL}
        <input type="submit"    value="{t}Delete{/t}"   name="DelNfsEnt"   id="DelNfsEntId" {if !$removeable} disabled {/if}>
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
<input type="hidden" name="goShareServerPosted" value="1">
