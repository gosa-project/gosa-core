<br>
{if $Method == "edit"}
	 <select name="select_type" size="12" style="width:100%">
							{html_options values=$PrinterTypeKeys output=$PrinterTypes}
	 </select><br>
	 <input name="type_string">
	 <input type="submit" name="add_type" 		value="{t}Add{/t}" >
	 <input type="submit" name="rename_type" 	value="{t}Rename{/t}" >
	 <input type="submit" name="del_type" 		value="{t}Delete{/t}" >

	<p class="seperator">&nbsp;</p>
	<div align="right">
		<p>
			<input name="close_edit_type" value="{t}Close{/t}" type="submit">
		</p>
	</div>
	<script language="JavaScript" type="text/javascript">
	  <!-- // First input field on page
		focus_field('type_string');
	  -->
	</script>

{else}

	{t}Please enter a new name{/t}&nbsp;<input name="string" value="{$string}">
	<p class="seperator">&nbsp;</p>
    <p>
        <div align="right" style="text-align: right;">
            <input type="submit" name="Rename_PType_OK"       value="{t}Rename{/t}" >
            <input type="submit" name="Rename_Cancel"   value="{t}Cancel{/t}" >
        </div>
    </p>
	<script language="JavaScript" type="text/javascript">
	  <!-- // First input field on page
		focus_field('string');
	  -->
	</script>

{/if}

