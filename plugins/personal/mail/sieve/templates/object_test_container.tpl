<table width='100%' class='sieve_test_container'  summary="{t}Sieve test case{/t}">
	<tr>
		<td style='width:20px; ; text-align:center; '>	

			{if $DisplayAdd}
				{image path="plugins/mail/images/sieve_add_test.png" action="Add_Test_Object_{$ID}"
					title="{t}Add object{/t}"}
			{/if}
			{if $DisplayDel}
				{image path="plugins/mail/images/sieve_del_object.png" action="Remove_Test_Object_{$ID}"
					title="{t}Remove object{/t}"}
			{/if}
		</td>
		<td>
			%%OBJECT_CONTENT%%
		</td>
	</tr>
</table>
