<h3>{t}Mail Address{/t}</h3>

<table style='width:100%;' cellspacing=0  summary="{t}Mail Address{/t}">
  <tr>
     <td><label for="mail">{t}Mail Address{/t}</label>
     <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
     </td>
  </tr>
</table>

<br>

<h3>{t}User Mail Aliases{/t}</h3>

<table style='width:100%;' cellspacing=0  summary="{t}Mail Aliases{/t}">
  <tr>
  <td><label for="single_alias">{t}Alias addresses{/t}</label>
      <div style="width: 65em">
      {render acl=$aliasACL}
      {$aliasWidget}
      {/render}
      </div>
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
<br>
