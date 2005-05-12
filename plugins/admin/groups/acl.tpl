<table width=100%>
 <tr>
  <td>
   <b>{t}Object{/t}</b>
  </td>
  <td>
   <select size="1" name="object" onChange="mainform.submit()">
    {html_options options=$objects selected=$object}
   </select>
   &nbsp;
   {if $javascript ne "true"}
   <input type=submit name="goButton" value="{t}Choose{/t}">
   {/if}
  </td>
  <td>
   <input type=checkbox name="selfflag" value="1" {$selfflag}>
   {t}Apply this acl only for users own entries{/t}
  </td>
 </tr>
</table>

<p class="seperator">
 &nbsp;
</p>
