<div class="system-wrapper">
  <div class="system-terminal-startup">
    <h2>{t}Boot parameters{/t}</h2>

    <div class="row">
      <div class="col s12 xl6">
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

        <h3>{t}LDAP server{/t}</h3>
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
        {render acl=$gotoBootKernelACL}
          <div class="input-field">
              <select id="gotoBootKernel" name="gotoBootKernel" size=1>
                {html_options options=$gotoBootKernels selected=$gotoBootKernel}
                <option disabled>&nbsp;</option>
              </select>
              <label for="gotoBootKernel">{t}Boot kernel{/t}</label>
          </div>
        {/render}

        {render acl=$gotoKernelParametersACL}
          <div class="input-field">
              <input type="text" name="gotoKernelParameters" id="gotoKernelParameters" maxlength=500 value="{$gotoKernelParameters}" title="{t}Enter any parameters that should be passed to the kernel as append line during boot up{/t}">
              <label for="gotoKernelParameters">{t}Custom options{/t}</label>
          </div>
        {/render}
      </div>
    </div>

    {if $gotoModulesACL || $gotoShareACL}
      <hr class="divider">

      <div class="row">
        {render acl=$gotoModulesACL}
          <div class="col s12 xl6">
            <h3>{t}Kernel modules (format: name parameters){/t}</h3>

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
          </div>
        {/render}

        {render acl=$gotoShareACL}
          <div class="col s12 xl6">
            <h3>{t}Shares{/t}</h3>

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
          </div>
        {/render}
      </div>
    {/if}
  </div>

  <input type="hidden" name="TerminalStarttabPosted" value="1">
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('gotoLdapServer');
  -->
</script>
