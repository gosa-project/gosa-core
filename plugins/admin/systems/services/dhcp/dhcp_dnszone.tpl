{* GOsa dhcp host - smarty template *}
<h3>{t}DNS update zone{/t}</h3>

<table>
 <tr>
   <td>{t}DNS zone{/t}{$must}</td>
   <td>
     <select name='cn' > 
       {html_options options=$cns selected=$cn}
     </select>
   </td>
 </tr>
   <td>{t}DNS server{/t}{$must}</td>
   <td>
     <select name='dhcpDnsZoneServer'  >
       {html_options options=$dhcpDnsZoneServers selected=$dhcpDnsZoneServer}
     </select>
   </td>
 </tr>
 <tr>
   <td>{t}Key DN{/t}{$must}</td>
   <td>
     <select name='dhcpKeyDN'>
       {html_options options=$dhcpKeyDNs selected=$dhcpKeyDN}
     </select>
  </td>
 </tr>
</table>

<input type='hidden' name='dhcp_dnszone_posted' value='1'>

<hr>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	 focus_field('cn');
  -->
</script>
