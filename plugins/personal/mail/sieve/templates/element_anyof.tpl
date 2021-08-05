<table width='100%' class='sieve_anyof_container' summary="{t}Sieve filter{/t}">
	<tr>
    	<td class='sieve_anyof_left'>
            {if $Inverse}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}Not{/t}</button>

            {else}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">-</button>

            {/if}
			<br>
			<b>{t}Any of{/t}</b>
		</td>
        <td class='sieve_anyof_right'>
			{$Contents}
        </td>
	</tr>
</table>
