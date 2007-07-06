<br>
     <select name="select_type_cartridge" size="12" style="width:100%">
                            {html_options values=$PrinterTypeKeys output=$PrinterTypes}
     </select><br>
	 <input name="cartridge_type_string">
	 <input type="submit" name="add_cartridge_type" 		value="{t}Add{/t}" >
	 <input type="submit" name="rename_cartridge_type" 	value="{t}Rename{/t}" >
	 <input type="submit" name="del_cartridge_type" 		value="{t}Delete{/t}" >

<p class="seperator">&nbsp;</p>
<div align="right">
<p>
<input name="close_edit_type_cartridge" value="{t}Close{/t}" type="submit">
</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cartridge_type_string');
  -->
</script>

