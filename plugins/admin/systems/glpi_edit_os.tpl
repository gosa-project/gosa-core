<br>
     <select name="select_os" size="12" style="width:100%">
                            {html_options values=$OSKeys output=$OSs}
     </select><br>
	 <input name="is_string">
	 <input type="submit" name="add_os" 		value="{t}Add{/t}" >
	 <input type="submit" name="rename_os" 	value="{t}Rename{/t}" >
	 <input type="submit" name="del_os" 		value="{t}Delete{/t}" >

<p class="seperator">&nbsp;</p>
<div align="right">
<p>
<input name="close_edit_os" value="{t}Close{/t}" type="submit">
</p>
</div>
