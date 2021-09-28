<h3>{t}Select the type of test you want to add{/t}</h3>

<b>{t}Available test types{/t}</b>&nbsp;:&nbsp;
<select name='test_type_to_add_{$ID}' size=1>
	{html_options options=$test_types_to_add}
</select>

<hr>
<br>
<div class='seperator' style='text-align:right; width:100%;'>
	<button type='submit' name='add_type'>{t}Continue{/t}</button>

    	&nbsp;
	<button type='submit' name='does_nothing'>{msgPool type=cancelButton}</button>

</div>
