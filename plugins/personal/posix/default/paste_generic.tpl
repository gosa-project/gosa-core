<table width='100%' summary="{t}POSIX settings{/t}">
 <tr>
  <td style='width:50%;' class='right-border'>

   <h3>{t}POSIX settings{/t}</h3>
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

   <table summary="{t}Group membership{/t}" width='100%'>
 
    <tr>
     <td><h3>{t}Group membership{/t}</h3>
     </td>
    </tr>
    <tr>
     <td>
      
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
