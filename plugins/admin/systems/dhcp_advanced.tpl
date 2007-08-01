{* GOsa dhcp sharedNetwork - smarty template *}

<table width="100%">
 <tr>

  <td width="50%">
   <br>
   <b>{t}DHCP statements{/t}</b>
   <br>
   <select name='dhcpstatements' style="width:350px;" size="14">
    {html_options values=$dhcpstatements output=$dhcpstatements}
   </select>
   <br>
   <input type='text' name='addstatement' size='25' maxlength='80'>&nbsp;
   <input type='submit' name='add_statement' value='{t}Add{/t}'>&nbsp;
   <input type='submit' name='delete_statement' value='{t}Delete{/t}'> 
  </td>

  <td>
   <br>
   <b>{t}DHCP options{/t}</b>
   <br>
   <select name='dhcpoptions' style="width:350px;" size="14">
    {html_options values=$dhcpoptions output=$dhcpoptions}
   </select>
   <br>
   <input type='text' name='addoption' size='25' maxlength='80'>&nbsp;
   <input type='submit' name='add_option' value='{t}Add{/t}'>&nbsp;
   <input type='submit' name='delete_option' value='{t}Delete{/t}'> 
  </td>
 </tr>
</table>

