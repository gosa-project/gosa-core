<table style='width:100%; ' summary="{t}Paste fax account settings{/t}">

 	<tr>
   		<td style='width:50%; '>

     		<h3>{t}Generic{/t}</h3>
     		<table summary="{t}Generic settings{/t}">
       			<tr>
         			<td>
					<label for="facsimileTelephoneNumber">{t}Fax{/t}</label>{$must}
				</td>
         			<td>
           				<input name="facsimileTelephoneNumber" id="facsimileTelephoneNumber" 
							size=40 maxlength=65 value="{$facsimileTelephoneNumber}" 
							title="{t}Fax number for GOfax to trigger on{/t}">
         			</td>
       			</tr>
                        <tr>
                                <td colspan=2>
                                      {t}Alternate fax numbers will not be copied{/t}
                                </td>
                        </tr>

			</table>
		</td>
	</tr>
</table>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('facsimileTelephoneNumber');
  -->
</script>
