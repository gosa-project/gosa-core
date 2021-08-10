<div class="row">

  <input type="hidden" name="ogroupedit" value="1">

  <div class="col s6">
    <h3>{t}Generic settings{/t}</h3>
    <div class="input-field">
      {render acl=$cnACL}
      <input type='text' name="cn" id="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of the group{/t}">
      {/render}
      <label for="cn">{t}Group name{/t}{$must}</label>
    </div>

    <div class="input-field">
      {render acl=$descriptionACL}
      <input type='text' id="description" name="description" size=40 maxlength=80 value="{$description}"
        title="{t}Descriptive text for this group{/t}">
      {/render}
      <label for="description">{t}Description{/t}</label>
    </div>

    {render acl=$baseACL}
    <div class="input-field ldap-tree">
      {$base}
    </div>
    {/render}

    {$trustModeDialog}
  </div>

  <div class="col s6">
    {if $isRestrictedByDynGroup}
    <div class="row">
      <h3>{t}The group members are part of a dyn-group and cannot be managed!{/t}</h3>
    </div>
    {/if}

    <label for="members">{t}Member objects{/t} ({$combinedObjects})</label>
    {render acl=$memberACL}
    {$memberList}
    {/render}

    {if !$isRestrictedByDynGroup}
    <div class="card-action">
      {render acl=$memberACL}
      <button class="btn-small gonicus-color" type='submit' name='edit_membership'>{msgPool
        type=addButton}</button>&nbsp;
      {/render}
    </div>
    {/if}
  </div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
  focus_field('cn');
</script>