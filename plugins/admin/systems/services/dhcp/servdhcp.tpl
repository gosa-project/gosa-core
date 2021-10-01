

{if $dns_take_over}
 <h3>{t}DHCP take over will take place when saving this entry. Choose 'Cancel' to abort.{/t}</h3>

{else}

 <table summary="{t}DHCP service{/t}" width="100%">
  
  {if $dhcp_server_list_cnt}
   <tr>
    <td style='width:100%;'>
     <b>{t}Take over DHCP configuration from following server{/t}</b>&nbsp;
     <select name='take_over_src' size=1>
      {html_options options=$dhcp_server_list}
     </select>
     <button type='submit' name='take_over'>{msgPool type=applyButton}</button>
    </td>
   </tr>
   
  {/if}
  <tr>
   <td style='width:100%;'>
    <h3>{t}DHCP sections{/t}</h3>
    {$DhcpList}
   </td>
  </tr>
 </table>
 
{/if}
<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
