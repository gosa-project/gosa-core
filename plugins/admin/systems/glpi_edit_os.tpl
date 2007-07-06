<h2>{t}Manage OS-types{/t}</h2>
{if $Method == "edit"}

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
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
		focus_field('is_string');
  -->
</script>
{else}
{t}Please enter a new name{/t}&nbsp;<input name="string" value="{$string}">
<p class="seperator">&nbsp;</p>
    <p>
        <div align="right" style="text-align: right;">
            <input type="submit" name="Rename_os_OK"       value="{t}Rename{/t}" >
            <input type="submit" name="Rename_Cancel"   value="{t}Cancel{/t}" >
        </div>
    </p>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('string');
  -->
</script>
{/if}


