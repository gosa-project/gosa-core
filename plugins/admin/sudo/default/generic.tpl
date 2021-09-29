

<div class="sudo-wrapper">
  {if $is_default}
    <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Generic{/t} - {t}global defaults{/t}</h3>

        <div class="input-field">
          <input type="text" name="dummy" id="dummy" value="{$cn}" disabled>
          <label for="dummy">{t}Name{/t}{$must}</label>
        </div>

        {render acl=$descriptionACL}
        <div class="input-field">
          <input type="text" name="description" id="description" value="{$description}">
          <label for="description">{t}Description{/t}</label>
        </div>
        {/render}
      </div>
    </div>
  {else}
    <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Generic{/t}</h3>

        {render acl=$cnACL}
        <div class="input-field">
          <input type="text" name="cn" id="cn" value="{$cn}">
          <label for="cn">{t}Name{/t}{$must}</label>
        </div>
        {/render}

        {render acl=$descriptionACL}
        <div class="input-field">
          <input type="text" name="description" id="description" value="{$description}">
          <label for="description">{t}Description{/t}</label>
        </div>
        {/render}
      </div>

      <div class="col s12 xl6">
        {$trustModeDialog}
      </div>
    </div>

    <hr class="divider">

    <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Users and groups{/t}</h3>

        {render acl=$sudoUserACL}
        {$listing_sudoUser}
        {/render}

        {render acl=$sudoUserACL}
        <div class="input-field add">
          <input type='text' value='' name='new_sudoUser'>
          <button class="btn-small" type='submit' name='add_sudoUser'>{msgPool type=addButton}</button>
          <button class="btn-small" type='submit' name='list_sudoUser'>{t}Add from list{/t}</button>
        </div>
        {/render}

      </div>

      <div class="col s12 xl6">
        <h3>{t}Systems{/t}</h3>

        {render acl=$sudoHostACL}
        {$listing_sudoHost}
        {/render}

        {render acl=$sudoHostACL}
        <div class="input-field add">
          <input type='text' value='' name='new_sudoHost'>
          <button class="btn-small" type='submit' name='add_sudoHost'>{msgPool type=addButton}</button>
          {if $systemEnabled}
          <button class="btn-small" type='submit' name='list_sudoHost'>{t}Add from list{/t}</button>
          {/if}
        </div>
        {/render}
      </div>
    </div>

    <hr class="divider">

    <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Commands{/t}</h3>

        {render acl=$sudoCommandACL}
        {$listing_sudoCommand}
        {/render}

        {render acl=$sudoCommandACL}
        <div class="input-field add">
          <input type='text' value='' name='new_sudoCommand'>
          <button class="btn-small" type='submit' name='add_sudoCommand'>{msgPool type=addButton}</button>
        </div>
        {/render}
      </div>

      <div class="col s12 xl6">
        <h3>{t}Run as{/t}</h3>

        {render acl=$sudoRunAsACL}
        {$listing_sudoRunAs}
        {/render}

        {render acl=$sudoRunAsACL}
        <div class="input-field add">
          <input type='text' value='' name='new_sudoRunAs'>
          <button class="btn-small" type='submit' name='add_sudoRunAs'>{msgPool type=addButton}</button>
        </div>
        {/render}
      </div>
    </div>
  {/if}
</div>
