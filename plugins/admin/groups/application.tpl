<table style="width:100%;">
 <tr>
  <td style="width:45%; vertical-align:top;">
   <b>{t}Used applications{/t}</b>
   <br>
   <select style="width:350px; height:275;" name="used_apps[]" size=15 multiple>
    {html_options options=$used_apps}
	<option disabled></option>
   </select>
   <br>
   <input type=submit name="edit_options" value="{t}Edit parameters{/t}" title="{t}Edit optional application parameters{/t}">
  </td>

  <td style="width:10%; vertical-align:middle; text-align:center;">
    <input type=submit name="add_app" value="&larr;" style="width:50px;">
    <br>
    <br>
    <input type=submit name="del_app" value="&rarr;" style="width:50px;">
  </td>
  <td style="width:45%; vertical-align:top;">
  <b>{t}Available applications{/t}</b>
  <br>
  <select style="width:350px; height:275px;" name="apps[]" size=15 multiple>
    {html_options options=$apps}
	<option disabled></option>
  </select>
 </td>
 </tr>
</table>

