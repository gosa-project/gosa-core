{* GOsa dhcp sharedNetwork - smarty template *}
<p><b>{t}Generic{/t}</b></p>
<table width="100%" border="0">
 <tr>

  <td width="50%">

   <table>
    <tr>
     <td>{t}Name{/t}{$must}</td>
     <td>
      <input id='cn' type='text' name='cn' size='25' maxlength='80' value='{$cn}' title='{t}Name for shared network{/t}'>
     </td>
    </tr>
    <tr>
     <td>{t}Server identifier{/t}</td>
     <td>
      <input type='text' name='server-identifier' size='25' maxlength='80' value='{$server_identifier}'
	title='{t}Propagated server identifier for this shared network{/t}'>
     </td>
    </tr>
   </table>
  
  </td>
  
  <td style="vertical-align:top;">
   <table>
    <tr>
     <td>
      <input type=checkbox name="authoritative" value="1" {$authoritative}
       title="{t}Select if this server is authoritative for this shared network{/t}">{t}Authoritative server{/t}
     </td>
    </tr>
   </table>
  </td>
  
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table width="100%">
 <tr>
  <td width="50%">
  
  <p><b>{t}Leases{/t}</b></p>
   <table>
    <tr>
     <td>{t}Default lease time{/t}</td>
     <td>
      <input type='text' name='default-lease-time' size='10' maxlength='25' value='{$default_lease_time}'
        title='{t}Default lease time{/t}'>&nbsp;{t}seconds{/t}
     </td>
    </tr>
    <tr>
     <td>{t}Max. lease time{/t}</td>
     <td>
      <input type='text' name='max-lease-time' size='10' maxlength='25' value='{$max_lease_time}'
        title='{t}Maximum lease time{/t}'>&nbsp;{t}seconds{/t}
     </td>
    </tr>
    <tr>
     <td>{t}Min. lease time{/t}</td>
     <td>
      <input type='text' name='min-lease-time' size='10' maxlength='25' value='{$min_lease_time}'
        title='{t}Minimum lease time{/t}'>&nbsp;{t}seconds{/t}
     </td>
    </tr>
   </table>
   
  </td>

  <td style="vertical-align:top">
   <p><b>{t}Access control{/t}</b></p>
   <table>
    <tr>
     <td>
     <input type=checkbox name="unknown-clients" value="1" {$allow_unknown_state}
        title="{t}Select if unknown clients should get dynamic IP addresses{/t}">{t}Allow unknown clients{/t}
     </td>
    </tr>
    <tr>
     <td>
     <input type=checkbox name="bootp" value="1" {$allow_bootp_state}
        title="{t}Select if bootp clients should get dynamic IP addresses{/t}">{t}Allow bootp clients{/t}
     </td>
    </tr>
    <tr>
     <td>
     <input type=checkbox name="booting" value="1" {$allow_booting_state}
        title="{t}Select if clients are allowed to boot using this DHCP server{/t}">{t}Allow booting{/t}
     </td>
    </tr>
   </table>
  </td>
  
 </tr> 
</table>

<p class="seperator">&nbsp;</p>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
