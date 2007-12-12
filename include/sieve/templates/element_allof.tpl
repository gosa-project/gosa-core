<table class='sieve_allof_container'>
	<tr>
    	<td class='sieve_allof_left'>
            {if $Inverse}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}Not{/t}' title='{t}Inverse match{/t}'>
            {else}
                <input type='submit' name='toggle_inverse_{$ID}' value='{t}-{/t}' title='{t}Inverse match{/t}'>
            {/if}
			<br>
			<b>{t}All of{/t}</b>
		</td>
        <td class='sieve_allof_right'>
			{$Contents}
        </td>
	</tr>
</table>
