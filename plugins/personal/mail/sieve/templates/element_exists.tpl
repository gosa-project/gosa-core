<table width='100%' class='sieve_test_case' summary="{t}Sieve filter{/t}">
    <tr>
        <td style='width:200px;'>

            {if $LastError != ""}
                <font color='red'>{$LastError}</font>
                <br>
            {/if}
            <b>{t}Exists{/t}</b>
            {if $Inverse}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}Not{/t}</button>

            {else}
                <button type='submit' name='toggle_inverse_{$ID}' title="{t}Inverse match{/t}">{t}-{/t}</button>

            {/if}

		</td>
		<td>
            <textarea style='width:99%;height:20px;' name='Values_{$ID}'>{$Values}</textarea>
		</td>
    </tr>
</table>
