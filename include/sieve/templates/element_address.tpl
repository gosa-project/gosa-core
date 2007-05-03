{if $Expert}
    	{if $LastError != ""}
		<table class='sieve_test_case'>
        	<tr>
				<td colspan=2>
		            <font color='red'><b>{$LastError}</b></font>
				</td>
			</tr>
		</table>
        {/if}

<table class='sieve_test_case'>
	<tr>
		<td >
			<b>{t}Address{/t}</b>
		</td>
        <td style='text-align:right; vertical-align:top;'>
			<input type='submit' name='Toggle_Expert_{$ID}' value='{t}Normal view{/t}'>
		</td>
	</tr>
</table>
<table>
	<tr>
		<td >
			{t}Match type{/t}
		</td>
		<td>
			<select name='matchtype_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'> 
				{html_options options=$match_types selected=$match_type}
			</select>

		</td>
	</tr>
	<tr>
		<td>
			{t}Invert test{/t}?
		</td>
		<td>
			{if $Inverse}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}Yes{/t}'>
			{else}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}No{/t}'>
			{/if}
		</td>
	</tr>
	<tr>
		<td>
			{t}Part of address that should be used{/t}
		</td>
		<td>
			<select name='address_part_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$address_parts selected=$address_part}
			</select>
		</td>
	</tr>
	<tr>
		<td>
			{t}Comparator{/t}
		</td>
		<td>
			<select name='comparator_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$comparators selected=$comparator}
			</select>
		</td>
	</tr>
		{if $match_type == ":count" || $match_type == ":value"}
	<tr>
		<td>
			{t}Operator{/t}
		</td>
		<td>
			<select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'>
				{html_options options=$operators selected=$operator}
			</select>
		</td>
	</tr>
		{/if}
	<tr>
		<td colspan=2>&nbsp;</td>
	</tr>
</table>
<table style='width:100%;'>
    <tr>
        <td style='width:50%;'>
            {t}Address fields to include{/t}<br>
            <textarea style='width:100%;height:70px;' name='keys_{$ID}'>{$keys}</textarea>
        </td>
        <td style='width:50%;'>
            {t}Values to match for{/t}<br>
            <textarea style='width:100%;height:70px;' name='values_{$ID}'>{$values}</textarea>
        </td>
    </tr>
</table>
	{else}

    	{if $LastError != ""}
		<table class='sieve_test_case'>
        	<tr>
				<td colspan=2>
		            <font color='red'><b>{$LastError}</b></font>
				</td>
			</tr>
		</table>
        {/if}

<table class='sieve_test_case'>
	<tr>
		{if $match_type == ":count" || $match_type == ":value"}
		<td style='vertical-align:top; width:350px;'>
		{else}
		<td style='vertical-align:top; width:200px;'>
		{/if}
			<b>{t}Address{/t}</b>
		
			{if $Inverse}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}'>
			{else}
				<input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}'>
			{/if}
			&nbsp;
			<select onChange='document.mainform.submit();' name='matchtype_{$ID}' title='{t}Boolean value{/t}'> 
				{html_options options=$match_types selected=$match_type}
			</select>

			{if $match_type == ":count" || $match_type == ":value"}
			<select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();'>
				{html_options options=$operators selected=$operator}
			</select>
			{/if}
		</td>
		<td>
			<textarea style='width:100%;height:40px;' name='keys_{$ID}'>{$keys}</textarea>
		</td>
		<td>
			<textarea style='width:100%;height:40px;' name='values_{$ID}'>{$values}</textarea>
		</td>
		<td style='text-align:right; vertical-align:top; width:120px;'>
			<input type='submit' name='Toggle_Expert_{$ID}' value='{t}Expert view{/t}'>
		</td>
	</tr>
</table>
	{/if}
