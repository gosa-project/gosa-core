
<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>
 <tr>
  <td style="vertical-align:top;width:50%;border-right:1px solid #A0A0A0"> 
   <b>{t}Blocked numbers/lists{/t}</b>
   <br> 
   <select style="width:100%; height:300px;" name="block_list[]" size=15 multiple>
	    {html_options values=$cblocklist output=$cblocklist}
		<option disabled>&nbsp;</option>
   </select>
   <br>
   <input name="block_number" size=25 align="middle" maxlength=30 value="">
   <input type=submit value="{t}Add{/t}" name="add_blocklist_number" >&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_blocklist_number" >
  </td>
  <td>
	<b>{t}List of predefined blocklists{/t}</b><br>
	<table style="width:100%;height:300px;" summary="">
		<tr>
			<td valign="top">
					{$divSelectPredefined}
			</td>
		</tr>
	</table>
   <input type=submit value="{t}Add the list to the blocklists{/t}" name="add_blocklist"><br>
  </td>
 </tr>
</table>

<p class="plugbottom">
  <input type=submit name="edit_blocklists_finish" value="{t}Apply{/t}">
  &nbsp;
  <input type=submit name="edit_blocklists_cancel" value="{t}Cancel{/t}">
</p>

