<table summary="" style="width:100%;">
 <tr>
  <td style="width:48%; vertical-align:top;">
   <b><LABEL for="used_apps">{t}Used applications{/t}</LABEL></b>
   <br>
   <select id="used_apps" style="width:100%;height:300px;" name="used_apps[]" size=15 multiple>
    {html_options options=$used_apps}
	<option disabled>&nbsp;</option>
   </select>
   <br>
   <input type=submit name="edit_options" value="{t}Edit parameters{/t}" title="{t}Edit optional application parameters{/t}">
   <input type=submit name="del_app" value="{t}Remove{/t}">
  </td>
  <td>&nbsp;
  </td>
  <td style="width:48%;vertical-align:top;">
  <b><LABEL for="apps">{t}Available applications{/t}</LABEL></b>
  <br>
   {$List}
 </td>
 </tr>
</table>

