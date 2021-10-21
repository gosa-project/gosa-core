<div class="posix-settings">
  <div class="row setting">
    <div class="col s12 xl6">
      <h3>{t}Generic{/t}</h3>

      {render acl=$homeDirectoryACL checkbox=$multiple_support checked=$use_homeDirectory}
        <div class="input-field">
          <input type='text' id="homeDirectory" name="homeDirectory" size=40 maxlength=120 value="{$homeDirectory}">
          <label for="homeDirectory">{t}Home directory{/t}{$must}</label>
        </div>
      {/render}

      {render acl=$loginShellACL checkbox=$multiple_support checked=$use_loginShell}
        <div class="input-field">
          <select id="loginShell" size="1" name="loginShell">
            {html_options values=$shells output=$shells selected=$loginShell}
          </select>
          <label for="loginShell">{t}Shell{/t}</label>
        </div>
      {/render}

      {render acl=$gidNumberACL checkbox=$multiple_support checked=$use_primaryGroup}
        <div class="input-field">
          <select id="primaryGroup" size="1" name="primaryGroup">
            {html_options options=$secondaryGroups selected=$primaryGroup}
          </select>
          <label for="primaryGroup">{t}Primary group{/t}</label>
        </div>
      {/render}

      {if !$multiple_support}
        <div class="row">
          <div class="col s6">
            <div class="status-info">
              <p>{t}Status{/t}:</p>
              <p><i>{$status}</i></p>
            </div>
          </div>

        {if $gotoLastSystemLogin}
          <div class="col s6">
            <div class="status-info">
              <p>{t}Last log-on{/t}:</p>
              <p><i>{$gotoLastSystemLogin}</i></p>
            </div>
          </div>
        {/if}
        </div>
      {/if}

      {if !$multiple_support}
        <div class="row">
          <div class="col s6">
            {render acl=$force_idsACL}
              <label>
                <p>
                  <input id="force_ids" type=checkbox name="force_ids" value="1" {$force_ids} onclick="{$onClickIDS}">
                  <span>{t}Force UID/GID{/t}</span>
                </p>
              </label>
            {/render}

            <div class="acl-numbers">
              {render acl=$uidNumberACL}
                <div class="input-field">
                  <input type='text' id="uidNumber" name="uidNumber" size=9 maxlength=9 {$forceMode} value="{$uidNumber}">
                  <label for="uidNumber">{t}UID{/t}</label>
                </div>
              {/render}

              {render acl=$gidNumberACL}
                <div class="input-field">
                  <input type='text' id="gidNumber" name="gidNumber" size=9 maxlength=9 {$forceMode} value="{$gidNumber}">
                  <label for="gidNumber">{t}GID{/t}</label>
                </div>
              {/render}
            </div>
          </div>

        </div>
      {/if}
    </div>

    <div class="col s12 xl6">
      <h3>{t}Group membership{/t}</h3>

      {render acl=$groupMembershipACL}
        {$groupList}
      {/render}
      {render acl=$groupMembershipACL}
        <button class="btn-small" type='submit' name='edit_groupmembership'>{msgPool
        type=addButton}</button>&nbsp;
      {/render}
    </div>
  </div>

  <hr class="divider">

  <div class="row setting">
    <div class="col s12 xl6">
      {if $sshPublicKey == 1}
        {render acl=$sshPublicKeyACL}
          <div class="ssh-plug-wrapper">
              <h3>{t}SSH keys{/t}</h3>
              <button class="btn-small" type='submit' name='edit_sshpublickey'>{t}Edit public ssh keys{/t}</button>
          </div>
          <hr class="divider">
        {/render}
      {/if}

      {include file="$pwmode.tpl"}
    </div>

    <div class="col s12 xl6">
      {$trustModeDialog}
    </div>

  </div>

  {if $multiple_support}
    <input type="hidden" name="posix_mulitple_edit" value="1">
  {/if}

  <input type="hidden" name="posixTab" value="posixTab">

  <!-- Place cursor -->
  <script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('homeDirectory');
  </script>
</div>
