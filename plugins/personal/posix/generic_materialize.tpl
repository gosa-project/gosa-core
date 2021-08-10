<div class="row setting">
  <div class="col s6">
    <h3>{t}Generic{/t}</h3>

    <div class="input-field">
      {render acl=$homeDirectoryACL checkbox=$multiple_support checked=$use_homeDirectory}
      <input type='text' id="homeDirectory" name="homeDirectory" size=40 maxlength=120 value="{$homeDirectory}">
      {/render}
      <label for="homeDirectory">{t}Home directory{/t}{$must}</label>
    </div>

    <div class="input-field">
      {render acl=$loginShellACL checkbox=$multiple_support checked=$use_loginShell}
      <select id="loginShell" size="1" name="loginShell">
        {html_options values=$shells output=$shells selected=$loginShell}
      </select>
      {/render}
      <label for="loginShell">{t}Shell{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$gidNumberACL checkbox=$multiple_support checked=$use_primaryGroup}
      <select id="primaryGroup" size="1" name="primaryGroup">
        {html_options options=$secondaryGroups selected=$primaryGroup}
      </select>
      {/render}
      <label for="primaryGroup">{t}Primary group{/t}</label>
    </div>

    {if !$multiple_support}

      <div class="row">
        <div class="col s6">
          <span>{t}Status{/t}</span>
        </div>
        <div class="col s6">
          <span>{$status}</span>
        </div>
      </div>

      {if $gotoLastSystemLogin}

      <div class="row">
        <div class="col s6">
          <span>{t}Last log-on{/t}</span>
        </div>
        <div class="col s6">
          <span>{$gotoLastSystemLogin}</span>
        </div>
      </div>

      {/if}
    {/if}

    {if !$multiple_support}
      <div class="row">

        <div class="col s6 valign-wrapper">
          <label>
            {render acl=$force_idsACL}
            <input id="force_ids" type=checkbox name="force_ids" value="1" {$force_ids} onclick="{$onClickIDS}">
            {/render}
            <span>{t}Force UID/GID{/t}</span>
          </label>
        </div>

        <div class="col s6">
          <div class="input-field">
            {render acl=$uidNumberACL}
            <input type='text' id="uidNumber" name="uidNumber" size=9 maxlength=9 {$forceMode} value="{$uidNumber}">
            {/render}
            <label for="uidNumber">{t}UID{/t}</label>
          </div>

          <div class="input-field">
            {render acl=$gidNumberACL}
            <input type='text' id="gidNumber" name="gidNumber" size=9 maxlength=9 {$forceMode} value="{$gidNumber}">
            {/render}
            <label for="gidNumber">{t}GID{/t}</label>
          </div>
        </div>

      </div>
    {/if}
  </div>

  <div class="col s6">
    <h3>{t}Group membership{/t}</h3>

    {render acl=$groupMembershipACL}
    {$groupList}
    {/render}
    {render acl=$groupMembershipACL}
    <button class="btn-small gonicus-color" type='submit' name='edit_groupmembership'>{msgPool
      type=addButton}</button>&nbsp;
    {/render}
  </div>
</div>

<hr>

<div class="row setting">

  <div class="col s6">
    {if $sshPublicKey == 1}
      {render acl=$sshPublicKeyACL}
      <h3>{t}SSH keys{/t}</h3>
      <button type='submit' name='edit_sshpublickey'>{t}Edit public ssh keys...{/t}</button>
      {/render}
    {/if}

    {include file="$pwmode.tpl"}
  </div>

  <div class="col s6">
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
