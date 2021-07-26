<div class="card">
  <div class="card-content">
    <span class="card-title">{t}Account settings{/t}</span>

    <!-- must_change_password  -->
    <label>
      {render acl=$mustchangepasswordACL checkbox=$multiple_support checked=$use_mustchangepassword}
      <input class="center" type="checkbox" name="mustchangepassword" value="1" {$mustchangepassword}>
      {/render}
      <span class="black-text">{t}User must change password on first login{/t}</span>
    </label>

    <!-- shadowMin -->
    <label>
      {render acl=$shadowMinACL checkbox=$multiple_support checked=$use_activate_shadowMin}
      <input type="checkbox" class="center" name="activate_shadowMin" value="1" {$activate_shadowMin}>
      {/render}
      <span class="black-text">
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
      <span class="black-text">
        {render acl=$shadowMaxACL}
        {$shadowmaxs}
        {/render}
      </span>
    </label>

    <!-- activate_shadowExpire -->
    <div>
      <label>
        {render acl=$shadowExpireACL checkbox=$multiple_support checked=$use_activate_shadowExpire}
        <input type="checkbox" class="center" name="activate_shadowExpire" value="1" {$activate_shadowExpire}>
        {/render}
        <span class="black-text">{t}Password expires on{/t}</span>
      </label>
      <div class="input-field inline">
        <i class="material-icons right">edit_calendar</i>
        {render acl=$shadowExpireACL}
        <input type="text" id="shadowExpire" name="shadowExpire" class="datepicker" style='width:100px'
          value="{$shadowExpire}">
        {/render}
      </div>
      {if $shadowExpireACL|regex_replace:"/[cdmr]/":"" == "w"}
      <script type="text/javascript">
        {literal}
        document.addEventListener('DOMContentLoaded', function () {
          var elems = document.querySelectorAll('.datepicker');
          var instances = M.Datepicker.init(elems, {});
        });
        {/literal}
      </script>
      {/if}
    </div>

    <!-- shadowInactive -->
    <label>
      {render acl=$shadowInactiveACL checkbox=$multiple_support checked=$use_activate_shadowInactive}
      <input type="checkbox" class="center" name="activate_shadowInactive" value="1" {$activate_shadowInactive}>
      {/render}
      <span class="black-text">
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
      <span class="black-text">
        {render acl=$shadowWarningACL}
        {$shadowwarnings}
        {/render}
      </span>
    </label>

  </div>
</div>
