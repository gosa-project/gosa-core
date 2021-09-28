<h3>{t}Account settings{/t}</h3>

<div class="account-settings">
  <!-- must_change_password  -->
  <label>
    {render acl=$mustchangepasswordACL checkbox=$multiple_support checked=$use_mustchangepassword}
    <input class="center" type="checkbox" name="mustchangepassword" value="1" {$mustchangepassword}>
    {/render}
    <span>{t}User must change password on first login{/t}</span>
  </label>

  <!-- shadowMin -->
  <label>
    {render acl=$shadowMinACL checkbox=$multiple_support checked=$use_activate_shadowMin}
    <input type="checkbox" class="center" name="activate_shadowMin" value="1" {$activate_shadowMin}>
    {/render}
    <span>
      {render acl=$shadowMinACL}
      {$shadowmins}
      {/render}
    </span>
  </label>

  <!-- activate_shadowMax -->
  <label>
    {render acl=$shadowMaxACL checkbox=$multiple_support checked=$use_activate_shadowMax}
    <input type="checkbox" class="center" name="activate_shadowMax" value="1" {$activate_shadowMax}>
    {/render}
    <span>
      {render acl=$shadowMaxACL}
      {$shadowmaxs}
      {/render}
    </span>
  </label>

  <!-- activate_shadowExpire -->
  <div class="inline-date-picker">
    <label>
      {render acl=$shadowExpireACL checkbox=$multiple_support checked=$use_activate_shadowExpire}
      <input type="checkbox" class="center" name="activate_shadowExpire" value="1" {$activate_shadowExpire}>
      {/render}
      <span>{t}Password expires on{/t}
        <div class="input-field inline">
          <i class="material-icons datepicker right">edit_calendar</i>
          {render acl=$shadowExpireACL}
          <input type="text" id="lang" value="{$lang}" hidden="true">
          <input type="text" id="shadowExpire" name="shadowExpire" class="datepicker" style='width:100px'
            value="{$shadowExpire}">
          {/render}
        </div>
      </span>
    </label>
  </div>

  <!-- shadowInactive -->
  <label>
    {render acl=$shadowInactiveACL checkbox=$multiple_support checked=$use_activate_shadowInactive}
    <input type="checkbox" class="center" name="activate_shadowInactive" value="1" {$activate_shadowInactive}>
    {/render}
    <span>
      {render acl=$shadowInactiveACL}
      {$shadowinactives}
      {/render}
    </span>
  </label>

  <!-- activate_shadowWarning -->
  <label>
    {render acl=$shadowWarningACL checkbox=$multiple_support checked=$use_activate_shadowWarning}
    <input type="checkbox" class="center" name="activate_shadowWarning" value="1" {$activate_shadowWarning}>
    {/render}
    <span>
      {render acl=$shadowWarningACL}
      {$shadowwarnings}
      {/render}
    </span>
  </label>
</div>
