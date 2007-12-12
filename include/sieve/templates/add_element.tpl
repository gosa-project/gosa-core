<h2>{t}Add a new element{/t}</h2>
{t}Please select the type of element you want to add{/t}
<br>
<select name='element_type'>
	{html_options options=$element_types selected=$element_type }
</select>

<p class='seperator'>&nbsp;</p>
<br>
<div class='seperator' style='text-align:right; width:100%;'>
    <input type='submit' name='select_new_element_type' value='{t}Continue{/t}'>
    &nbsp;
    <input type='submit' name='select_new_element_type_cancel' value='{t}Abort{/t}'>
</div>
