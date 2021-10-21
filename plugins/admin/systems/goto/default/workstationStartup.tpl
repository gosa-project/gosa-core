<div class="system-wrapper">
  <div class="system-workstaion-startup">
    <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Boot parameters{/t}</h3>

        {if $fai_activated && $si_active && !$si_fai_action_failed}
          {render acl=$gotoBootKernelACL}
            <div class="input-field">
                <select id="gotoBootKernel" name="gotoBootKernel" size=1>
                  {html_options options=$gotoBootKernels selected=$gotoBootKernel}
                </select>
                <label for="gotoBootKernel">{t}Boot kernel{/t}</label>
            </div>
          {/render}
        {/if}

        {render acl=$gotoKernelParametersACL}
          <div class="input-field">
              <input type="text" name="gotoKernelParameters" id="gotoKernelParameters" maxlength=500 value="{$gotoKernelParameters}" title="{t}Enter any parameters that should be passed to the kernel as append line during boot up{/t}">
              <label for="gotoKernelParameters">{t}Custom options{/t}</label>
          </div>
        {/render}

        <hr class="divider">

        <h3>{t}LDAP server{/t}</h3>
        {render acl=$gotoLdapServerACL}
          {if $member_of_ogroup}
            <label>
              <p>
                  <input type="checkbox" name="gotoLdap_inherit" value="1" {if $gotoLdap_inherit} checked {/if} onClick="document.mainform.submit();">
                  <span>{t}inherit from group{/t}</span>
                </p>
            </label>
          {/if}
        {/render}

        {render acl=$gotoLdapServerACL_inherit}
          {$gotoLdapServers}

          <div class="input-field add">
            <select name="ldap_server_to_add" id="ldap_server_to_add" size=1>
              {html_options options=$gotoLdapServerList}
            </select>
            <button class="btn-small" type="submit" name="add_ldap_server" id="add_ldap_server">{msgPool type=addButton}</button>
          </div>
        {/render}
      </div>

      <div class="col s12 xl6">
        <div class="fai-wrapper">
          {if !$fai_activated}
            <h3>{t}FAI Object assignment disabled. You can't use this feature until FAI is activated.{/t}</h3>
          {elseif !$si_active}
            <h3>{t}GOsa support daemon not configured{/t}</h3>
            <p>{t}FAI settings cannot be modified{/t}</p>
          {elseif $si_fai_action_failed}
            <h3>{msgPool type=siError}</h3>
            <p>{t}Check if the GOsa support daemon (gosa-si) is running.{/t}</p>

            <button class="btn-small" type="submit" name="fai_si_retry">{t}retry{/t}</button>
          {elseif $fai_activated}
            <h3>FAI-Eigenschaften</h3>

            {if $FAIdebianMirror == "inherited"}
              <div class="row">
                <div class="col s12 xl6">
                  {render acl=$FAIdebianMirrorACL}
                    <div class="input-field">
                      <select name="FAIdebianMirror" id="FAIdebianMirror" onchange="document.mainform.submit()" size=1>
                        {foreach from=$FAIservers item=val key=key}
                          {if $key == "inherited" || $key == "auto"}
                            <option value="{$key}" {if $FAIdebianMirror == $key} selected {/if}>{t}{$key}{/t}</option>
                          {else}
                            <option value="{$key}" {if $FAIdebianMirror == $key} selected {/if}>{$key}</option>
                          {/if}
                        {/foreach}
                      </select>
                      <label for="FAIdebianMirror">{t}FAI server{/t}</label>
                    </div>
                  {/render}
                </div>

                <div class="col s12 xl6">
                  <div class="input-field">
                    <select name="FAIrelease" id="FAIrelease" size=1 disabled>
                      {html_options options=$InheritedFAIrelease output=$InheritedFAIrelease selected=$InheritedFAIrelease}
                    </select>
                    <label for="FAIrelease">{t}Release{/t}</label>
                  </div>
                </div>
              </div>

              <h3>{t}Assigned FAI classes{/t}</h3>

              {render acl=$FAIclassACL}
                {$FAIScriptlist}
              {/render}
            {else}
              <div class="row">
                <div class="col s12 xl6">
                  {render acl=$FAIdebianMirrorACL}
                    <div class="input-field">
                      <select name="FAIdebianMirror" id="FAIdebianMirror" onchange="document.mainform.submit()" size=1>
                        {foreach from=$FAIservers item=val key=key}
                          {if $key == "inherited" || $key == "auto"}
                            <option value="{$key}" {if $FAIdebianMirror == $key} selected {/if}>{t}{$key}{/t}</option>
                          {else}
                            <option value="{$key}" {if $FAIdebianMirror == $key} selected {/if}>{$key}</option>
                          {/if}
                        {/foreach}
                      </select>
                      <label for="FAIdebianMirror">{t}FAI server{/t}</label>
                    </div>
                  {/render}
                </div>

                <div class="col s12 xl6">
                  {render acl=$FAIreleaseACL}
                    <div class="input-field">
                      <select name="FAIrelease" id="FAIrelease" onchange="document.mainform.submit()" size=1>
                        {foreach from=$FAIservers.$FAIdebianMirror item=val key=key}
                          <option value="{$val}" {if $FAIrelease == $key} selected {/if}>{$val}</option>
                        {/foreach}
                      </select>
                      <label for="FAIrelease">{t}Release{/t}</label>
                    </div>
                  {/render}
                </div>
              </div>

              <h4>{t}Assigned FAI classes{/t}</h4>

              {render acl=$FAIclassACL}
                {$FAIScriptlist}

                <div class="input-field add">
                  <select name="FAIclassesSel" id="FAIclassesSel">
                    {foreach from=$FAIclasses item=val key=key}
                      <option value="{$key}">{$key}&nbsp;[{$val}]</option>
                    {/foreach}
                  </select>

                  <button class="btn-small" type="submit" name="AddClass">{msgPool type=addButton}</button>
                </div>
              {/render}
            {/if}
          {/if}
        </div>
      </div>
    </div>

    <hr class="divider">

    <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Kernel modules (format: name parameters){/t}</h3>

        {render acl=$gotoModulesACL}
          <div class="input-field">
            <select name="modules_list[]" title="{t}Add additional modules to load on startup{/t}" size=1 multiple>
              {html_options values=$gotoModules output=$gotoModules}
              <option disabled>&nbsp;</option>
            </select>
          </div>

          <div class="input-field add">
            <input type="text" name="module" id="module" maxlength=30>

            <button class="btn-small" type="submit" name="add_module" id="add_module">{msgPool type=addButton}</button>

            <button class="btn-small" type="submit" name="delete_module" id="delete_module">{msgPool type=delButton}</button>
          </div>
        {/render}
      </div>

      <div class="col s12 xl6">
        <h3>{t}Shares{/t}</h3>

        {render acl=$gotoShareACL}
          <div class="input-field">
            <select name="gotoShare" id="gotoShare" size=1 multiple>
              {html_options values=$gotoShareKeys output=$gotoShares}
              <option disabled>&nbsp;</option>
            </select>
          </div>

          <div class="input-field">
            <select name="gotoShareSelection" id="gotoShareSelection" size=1>
              {html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
              <option disabled>&nbsp;</option>
            </select>
          </div>

          <div class="input-field add">
            <input type="text" name="gotoShareMountPoint" id="gotoShareMountPoint" value="{t}Mount point{/t}">

            <button class="btn-small" type="submit" name="gotoShareAdd" id="gotoShareAdd">{msgPool type=addButton}</button>

            <button class="btn-small" type="submit" name="gotoShareDel" id="gotoShareDel" {if $gotoSharesCount == 0} disabled {/if}>{t}Remove{/t}</button>
          </div>
        {/render}
      </div>
    </div>
  </div>

  <input name="WorkstationStarttabPosted" type="hidden" value="1">

  <script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  focus_field('gotoLdapServer');
  -->
  </script>
</div>
