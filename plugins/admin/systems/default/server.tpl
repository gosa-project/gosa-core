<div class="system-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <div class="sys-server-wrapper">
        <h3>{t}Generic{/t}</h3>

        {render acl=$cnACL}
          <div class="input-field">
              <input type="text" name="cn" id="cn" value="{$cn}">
              <label for="cn">{t}Server name{/t}{$must}</label>
          </div>
        {/render}

        {render acl=$descriptionACL}
          <div class="input-field">
              <input type="text" name="description" id="description" value="{$description}" maxlength=80 >
              <label for="description">{t}Description{/t}</label>
          </div>
        {/render}

        {render acl=$gotoModeACL}
          <div class="input-field">
              <select name="gotoMode" title="{t}Select terminal mode{/t}" size=1>
                {html_options options=$modes selected=$gotoMode}
              </select>
              <label for="gotoMode">{t}Mode{/t}</label>
          </div>
        {/render}

        {render acl=$baseACL}
          <div class="input-field ldap-tree">
            <label for="base">{t}Base{/t}{$must}</label>
            {$base}
          </div>
        {/render}

        {if $host_key}
          <hr class="divider">

          {$host_key}
        {/if}
      </div>
    </div>

    {if $netconfig}
      <div class="col s12 xl6">
        {$netconfig}
      </div>

      </div>

      {if $fai_activated && $si_activated}
        <hr class="divider">

        <div class="row">
      {/if}
    {/if}

  {if $fai_activated && $si_activated}
    <div class="col s12 xl6">
      <h3>{t}Action{/t}</h3>

      {if $currently_installing}
        <p>{t}System installation in progress, the FAI state cannot be changed right now.{/t}</p>
      {else}
        {render acl=$FAIstateACL}
          <div class="input-field add">
            <select name="saction" title="{t}Select action to execute for this server{/t}" size=1>
              <option>&nbsp;</option>
              {html_options options=$actions}
            </select>
            <button class="btn-small" type="submit" name="action">{t}Execute{/t}</button>
          </div>
        {/render}
      {/if}
    </div>
  </div>
  {/if}

  <!-- Place cursor -->
  <script language="JavaScript" type="text/javascript">
    <!-- // First input field on page
    focus_field('cn');
    -->
  </script>
</div>
