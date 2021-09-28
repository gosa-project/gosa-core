<table width='100%' summary="{t}Sieve element clear{/t}">
	<tr>
		<td style='width:20px; background-color: #B8B8B8; vertical-align:middle;' >
			&nbsp;	
		</td>
		<td style='width:200px; background-color: #B8B8B8; vertical-align:middle;' >
            {image path='plugins/mail/images/sieve_move_object_up.png' 
                action="Move_Up_Object_{$ID}" title="{t}Move object up one position{/t}"}

            {image path='plugins/mail/images/sieve_move_object_down.png' 
                action="Move_Down_Object_{$ID}" title="{t}Move object down one position{/t}"}

            {image path='images/lists/trash.png' action="Remove_Object_{$ID}" 
                title="{t}Remove object{/t}"}
		</td>
    </tr>
    <tr>
		<td>
        </td>
        <td>
            %%OBJECT_CONTENT%%
        </td>
    </tr>
</table>
