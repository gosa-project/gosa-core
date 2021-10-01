

{if $dns_take_over}
 <table summary="" width="100%">
  <tr>
   <td style='width:100%;'>
    <h3>{t}DNS take over initiated{/t}
    </h3>
    {$warning}{t}This includes 'all' DNS zones that are located within this server. Please double check if your really want to do this.{/t}
    <p>
     {$warning2}
     <br>
     <button type='submit' name='cancel_take_over'>
     {msgPool type=cancelButton}</button>
    </p>
    <p>{t}Following objects will be taken over{/t}&nbsp;:
    </p>
    <p>
     <pre>
      {$info}
     </pre>
    </p>
   </td>
  </tr>
 </table>
 {else}
 <table summary="" width="100%">
  
  {if $dns_server_list_cnt}
   <tr>
    <td style='width:100%;'>
     <b>{t}Take over DNS configuration from{/t}</b>&nbsp;
     <select name='take_over_src' size=1>
      {html_options options=$dns_server_list}
     </select>
     <button type='submit' name='take_over'>{t}Take over{/t}</button>
    </td>
   </tr>
   
  {/if}
  <tr>
   <td style='width:100%;'>
    <h3>{t}Zones{/t}
    </h3>
    {$ZoneList}
    
    {if $is_createable}
     <button type='submit' name='AddZone'>
     {msgPool type=addButton}</button>
     {else}
     <input type="button" value="
     {msgPool type=addButton}" disabled>
     
    {/if}
   </td>
  </tr>
 </table>
 <script language="JavaScript" type="text/javascript"><!-- // First input field on page      focus_field('AddZone');    --></script>
 
{/if}
<hr>
<div class="plugin-actions">
 <button type='submit' name='SaveService'>
 {msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>
 {msgPool type=cancelButton}</button>
</div>