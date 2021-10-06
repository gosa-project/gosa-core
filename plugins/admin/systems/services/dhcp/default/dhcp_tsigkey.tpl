{* GOsa dhcp host - smarty template *}
<h3>{t}DNS update key{/t}</h3>

<table width="100%">
 <tr>
  <td>{t}Key name{/t}{$must}</td>
  <td>
   <input id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}'
          title='{t}Name of Key{/t}'>
  </td>
 </tr>
 <tr>
  <td>{t}Encryption method{/t}</td>
  <td>
   <select name='dhcpKeyAlgorithm'>
    {html_options options=$dhcpKeyAlgorithms selected=$dhcpKeyAlgorithm}
   </select>
  </td>
 </tr>
 <tr>
  <td>{t}Key{/t}{$must}</td>
  <td>
   <input  type='text' name='dhcpKeySecret' size='30' maxlength='80' value='{$dhcpKeySecret}'>
  </td>
 </tr>
</table>

<input type='hidden' name='dhcp_tsigkey_posted' value='1'>
<hr>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	 focus_field('cn');
  -->
</script>
