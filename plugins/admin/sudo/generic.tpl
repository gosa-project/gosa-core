<h2>Sudo generic</h2>
<table style="width: 90%;">
 <tr>
  <td style="width:50%"><b>Generic</b>
   <table> 
    <tr>
     <td>
      {t}Name{/t}
     </td>
     <td>
      <input type="text" name="cn" value="{$cn}">
     </td>
    </tr>
    <tr>
     <td>
      {t}Description{/t}
     </td>
     <td>
      <input type="text" name="description" value="{$description}">
     </td>
    </tr>
   </table>
  </td>
 </tr> 
 <tr>
  <td style="width:50%"><b>User / Groups</b>
   {$divlist_sudoUser}
   <input type='text' value='' name='new_sudoUser'><input type='submit' name='add_sudoUser' value='{msgPool type=addButton}'>
   <input type='submit' name='list_sudoUser' value='{t}Add from list{/t}'>
  </td>
  <td style="border-left: solid 1px #AAAAAA;"><b>Commands</b>
   {$divlist_sudoCommand}
   <input type='text' value='' name='new_sudoCommand'><input type='submit' name='add_sudoCommand' value='{msgPool type=addButton}'>
  </td>
 </tr> 
 <tr><td style="width:100%;"colspan="2"><p class="seperator">&nbsp;</p></td></tr>
 <tr>
  <td colspan="2"><b>Flags</b>
   {$divlist_sudoOption}
   <input type='submit' name='list_sudoOption' value='{t}Add from list{/t}'>
  </td>
 </tr>
 <tr><td style="width:100%;"colspan="2"><p class="seperator">&nbsp;</p></td></tr>
 <tr>
  <td><b>Hosts</b>
   {$divlist_sudoHost}
   <input type='text' value='' name='new_sudoHost'><input type='submit' name='add_sudoHost' value='{msgPool type=addButton}'>
   <input type='submit' name='list_sudoHost' value='{t}Add from list{/t}'>
  </td>
  <td style="border-left: solid 1px #AAAAAA;"><b>Run as</b>
   {$divlist_sudoRunas}
   <input type='text' value='' name='new_sudoRunas'><input type='submit' name='add_sudoRunas' value='{msgPool type=addButton}'>
  </td>
 </tr>
</table>
