
<table style='width:100%; ' summary="{t}Blocklist settings{/t}">

 <tr>
  <td style='width:50%;' class='right-border'> 

   <b>{t}Blocked numbers/lists{/t}</b>
   <br> 
   <select style="width:100%; height:300px;" name="block_list[]" size=15 multiple>
	    {html_options values=$cblocklist output=$cblocklist}
		<option disabled>&nbsp;</option>
   </select>
   <br>
   <input type='text' name="block_number" size=25 align="middle" maxlength=30 value="">
   <button type='submit' name='add_blocklist_number'>{msgPool type=addButton}</button>&nbsp;

   <button type='submit' name='delete_blocklist_number'>{msgPool type=delButton}</button>

  </td>
  <td>
	<b>{t}List of predefined blacklists{/t}</b><br>
	<table style="width:100%;height:300px;" summary="{t}List of blocked numbers{/t}">
		<tr>
			<td valign="top">
					{$predefinedList}
			</td>
		</tr>
	</table>
   <button type='submit' name='add_blocklist'>{t}Add the list to the blacklists{/t}</button>
<br>
  </td>
 </tr>
</table>

<hr>
<div class="plugin-actions">
  <button type='submit' name='edit_blocklists_finish'>{msgPool type=applyButton}</button>

  <button type='submit' name='edit_blocklists_cancel'>{msgPool type=cancelButton}</button>

</div>
