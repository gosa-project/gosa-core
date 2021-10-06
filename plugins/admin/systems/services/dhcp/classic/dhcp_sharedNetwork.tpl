{* GOsa dhcp sharedNetwork - smarty template *}

<h3>{t}Generic{/t}</h3>

<table width="100%" border="0" summary="{t}DHCP shared network{/t}">
 <tr>
  <td width="50%">

   <table summary="{t}DHCP shared network{/t}">
    <tr>
     <td>{t}Name{/t}{$must}</td>
     <td>
      {render acl=$acl}
       <input id='cn' type='text' name='cn' 
        value='{$cn}' title='{t}Name for shared network{/t}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Server identifier{/t}
     </td>
     <td>
      {render acl=$acl}
       <input type='text' name='server-identifier' 
        value='{$server_identifier}'	title='{t}Propagated server identifier for this shared network{/t}'>
      {/render}
     </td>
    </tr>
   </table>

  </td>
  <td>

    {render acl=$acl}
     <input type=checkbox name="authoritative" 
      value="1" {if $authoritative} checked {/if} 
      title="{t}Select if this server is authoritative for this shared network{/t}">{t}Authoritative server{/t}
    {/render}

  </td>
 </tr>
</table>

<hr>
<table width="100%"  summary="{t}DHCP shared network{/t}">
 <tr>
  <td width="50%">
   <h3>{t}Leases{/t}</h3>

   <table  summary="{t}DHCP shared network{/t}">
    <tr>
     <td>{t}Default lease time{/t}
     </td>
     <td>
      {render acl=$acl}
       <input type='text' name='default-lease-time'
        value='{$default_lease_time}' title='{t}Default lease time{/t}'>&nbsp;{t}seconds{/t}
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Max. lease time{/t}</td>
     <td>
      {render acl=$acl}
       <input type='text' name='max-lease-time'
        value='{$max_lease_time}' title='{t}Maximum lease time{/t}'>&nbsp;{t}seconds{/t}
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Min. lease time{/t}</td>
     <td>
      {render acl=$acl}
       <input type='text' name='min-lease-time'
        value='{$min_lease_time}' title='{t}Minimum lease time{/t}'>&nbsp;{t}seconds{/t}
      {/render}
     </td>
    </tr>
   </table>

  </td>
  <td>

   <h3>{t}Access control{/t}</h3>
   <table  summary="{t}DHCP shared network{/t}">
    <tr>
     <td>
      {render acl=$acl}
       <input type=checkbox name="unknown-clients" 
        value="1" {$allow_unknown_state} title="{t}Select if unknown clients should get dynamic IP addresses{/t}">{t}Allow unknown clients{/t}
      {/render}
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$acl}
       <input type=checkbox name="bootp" 
        value="1" {$allow_bootp_state} title="{t}Select if BOOTP clients should get dynamic IP addresses{/t}">{t}Allow BOOTP clients{/t}
      {/render}
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$acl}
       <input type=checkbox name="booting" 
        value="1" {$allow_booting_state} title="{t}Select if clients are allowed to boot using this DHCP server{/t}">{t}Allow booting{/t}
      {/render}
     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>
<hr>

<!-- Place cursor in correct field -->
<script language="JavaScript" type="text/javascript">
 <!-- // First input field on page  
  document.mainform.cn.focus();  
 -->
</script>
