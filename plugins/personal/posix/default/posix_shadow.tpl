<h3>{t}Account settings{/t}</h3>

<div class="account-settings">
  <!-- must_change_password  -->
  <label>
    <p>
      {render acl=$mustchangepasswordACL checkbox=$multiple_support checked=$use_mustchangepassword}
      <input class="center" type="checkbox" name="mustchangepassword" value="1" {$mustchangepassword}>
      {/render}
      <span>{t}User must change password on first login{/t}</span>
    </p>
  </label>

  <!-- shadowMin -->
  <label>
    <p>
      {render acl=$shadowMinACL checkbox=$multiple_support checked=$use_activate_shadowMin}
      <input type="checkbox" class="center" name="activate_shadowMin" value="1" {$activate_shadowMin}>
      {/render}
      <span>
        {render acl=$shadowMinACL}
        {$shadowmins}
        {/render}
      </span>
    </p>
  </label>

  <!-- activate_shadowMax -->
  <label>
    <p>
      {render acl=$shadowMaxACL checkbox=$multiple_support checked=$use_activate_shadowMax}
      <input type="checkbox" class="center" name="activate_shadowMax" value="1" {$activate_shadowMax}>
      {/render}
      <span>
        {render acl=$shadowMaxACL}
        {$shadowmaxs}
        {/render}
      </span>
    </p>
  </label>

  <!-- activate_shadowExpire -->
  <div class="inline-date-picker">
    <label>
      <p>
        {render acl=$shadowExpireACL checkbox=$multiple_support checked=$use_activate_shadowExpire}
        <input type="checkbox" class="center" name="activate_shadowExpire" value="1" {$activate_shadowExpire}>
        {/render}
        <span>{t}Password expires on{/t}
          {render acl=$shadowExpireACL}
          <input type="text" id="lang" value="{$lang}" hidden="true">
          <input type="text" id="shadowExpire" name="shadowExpire" class="datepicker" style='width:100px'
            value="{$shadowExpire}">
          <i class="material-icons datepicker">edit_calendar</i>
          {/render}
        </span>
      </p>
    </label>
  </div>

  <!-- shadowInactive -->
  <label>
    <p>
      {render acl=$shadowInactiveACL checkbox=$multiple_support checked=$use_activate_shadowInactive}
      <input type="checkbox" class="center" name="activate_shadowInactive" value="1" {$activate_shadowInactive}>
      {/render}
      <span>
        {render acl=$shadowInactiveACL}
        {$shadowinactives}
        {/render}
      </span>
    </p>
  </label>

  <!-- activate_shadowWarning -->
  <label>
    <p>
      {render acl=$shadowWarningACL checkbox=$multiple_support checked=$use_activate_shadowWarning}
      <input type="checkbox" class="center" name="activate_shadowWarning" value="1" {$activate_shadowWarning}>
      {/render}
      <span>
        {render acl=$shadowWarningACL}
        {$shadowwarnings}
        {/render}
      </span>
    </p>
  </label>
</div>
