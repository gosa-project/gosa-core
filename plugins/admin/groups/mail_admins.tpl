<table summary="" style="width:100%;">
 <tr>
  <td style="width:45%; vertical-align:top;">
  <LABEL for="admins"> {t}Folder administrators{/t}</LABEL>
   <br>
   <select id="admins" style="width:380px; height:300px;" name="admins[]" size=15 multiple>
    {html_options options=$admins}
   </select>
  </td>

  <td style="width:10%; vertical-align:middle; text-align:center;">
   <input type=submit name="add_users" value="&larr;" style="width:50px;">
   <br>
   <br>
   <input type=submit name="del_users" value="&rarr;" style="width:50px;">
  </td>

  <td style="width:45%; vertical-align:top;">
   <br>
   <select style="width:380px; height:275px;" name="users[]" size=15 multiple>
    {html_options options=$mailusers}
   </select>
   <br>
	<LABEL for="department">{t}Select a specific department{/t}</LABEL>:
   <select id="department" name="department" size=1 onChange="mainform.submit()">
    {html_options options=$departments selected=$department}
   </select>

   {if $javascript ne "true"}
   <input type=submit name="goButton" value="{t}Choose{/t}">
   {/if}
  </td>
 </tr>
</table>

<p class="plugbottom">
 <input type=submit name="edit_admins_finish" value="{t}Apply{/t}">
 &nbsp;
 <input type=submit name="edit_admins_cancel" value="{t}Cancel{/t}">
</p>
