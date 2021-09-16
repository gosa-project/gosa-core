<table width='100%' class='sieve_allof_container' summary="{t}Sieve filter{/t}">
	<tr>
    	<td class='sieve_allof_left'>
            {if $Inverse}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}Not{/t}</button>

            {else}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}-{/t}</button>

            {/if}
			<br>
			<b>{t}All of{/t}</b>
		</td>
        <td class='sieve_allof_right'>
			{$Contents}
        </td>
	</tr>
</table>
