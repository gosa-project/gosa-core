<p>
 {t}This step checks if your PHP server has all required modules and configuration settings.{/t}
</p>

<table style='width:100%;' summary='{t}Inspection{/t}'>
 <tr>
  <td style='text-align:top; width: 50%;'>
   <h2>{t}PHP module and extension checks{/t}</h2>

   <table cellspacing='0' class='sortableListContainer' style='border:1px solid #CCC; width:100%;' summary='{t}Basic checks{/t}'>
    {foreach from=$basic item=val key=key}
     {if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
      <tr class='entry_container_info'>
     {else}
      <tr class='entry_container'>
     {/if}
       
     <td>{$basic[$key].NAME}</td>

     {if $basic[$key].RESULT}
       <td style='color:#0A0'>{t}Ok{/t}</td>
      </tr>
     {else}
        {if $basic[$key].MUST}
         <td style='color:red'>{t}Error{/t}</td>
        {else}
         <td style='color:orange'>{t}Warning{/t}</td>
        {/if}
       </tr>

      {if $basic[$key].SOLUTION != ""}
       <tr>       
        <td colspan=2>{$basic[$key].SOLUTION}</td>
       </tr>
       <tr>       
        <td colspan=2>
         {if $basic[$key].MUST}
          <b>{t}GOsa will NOT run without fixing this.{/t}</b>
         {else}
          <b>{t}GOsa will run without fixing this.{/t}</b>
         {/if}
        </td>
       </tr>
      {/if}
     {/if}
    {/foreach}
   </table>
  </td>
  <td>
   <h2>{t}PHP setup configuration{/t} (<a style='text-decoration:underline' href='?info' target='_blank'>{t}show information{/t})</a></h2>
   <table cellspacing='0' class='sortableListContainer' style='border:1px solid #CCC; width:100%;' summary='{t}Extended checks{/t}'>
    {foreach from=$config item=val key=key}
     {if $config[$key].SOLUTION != "" && !$config[$key].RESULT}
      <tr class='entry_container_info'>
     {else}
      <tr class='entry_container'>
     {/if}
       
     <td>{$config[$key].NAME}</td>

     {if $config[$key].RESULT}
       <td style='color:#0A0'>{t}Ok{/t}</td>
      </tr>
     {else}
        {if $config[$key].MUST}
         <td style='color:red'>{t}Error{/t}</td>
        {else}
         <td style='color:orange'>{t}Warning{/t}</td>
        {/if}
       </tr>

      {if $config[$key].SOLUTION != ""}
       <tr>       
        <td colspan=2>{$config[$key].SOLUTION}</td>
       </tr>
       <tr>       
        <td colspan=2>
         {if $config[$key].MUST}
          <b>{t}GOsa will NOT run without fixing this.{/t}</b>
         {else}
          <b>{t}GOsa will run without fixing this.{/t}</b>
         {/if}
        </td>
       </tr>
      {/if}
     {/if}
    {/foreach}
   </table>
  </td>
 </tr>
</table>

