<div class="role-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>

      {render acl=$cnACL}
      <div class="input-field">
        <input type='text' value='{$cn}' name='cn' id="cn">
        <label for="cn">{t}Name{/t}{$must}</label>
      </div>
      {/render}

      {render acl=$descriptionACL}
      <div class="input-field">
        <input type='text' value='{$description}' name='description' id="description">
        <label for="description">{t}Description{/t}</label>
      </div>
      {/render}

      {render acl=$baseACL}
      <div class="input-field ldap-tree">
        {$base}
      </div>
      {/render}

      {render acl=$telephoneNumberACL}
      <div class="input-field">
        <input type='text' value='{$telephoneNumber}' name='telephoneNumber' id="telephoneNumber">
        <label for="telephoneNumber">{t}Phone number{/t}</label>
      </div>
      {/render}

      {render acl=$facsimileTelephoneNumberACL}
      <div class="input-field">
        <input type='text' value='{$facsimileTelephoneNumber}' name='facsimileTelephoneNumber' id="facsimileTelephoneNumber">
        <label for="facsimileTelephoneNumber">{t}Fax number{/t}</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <h3>{t}Occupants{/t}</h3>

      {render acl=$roleOccupantACL}
        {$memberList}
      {/render}
      {render acl=$roleOccupantACL}
        <button class="btn-small" type='submit' name='edit_membership'>{msgPool type=addButton}</button>
      {/render}
    </div>
  </div>
</div>
