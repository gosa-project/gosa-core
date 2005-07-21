<table summary="" width="100%">
 <tr>
  <td>
   <b><LABEL for="object">{t}Object{/t}</LABEL></b>
  </td>
  <td>
   <select size="1" id="object" name="object" onChange="mainform.submit()">
    {html_options options=$objects selected=$object}
   </select>
   &nbsp;
   {if $javascript ne "true"}
   <input type=submit name="goButton" value="{t}Choose{/t}">
   {/if}
  </td>
  <td>
   <input type=checkbox name="selfflag" id="selfflag" value="1" {$selfflag}>
   <LABEL for="selfflag">{t}Apply this acl only for users own entries{/t}</LABEL>
  </td>
 </tr>
</table>

<p class="seperator">
 &nbsp;
</p>
