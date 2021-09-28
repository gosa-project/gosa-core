<table width='100%' class='object_container_container' summary="{t}Sieve element{/t}">
	<tr>
	    <td style='width:20px;background-color: #B8B8B8'>
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
		 <td style=' background-color: #B8B8B8'>
			<select name='element_type_{$ID}' size=1>
				<option value=''>&lt;{t}choose element{/t}&gt;</option>
				<option value='sieve_keep'>{t}Keep{/t}</option>
				<option value='sieve_comment'>{t}Comment{/t}</option>
				<option value='sieve_fileinto'>{t}File into{/t}</option>
				<option value='sieve_keep'>{t}Keep{/t}</option>
				<option value='sieve_discard'>{t}Discard{/t}</option>
				<option value='sieve_redirect'>{t}Redirect{/t}</option>
				<option value='sieve_reject'>{t}Reject{/t}</option>
				<option value='sieve_require'>{t}Require{/t}</option>
				<option value='sieve_stop'>{t}Stop{/t}</option>
				<option value='sieve_vacation'>{t}Vacation message{/t}</option>
				<option value='sieve_if'>{t}If{/t}</option>
				<option value='sieve_else'>{t}Else{/t}</option>
				<option value='sieve_elsif'>{t}Else If{/t}</option>
			</select>

            {image path="plugins/mail/images/sieve_move_object_up.png[new]" 
                action="Add_Object_Top_{$ID}" title="{t}Add a new object above this one.{/t}"}
            {image path="plugins/mail/images/sieve_move_object_down.png[new]" 
                action="Add_Object_Bottom_{$ID}" title="{t}Add a new object below this one.{/t}"}
		</td>
	</tr>
	<tr>
		<td class='object_container_cell_bottom_left'>
		</td>
		<td colspan=2>
			%%OBJECT_CONTENT%%
		</td>
	</tr>
</table>
