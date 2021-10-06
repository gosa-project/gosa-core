{* GOsa dhcp host - smarty template *}

<table width="100%" summary="{t}DHCP host configuration{/t}">
 <tr>
  <td class='right-border' style='width:50%;'>

   <h3>{t}Generic{/t}</h3>
   <table  summary="{t}DHCP host configuration{/t}">
    <tr>
     <td>{t}Name{/t}{$must}</td>
     <td>
      {render acl=$acl}
       <input {if $realGosaHost} disabled {/if} id='cn' type='text' name='cn' 
         value='{$cn}' title='{t}Name of host{/t}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Fixed address{/t}</td>
     <td>
      {render acl=$acl}
       <input {if $realGosaHost} disabled {/if} type='text' name='fixedaddr'
         value='{$fixedaddr}' title='{t}Use host name or IP-address to assign fixed address{/t}'>
      {/render}
     </td>
    </tr>
   </table>

  </td>
  <td>
   <h3>{t}Hardware{/t}</h3>

   <table summary="{t}DHCP host configuration{/t}">
    <tr>
     <td>{t}Hardware type{/t}</td>
     <td>
      {render acl=$acl}
       <select name='hwtype'  {if $realGosaHost} disabled {/if}size=1>
       {html_options options=$hwtypes selected=$hwtype}
      </select>
     {/render}
    </td>
   </tr>
   <tr>
    <td>{t}Hardware address{/t}{$must}</td>
    <td>
     {render acl=$acl}
      <input  {if $realGosaHost}  disabled {/if} type='text' 
        name='dhcpHWAddress' value='{$dhcpHWAddress}'>
     {/render}
    </td>
   </tr>
  </table>

 </td>
</tr>
</table>

<input type='hidden' name='dhcp_host_posted' value='1'>

<hr>
<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
 <!-- // First input field on page	 
  focus_field('cn');  
 -->
</script>
