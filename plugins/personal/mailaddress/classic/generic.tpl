<table summary="{t}Mail Address Settings{/t}" style='width:100%;'>
 <tr>
  <td style='width:50%; '>

   {if !$multiple_support}
   <h3>{t}User Mail Address{/t}</h3>
   <table summary="{t}Mail address{/t}">
    <tr>
     <td><label for="mail">{t}Mail address{/t}</label>{$must}</td>
     <td>
      {render acl=$mailACL}
      <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
      {/render}
     </td>
    </tr>
   </table>
   {else}
   <h3>{t}Users' Mail Address Scheme{/t}</h3>
   <table summary="{t}Mail addresses{/t}">
    <tr>
    <td colspan="2"><i>{t}Please specify a mail address pattern (e.g., &#123;%uid&#124;@your-domain.com). Valid placeholders are: &#123;%uid&#124;, &#123;%givenName&#124; and &#123;%sn&#124;. For no action on the users' mail addresses, please leave empty.{/t}</i>
    </td>
    </tr>
    <tr><td>&nbsp;</td></tr>
    <tr>
     <td><label for="mail">{t}Mail address pattern{/t}</label></td>
     <td>
      {render acl=$mailACL}
      <input type='text' id="mail" name="mail" size=35 maxlength=65 value="">
      {/render}
     </td>
    </tr>
   </table>
   {/if}

   <!-- further mail aliases -->
   <br>

   {if !$multiple_support}
   <h3>{t}User's Mail Aliases{/t}</h3>
   <table summary="{t}Alias addresses{/t}">
    <tr>
     <td><label for="single_alias">{t}Alias addresses{/t}</label></td>
     <td>
      {render acl=$aliasACL}
      {$aliasWidget}
      {/render}
      {render acl=$aliasACL}
      <input type="text" id="single_alias" name="single_alias" maxlength=65
      value="{t}Email alias address{/t}" onFocus='document.getElementById("single_alias").value=""'>
      {/render}
      {render acl=$aliasACL}
      <button id="add_alias" type="submit" name="add_alias">{t}Add{/t}</button>
      {/render}
     </td>
    </tr>
   </table>
   {else}
   <h3>{t}Users' Mail Aliases{/t}</h3>
   <table summary="{t}Alias addresses{/t}">
    <tr>
    <td><i>{t}Mass editing of email aliases is not supported.{/t}</i>
    </td>
    </tr>
   </table>
   {/if}

  </td>
</tr>
</table>

<input type="hidden" name="mailAccountTab" value="mailAccountTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">

{literal}
focus_field('mail');
{/literal}
</script>
