{if $is_default}

<h3>{t}Generic{/t} - {t}global defaults{/t}</h3>
<table style="width:100%;" summary="{t}Generic{/t} - {t}global defaults{/t}">
 <tr>
  <td>
   {t}Name{/t}{$must}
  </td>
  <td>
   <input type="text" name="dummy" value="{$cn}" disabled>
  </td>
 </tr>
 <tr>
  <td>
   {t}Description{/t}
  </td>
  <td>
   {render acl=$descriptionACL}
   <input type="text" name="description" value="{$description}">
   {/render}
  </td>
 </tr>
</table>

{else}

<table style="width:100%;" summary="{t}Generic{/t}">
 <tr>
  <td style='width:50%'>   <h3>{t}Generic{/t}</h3>

   <table  summary="{t}Generic{/t}"> 
    <tr>
     <td>
      {t}Name{/t}{$must}
     </td>
     <td>
      {render acl=$cnACL}
      <input type="text" name="cn" value="{$cn}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>
      {t}Description{/t}
     </td>
     <td>
      {render acl=$descriptionACL}
      <input type="text" name="description" value="{$description}">
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td class='left-border'>
    {$trustModeDialog}
  </td>
 </tr> 
 <tr><td style="width:100%;"colspan="2"><hr></td></tr>
 <tr>
  <td style="width:50%;padding-right:5px;">
   <h3>{t}Users and groups{/t}</h3>
   {render acl=$sudoUserACL}
   {$listing_sudoUser}
   {/render}
   {render acl=$sudoUserACL}
   <input type='text' value='' name='new_sudoUser'>
   {/render}
   {render acl=$sudoUserACL}
   <button type='submit' name='add_sudoUser'>{msgPool type=addButton}</button>

   {/render}
   {render acl=$sudoUserACL}
   <button type='submit' name='list_sudoUser'>{t}Add from list{/t}</button>

   {/render}
  </td>
  <td class='left-border'>
   <h3>{t}Systems{/t}</h3>
   {render acl=$sudoHostACL}
   {$listing_sudoHost}
   {/render}
   {render acl=$sudoHostACL}
   <input type='text' value='' name='new_sudoHost'>
   {/render}
   {render acl=$sudoHostACL}
   <button type='submit' name='add_sudoHost'>{msgPool type=addButton}</button>

   {/render}
   {if $systemEnabled}
   {render acl=$sudoHostACL}
   <button type='submit' name='list_sudoHost'>{t}Add from list{/t}</button>
   {/render}
   {/if}
  </td>
 </tr> 
 <tr><td style="width:100%;"colspan="2"><hr></td></tr>
 <tr>
  <td style="padding-right:5px;">
   <h3>{t}Commands{/t}</h3>
   {render acl=$sudoCommandACL}
   {$listing_sudoCommand}
   {/render}
   {render acl=$sudoCommandACL}
   <input type='text' value='' name='new_sudoCommand'>
   {/render}
   {render acl=$sudoCommandACL}
   <button type='submit' name='add_sudoCommand'>{msgPool type=addButton}</button>

   {/render}
  </td>
  <td class='left-border'>
   <h3>{t}Run as{/t}</h3>
   {render acl=$sudoRunAsACL}
   {$listing_sudoRunAs}
   {/render}
   {render acl=$sudoRunAsACL}
   <input type='text' value='' name='new_sudoRunAs'>
   {/render}
   {render acl=$sudoRunAsACL}
   <button type='submit' name='add_sudoRunAs'>{msgPool type=addButton}</button>

   {/render}
  </td>
 </tr>
</table>
{/if}
