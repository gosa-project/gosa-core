<br>
     <select name="select_type" size="12" style="width:100%">
                            {html_options values=$SystemTypeKeys output=$SystemTypes}
     </select><br>
	 <input name="type_string">
	 <input type="submit" name="add_type" 		value="{t}Add{/t}" >
	 <input type="submit" name="rename_type" 	value="{t}Rename{/t}" >
	 <input type="submit" name="del_type" 		value="{t}Delete{/t}" >

<p class="seperator">&nbsp;</p>
<div align="right">
<p>
<input name="close_edit_type" value="{t}close{/t}" type="submit">
</p>
</div>
