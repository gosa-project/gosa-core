<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td> 
   <b>{t}Blocked numbers/lists{/t}</b>
   <br> 
   <select style="width:350px; height:300px;" name="block_list[]" size=15 multiple>
	{if $SELECT_cblocklist==true}
	    {html_options values=$cblocklist output=$cblocklist}
	{else}
		<option disabled>
	{/if}
   </select>
   <br>
   <input name="block_number" size=25 align="middle" maxlength=30 {$goFaxBlockListACL} value="">
   <input type=submit value="{t}Add{/t}" name="add_blocklist_number" {$goFaxBlockListACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_blocklist_number" {$goFaxBlockListACL}>
  </td>
  <td style="vertical-align:middle; text-align:center;">
   <input type=submit value="< {t}Add{/t}" name="add_blocklist" {$goFaxBlockListACL}>
  </td>
  <td style="vertical-align:top;">
   <b>{t}Select a specific department{/t}</b><br>
   <select name="department" size=1 onChange="mainform.submit()">
    {html_options options=$departments selected=$department}
   </select>
   <br>
   <br>
   <input type=submit name="goButton" value="{t}Choose{/t}">

   <b>{t}List of predefined blocklists{/t}</b>
   <select style="width:350px; height:250px;" name="predefined_list[]" size=15 multiple>
    {if $SELECT_list==true}
        {html_options values=$list}
    {else}
        <option>
    {/if}
   </select>
  </td>
 </tr>
</table>

<p class="plugbottom">
  <input type=submit name="edit_blocklists_finish" value="{t}Apply{/t}">
  &nbsp;
  <input type=submit name="edit_blocklists_cancel" value="{t}Cancel{/t}">
</p>

