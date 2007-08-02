{* GOsa dhcp sharedNetwork - smarty template *}

<p class='seperator'></p>
<br>

{if $show_advanced}

<input type='submit' name='hide_advanced' value='{t}Hide advanced settings{/t}'>

<table width="100%">
 <tr>

  <td width="50%">
   <br>
   <b>{t}DHCP statements{/t}</b>
   <br>
   <select name='dhcpstatements' style="width:100%;" size="14">
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
   <select name='dhcpoptions' style="width:100%;" size="14">
    {html_options values=$dhcpoptions output=$dhcpoptions}
   </select>
   <br>
   <input type='text' name='addoption' size='25' maxlength='80'>&nbsp;
   <input type='submit' name='add_option' value='{t}Add{/t}'>&nbsp;
   <input type='submit' name='delete_option' value='{t}Delete{/t}'> 
  </td>
 </tr>
</table>

{else}

<input type='submit' name='show_advanced' value='{t}Show advanced settings{/t}'>

{/if}
<p class='seperator'>&nbsp;</p>
