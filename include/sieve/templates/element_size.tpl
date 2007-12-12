<table class='sieve_test_case'>
	<tr>
		<td>
			<b>{t}Size{/t}</b>
			{if $LastError != ""}
				<font color='red'>{$LastError}</font>
				<br>
			{/if}			

     		{if $Inverse}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}' title='{t}Inverse match{/t}'>
            {else}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}' title='{t}Inverse match{/t}'>
            {/if}

			<select name='Match_type_{$ID}' title='{t}Select match type{/t}'>
				{html_options options=$Match_types selected=$Match_type}
			</select>
			<input type='text' name='Value_{$ID}' value='{$Value}'>
			<select name='Value_Unit_{$ID}' title='{t}Select value unit{/t}'>
				{html_options options=$Units selected=$Value_Unit}
			</select>
		</td>
	</tr>
</table>
