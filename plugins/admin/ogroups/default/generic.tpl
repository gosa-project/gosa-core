<div class="ogroup-wrapper">
  <h2>{t}Generic settings{/t}</h2>

  <div class="row">
    <input type="hidden" name="ogroupedit" value="1">
    <div class="col s12 xl6">

      {render acl=$cnACL}
      <div class="input-field">
        <input type='text' name="cn" id="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of the group{/t}">
        <label for="cn">{t}Group name{/t}{$must}</label>
      </div>
      {/render}

      {render acl=$descriptionACL}
      <div class="input-field">
        <input type='text' id="description" name="description" size=40 maxlength=80 value="{$description}"
          title="{t}Descriptive text for this group{/t}">
        <label for="description">{t}Description{/t}</label>
      </div>
      {/render}

      {render acl=$baseACL}
        <div class="input-field ldap-tree">
          {$base}
        </div>
      {/render}

      <hr class="divider">

      {$trustModeDialog}
    </div>

    <div class="col s12 xl6">
      {if $isRestrictedByDynGroup}
        <h3>{t}The group members are part of a dyn-group and cannot be managed!{/t}</h3>
      {/if}

      <label for="members">{t}Member objects{/t} ({$combinedObjects})</label>
      {render acl=$memberACL}
        {$memberList}
      {/render}

      {if !$isRestrictedByDynGroup}
        {render acl=$memberACL}
          <button class="btn-small" type='submit' name='edit_membership'>{msgPool
            type=addButton}</button>&nbsp;
        {/render}
      {/if}
    </div>
  </div>

  <!-- Place cursor -->
  <script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('cn');
  </script>
</div>
