{* GOsa dhcp sharedNetwork - smarty template *}
<table width="100%">
 <tr>
  <td width="50%" style="vertical-align:top">
   <p><b>{t}Network configuration{/t}</b></p>
   <table>
    <tr>
     <td>{t}Router{/t}</td>
     <td>
      <input id='routers' type='text' name='routers' size='25' maxlength='80' value='{$routers}'
              title='{t}Enter name or IP address of router to be used in this section{/t}'>
     </td>
    </tr>
    <tr>
     <td>{t}Netmask{/t}</td>
     <td>
      <input type='text' name='subnet_mask' size='18' maxlength='15' value='{$subnet_mask}'>
     </td>
    </tr>
    <tr>
     <td>{t}Broadcast{/t}</td>
     <td>
      <input type='text' name='broadcast_address' size='18' maxlength='15' value='{$broadcast_address}'>
     </td>
    </tr>
   </table>
   <br>
   <br>
   <p><b>{t}Bootup{/t}</b></p>
   <table>
    <tr>
     <td>{t}Filename{/t}</td>
     <td>
      <input type='text' name='filename' size='25' maxlength='80' value='{$filename}'
              title='{t}Enter name of file that will be loaded via tftp after client has started{/t}'>
     </td>
    </tr>
    <tr>
     <td>{t}Next server{/t}</td>
     <td>
      <input type='text' name='nextserver' size='25' maxlength='80' value='{$nextserver}'
              title='{t}Enter name of server to retrieve bootimages from{/t}'>
     </td>
    </tr>
   </table>
  
  </td>
  <td style="vertical-align:top">
   <p><b>{t}Domain Name Service{/t}</b></p>
   <table>
    <tr>
     <td>{t}Domain{/t}</td>
     <td>
      <input type='text' name='domain' size='25' maxlength='80' value='{$domain}'
              title='{t}Name of domain{/t}'>
     </td>
    </tr>
    <tr>
     <td colspan=2>
      <br>
      {t}DNS server{/t}<br>
      <select name='dnsserver'  title='{t}List of DNS servers to be propagated{/t}' style="width:350px;" size="4">
       {html_options options=$dnsservers}
      </select>
      <br>
      <input type='text' name='addserver' size='25' maxlength='80' title='{t}DNS server do be added{/t}'>&nbsp;
      <input type='submit' name='add_dns' value='{t}Add{/t}' title='{t}Click here add the selected server to the list{/t}'>
      <input type='submit' name='delete_dns' value='{t}Delete{/t}' title='{t}Click here remove the selected servers from the list{/t}'>
     </td>
    </tr>
    <tr>
     <td colspan=2>
      <p><b>{t}Domain Name Service options{/t}</b></p>
      <input type=checkbox name="autohost" value="1" {$autohost}>{t}Assign hostnames found via reverse mapping{/t}
      <br>
      <input type=checkbox name="autohostdecl" value="1" {$autohostdecl}>{t}Assign hostnames from host declarations{/t}
     </td>
    </tr>
   </table>
  
  </td>
 </tr>
</table>
<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
     focus_field('cn','routers');
  -->
</script>

