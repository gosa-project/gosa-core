<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="0" border="0">
 	<tr>
   		<td style="width:50%; vertical-align:top;">
     		<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" />&nbsp;{t}Generic{/t}</h2>
     		<table summary="">
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
