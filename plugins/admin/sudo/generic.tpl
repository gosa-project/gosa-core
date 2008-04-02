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
  </td>
  <td style="border-left: solid 1px #AAAAAA;"><b>Commands</b>
   {$divlist_sudoCommand}
  </td>
 </tr> 
 <tr><td style="width:100%;"colspan="2"><p class="seperator">&nbsp;</p></td></tr>
 <tr>
  <td colspan="2"><b>Flags</b>
   {$divlist_sudoOption}
  </td>
 </tr>
 <tr><td style="width:100%;"colspan="2"><p class="seperator">&nbsp;</p></td></tr>
 <tr>
  <td><b>Hosts</b>
   {$divlist_sudoHost}
  </td>
  <td style="border-left: solid 1px #AAAAAA;"><b>Run as</b>
   {$divlist_sudoRunas}
  </td>
 </tr>
</table>
