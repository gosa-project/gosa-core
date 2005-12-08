<table summary="" style="width:100%;">
 <tr>
  <td style="width:48%; vertical-align:top;">
   <b><LABEL for="used_apps">{t}Used applications{/t}</LABEL></b>
  	{$UsedApps} 
	<input type="text" value="" size="35" name='CatName'>
	<input type="submit" value="{t}Add category{/t}" name='AddCat'>
  </td>
  <td>&nbsp;
  </td>
  <td style="width:48%;vertical-align:top;">
  <b><LABEL for="apps">{t}Available applications{/t}</LABEL></b>
  <br>
   {$List}
	<input type="submit" value="{t}Add{/t}" name='AddApps'>
 </td>
 </tr>
</table>

