<table width='100%' class='sieve_test_case' summary="{t}Sieve test case: size{/t}">
	<tr>
		<td>
			<b>{t}Size{/t}</b>
			{if $LastError != ""}
				<font color='red'>{$LastError}</font>
				<br>
			{/if}			

     		{if $Inverse}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}Not{/t}</button>

            {else}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}-{/t}</button>

            {/if}

			<select name='Match_type_{$ID}' title='{t}Select match type{/t}' size=1>
				{html_options options=$Match_types selected=$Match_type}
			</select>
			<input type='text' name='Value_{$ID}' value='{$Value}'>
			<select name='Value_Unit_{$ID}' title='{t}Select value unit{/t}' size=1>
				{html_options options=$Units selected=$Value_Unit}
			</select>
		</td>
	</tr>
</table>
