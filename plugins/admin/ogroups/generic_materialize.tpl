<div class="row">

  <input type="hidden" name="ogroupedit" value="1">

  <div class="col s6">
    <div class="card">
      <div class="card-content">
        <span class="card-title">{t}Generic settings{/t}</span>
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

        <div class="row">
          {render acl=$baseACL}
          {$base}
          {/render}
        </div>
      </div>
    </div>

    {$trustModeDialog}
  </div>

  <div class="col s6">
    {if $isRestrictedByDynGroup}
    <div class="row">
      <h5>{t}The group members are part of a dyn-group and cannot be managed!{/t}</h5>
    </div>
    {/if}

    <div class="card">
      <div class="card-content">
        <label for="members">{t}Member objects{/t} ({$combinedObjects})</label>
        {render acl=$memberACL}
        {$memberList}
        {/render}
    
        {if !$isRestrictedByDynGroup}
        <div class="card-action">
          {render acl=$memberACL}
          <button class="btn-small gonicus-color" type='submit' name='edit_membership'>{msgPool type=addButton}</button>&nbsp;
          {/render}
        </div>
        {/if}
      </div>
    </div>
  </div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
  focus_field('cn');
</script>