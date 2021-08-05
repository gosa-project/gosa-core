	{if $Expert}
    	{if $LastError != ""}
		<table width='100%' class='sieve_test_case' summary="{t}Sieve header{/t}">
        	<tr>
				<td colspan=4>
		            <font color='red'><b>{$LastError}</b></font>
				</td>
			</tr>
		</table>
        {/if}


<table width='100%' class='sieve_test_case' summary="{t}Sieve element{/t}">
	<tr>
		<td>
			<b>{t}Header{/t}</b>
		</td>
        <td style='text-align:right; '>

            <button type='submit' name='Toggle_Expert_{$ID}'>{t}Normal view{/t}</button>

        </td>
    </tr>
</table>
<table width='100%' summary="{t}Sieve element{/t}">
    <tr>
		<td>
            {t}Match type{/t}
        </td>
        <td>
            <select name='matchtype_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();' size=1>
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
                <button type='submit' name='toggle_inverse_{$ID}'>{t}Yes{/t}</button>

            {else}
                <button type='submit' name='toggle_inverse_{$ID}'>{t}No{/t}</button>

            {/if}
        </td>
    </tr>
    <tr>
        <td>
            {t}Comparator{/t}
        </td>
        <td>
            <select name='comparator_{$ID}' title='{t}Boolean value{/t}' size=1>
                {html_options options=$comparators selected=$comparator}
            </select>
        </td>
    </tr>
        {if $match_type == ":count" || $match_type == ":value"}
    <tr>
        <td>
            {t}operator{/t}
        </td>
        <td>
            <select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();' size=1>
                {html_options options=$operators selected=$operator}
            </select>
        </td>
    </tr>
        {/if}

	 <tr>
        <td colspan=2>&nbsp;</td>
    </tr>
   </table>
   <table width='100%' class='sieve_test_case' summary="{t}Sieve element{/t}">
    <tr>
        <td >
            {t}Address fields to include{/t}<br>
            <textarea style='width:100%;height:70px;' name='keys_{$ID}'>{$keys}</textarea>
        </td>
        <td >
            {t}Values to match for{/t}<br>
            <textarea style='width:100%;height:70px;' name='values_{$ID}'>{$values}</textarea>
        </td>
    </tr>
	</table>

	{else}
    	{if $LastError != ""}
		<table width='100%' class='sieve_test_case' summary="{t}Sieve element{/t}">
        	<tr>
				<td colspan=4>
		            <font color='red'><b>{$LastError}</b></font>
				</td>
			</tr>
		</table>
        {/if}

		
<table width='100%' class='sieve_test_case' summary="{t}Sieve element{/t}">
    <tr>
		{if $match_type == ":count" || $match_type == ":value"}
		<td style='width:350px;'>

		{else}
		<td style='width:200px;'>

		{/if}
            <b>{t}Header{/t}</b>

            {if $Inverse}
                <button type='submit' name='toggle_inverse_{$ID}'>{t}Not{/t}</button>

            {else}
                <button type='submit' name='toggle_inverse_{$ID}'>{t}-{/t}</button>

            {/if}
            &nbsp;
            <select onChange='document.mainform.submit();' name='matchtype_{$ID}' title='{t}Boolean value{/t}' size=1>
                {html_options options=$match_types selected=$match_type}
            </select>

            {if $match_type == ":count" || $match_type == ":value"}
            <select name='operator_{$ID}' title='{t}Boolean value{/t}' onChange='document.mainform.submit();' size=1>
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
        <td style='text-align:right; width:120px;'>

            <button type='submit' name='Toggle_Expert_{$ID}'>{t}Expert view{/t}</button>

        </td>
    </tr>

</table>
	{/if}
