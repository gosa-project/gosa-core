{* GOsa dhcp sharedNetwork - smarty template *}

<table width="100%" summary="{t}Network configuration{/t}">
 <tr>
  <td style='width:50%' class='right-border'>

   <h3>{t}Network configuration{/t}</h3>

   <table summary="{t}Network configuration{/t}">
    <tr>
     <td>{t}Router{/t}</td>
     <td>
      {render acl=$acl}
       <input id='routers' type='text' name='routers' value='{$routers}' 
         title='{t}Enter name or IP address of router to be used in this section{/t}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Net mask{/t}</td>
     <td>
      {render acl=$acl}
       <input type='text' name='subnet_mask' value='{$subnet_mask}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Broadcast address{/t}
     </td>
     <td>
      {render acl=$acl}
       <input type='text' name='broadcast_address' value='{$broadcast_address}'>
      {/render}
     </td>
    </tr>
   </table>

   <hr>

   <h3>{t}Boot up{/t}</h3>

   <table summary="{t}Network configuration{/t}">
    <tr>
     <td>{t}Filename{/t}</td>
     <td>
      {render acl=$acl}
       <input type='text' name='filename' value='{$filename}'
         title='{t}Enter name of file that will be loaded via TFTP after client has started{/t}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Next server{/t}</td>
     <td>
      {render acl=$acl}
       <input type='text' name='nextserver' value='{$nextserver}' 
         title='{t}Enter name of server to retrieve boot images from{/t}'>
      {/render}
     </td>
    </tr>
   </table>

  </td>
  <td>

   <h3>{t}Domain Name Service{/t}</h3>

   <table summary="{t}Network configuration{/t}">
    <tr>
     <td>{t}Domain{/t}</td>
     <td>
      {render acl=$acl}
       <input type='text' name='domain' value='{$domain}' title='{t}Name of domain{/t}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td colspan=2>
      <br>{t}DNS server{/t}
      <br>
      {render acl=$acl}
       <select name='dnsserver' title='{t}List of DNS servers to be propagated{/t}' 
          style="width:350px;" size="4">
        {html_options options=$dnsservers}
       </select>
      {/render}
      <br>
      {render acl=$acl}
       <input type='text' name='addserver' title='{t}DNS server do be added{/t}'>&nbsp;
      {/render}
      {render acl=$acl}
       <button type='submit' name='add_dns' title="{t}Click here add the selected server to the list{/t}">
       {msgPool type=addButton}</button>
      {/render}
      {render acl=$acl}
       <button type='submit' name='delete_dns' 
        title="{t}Click here remove the selected servers from the list{/t}">{msgPool type=delButton}</button>
      {/render}

      <hr>

      <h3>{t}Domain Name Service options{/t}</h3>
      {render acl=$acl}
       <input type=checkbox name="autohost" value="1" {$autohost}>{t}Assign host names found via reverse mapping{/t}
      {/render}
      <br>
      {render acl=$acl}
       <input type=checkbox name="autohostdecl" value="1" {$autohostdecl}>{t}Assign host names from host declarations{/t}
      {/render}
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
