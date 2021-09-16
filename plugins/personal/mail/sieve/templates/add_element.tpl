<h3>{t}Add a new element{/t}</h3>
{t}Please select the type of element you want to add{/t}
<br>
<select name='element_type' size=1>
	{html_options options=$element_types selected=$element_type}
</select>

<hr>
<br>
<div class='seperator' style='text-align:right; width:100%;'>
    <button type='submit' name='select_new_element_type'>{t}Continue{/t}</button>

    &nbsp;
    <button type='submit' name='select_new_element_type_cancel'>{t}Abort{/t}</button>

</div>
