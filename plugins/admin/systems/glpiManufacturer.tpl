<h2>{t}Manage manufacturers{/t}</h2>
<select name="manufacturer" style="width:100%;" size="12">
  {html_options values=$ManuKeys output=$Manus}
</select>
<br>
<input name="add_manu" 		value="{t}Add{/t}" 		type="submit">
<input name="edit_manu" 	value="{t}Edit{/t}" 	type="submit">
<input name="remove_manu" 	value="{t}Remove{/t}" 	type="submit">

<p class="seperator">&nbsp;</p>
<div align="right">
<p>
	<input name="close_edit_manufacturer" value="{t}Close{/t}" type="submit">
</p>
</div>
