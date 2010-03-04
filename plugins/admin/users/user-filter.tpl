<div class="filter">

   <h3>Filter


  {$TEMPLATES}&nbsp;<LABEL for='TEMPLATES'>{t}Show templates{/t}</LABEL><br>
  {$POSIX}&nbsp;<LABEL for='POSIX'>{t}Show POSIX users{/t}</LABEL><br>
  {$MAIL}&nbsp;<LABEL for='MAIL'>{t}Show Mail users{/t}</LABEL><br>
  {$SAMBA}&nbsp;<LABEL for='SAMBA'>{t}Show Samba users{/t}</LABEL><br>

  <hr>

  {$SCOPE}

  <table>
   <tr>
    <td>
     <label for="NAME">
      <img src="images/lists/search.png" align=middle>&nbsp;Name
     </label>
    </td>
    <td>
     {$NAME}
    </td>
   </tr>
  </table>

  <table>
   <tr>
    <td width="100%" align="right">
     {$APPLY}
    </td>
   </tr>
  </table>

</div>
