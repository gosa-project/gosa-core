<div class="system-wrapper">
      <div class="row">
            <div class="system-printer">
                  {if $StandAlone}
                        <div class="col s12 xl6">
                              <h3>{t}General{/t}</h3>

                              {render acl=$cnACL}
                                    <div class="input-field">
                                          <input type="text" name="cn" id="cn" value="{$cn}" maxlength=60>
                                          <label for="cn">{t}Printer name{/t}{$must}</label>
                                    </div>
                              {/render}

                              {render acl=$descriptionACL}
                                    <div class="input-field">
                                          <input type="text" name="description" id="description" value="{$description}" maxlength=80>
                                          <label for="description">{t}Description{/t}</label>
                                    </div>
                              {/render}

                              {render acl=$baseACL}
                                    <div class="input-field ldap-tree">
                                          <label for="base">{t}Base{/t}{$must}</label>
                                          {$base}
                                    </div>
                              {/render}
                        </div>
                  {/if}

                  <div class="col s12 xl6">
                        <h3>{t}Details{/t}</h3>

                        {if !$StandAlone}
                              {render acl=$descriptionACL}
                                    <div class="input-field">
                                          <input type="text" name="description" id="description" value="{$description}" maxlength=80>
                                          <label for="description">{t}Description{/t}</label>
                                    </div>
                              {/render}
                        {/if}

                        {render acl=$lACL}
                              <div class="input-field">
                                    <input type="text" name="l" id="l" value="{$l}" maxlength=80>
                                    <label for="l">{t}Printer location{/t}</label>
                              </div>
                        {/render}

                        {render acl=$labeledURIACL}
                              <div class="input-field">
                                    <input type="text" name="labeledURI" id="labeledURI" value="{$labeledURI}" maxlength=80>
                                    <label for="labeledURI">{t}Printer URL{/t}{$must}</label>
                              </div>
                        {/render}

                        {if $displayServerPath && 0}
                              <div class="input-field">
                                    <input type="text" name="ppdServerPart" id="ppdServerPart" value="{$ppdServerPart}">
                                    <label for="ppdServerPart">{t}PPD Provider{/t}</label>
                              </div>
                        {/if}

                        <div class="driver-config">
                              <div class="driver-info">
                                    <p>{t}Driver{/t}:</p>
                                    <p><i>{$driverInfo}</i></p>
                              </div>

                              {render acl=$gotoPrinterPPDACL mode=read_active}
                                    <button class="btn-small" type="submit" name="EditDriver">{t}Edit{/t}</button>
                              {/render}

                              {render acl=$gotoPrinterPPDACL}
                                    <button class="btn-small" type="submit" name="RemoveDriver">{t}Remove{/t}</button>
                              {/render}
                        </div>
                  </div>
            </div>
      {if $StandAlone || $netconfig}
      </div>

      <hr class="divider">

      <div class="row">
      {/if}
            <div class="col s12 xl6">
                  <h3>{t}Permissions{/t}</h3>

                  {render acl=$gotoUserPrinterACL}
                        <div class="input-field add">
                              <select name="UserMember[]" id="UserMember" title="{t}Users{/t}" size=1 multiple>
                                    {html_options options=$UserMembers values=$UserMemberKeys}
                              </select>
                              <label for="UserMember">{t}Users which are allowed to use this printer{/t}</label>

                              <button class="btn-small" type="submit" name="AddUser">{msgPool type=addButton}</button>
                              <button class="btn-small" type="submit" name="DelUser">{msgPool type=delButton}</button>
                        </div>

                        <div class="input-field add">
                              <select name="AdminMember[]" id="AdminMember" title="{t}Administrators{/t}" size=1 multiple>
                                    {html_options options=$AdminMembers values=$AdminMemberKeys}
                              </select>
                              <label for="AdminMember">{t}Users which are allowed to administrate this printer{/t}</label>

                              <button class="btn-small" type="submit" name="AddAdminUser">{msgPool type=addButton}</button>
                              <button class="btn-small" type="submit" name="DelAdmin">{msgPool type=delButton}</button>
                        </div>
                  {/render}
            </div>

            {if $netconfig ne ''}
                  <div class="col s12 xl6">
                        {$netconfig}
                  </div>
            {/if}
      </div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">

  <!-- // First input field on page
  if(document.mainform.cn)
	focus_field('cn');
  -->
</script>
