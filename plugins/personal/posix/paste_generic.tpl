<table width='100%' summary="{t}Posix settings{/t}">
 <tr>
  <td style='vertical-align:top;'>
   <h3>{t}Posix settings{/t}</h3>
   <table summary="{t}Home directory{/t}">
    <tr>
     <td>
      <label for="homeDirectory">{t}Home directory{/t}</label>
     </td>
     <td>
      <input type='text' id="homeDirectory" name="homeDirectory" size=40 maxlength=120 value="{$homeDirectory}">
     </td>
    </tr>
   </table>
   <table summary="{t}Account settings{/t}">
    <tr>
     <td><label for="primaryGroup">{t}Primary group{/t}</label></td>
     <td>
      <select id="primaryGroup" size="1" name="primaryGroup">
       {html_options options=$secondaryGroups selected=$primaryGroup}
      </select> 
     </td>
    </tr>
    <tr>
     <td>
      <input id="force_ids" type=checkbox name="force_ids" value="1"  {$force_ids}
      onclick="changeState('uidNumber');changeState('gidNumber');">
     </td>
     <td>
      <label for="force_ids">
      {t}Force UID/GID{/t}
      </label>
     </td>
     <td>&nbsp;</td>
     <td>
      <label for="uidNumber">{t}UID{/t}</label>
     </td>
     <td>
      <input type='text' id="uidNumber" name="uidNumber" size=9 maxlength=9 {$forceMode} value="{$uidNumber}">
     </td>
    </tr>
    <tr>
     <td colspan="3">&nbsp;</td>
     <td>
      <label for="gidNumber">{t}GID{/t}</label>
     </td>
     <td>
      <input type='text' id="gidNumber" name="gidNumber" size=9 maxlength=9 {$forceMode} value="{$gidNumber}">
     </td>
    </tr>
   </table>
  </td>
  <td>
   <table summary="{t}Group membership{/t}">
    <tr>
     <td style="vertical-align:top;" colspan="4">
      <h3>{t}Group membership{/t}
      </h3>
     </td>
    </tr>
    <tr>
     <td style="vertical-align:top;" colspan="4">
      
      {if $groups eq "too_many_for_nfs"}
      <b style="color:red">{t}(Warning: more than 16 groups are not supported by NFS!){/t}</b>
      <br>
      {/if}
      <select style="width:100%; height:130px;" name="group_list[]" size=16 multiple>
       {html_options options=$groupMembership}
      </select>
      <br>
      <button type='submit' name='edit_groupmembership'>{msgPool type=addButton}</button>&nbsp;
      
      <button type='submit' name='delete_groupmembership'>{msgPool type=delButton}</button>
      
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<br>
<br>
