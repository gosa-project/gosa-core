<div class="system-wrapper">
     <div class="system-terminal">
            <h3>{t}Properties{/t}</h3>

            <div class="row">
                  <div class="col s12 xl6">
                        {if $cn eq 'default'}
                              <p>{t}Terminal template{/t}</p>
                        {else}
                              {render acl=$cnACL}
                                    <div class="input-field">
                                          <input type="text" name="cn" id="cn" value="{$cn}">
                                          <label for="cn">{t}Terminal name{/t}{$must}</label>
                                    </div>
                              {/render}
                        {/if}

                        {render acl=$descriptionACL}
                              <div class="input-field">
                                    <input type="text" name="description" id="description" value="{$description}" maxlength=60>
                                    <label for="description">{t}Description{/t}</label>
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

                  <div class="col s12 xl6">
                        {render acl=$gotoModeACL}
                              <div class="input-field">
                                    <select name="gotoMode" id="gotoMode" title="{t}Select terminal mode{/t}" size=1>
                                          {html_options options=$modes selected=$gotoMode_select}
                                    </select>
                                    <label for="gotoMode">{t}Mode{/t}</label>
                              </div>
                        {/render}

                        {render acl=$gotoSyslogServerACL}
                              <div class="input-field">
                                    <select name="gotoSyslogServer" id="gotoSyslogServer" title="{t}Choose server to use for logging{/t}" size=1>
                                          {html_options options=$syslogservers selected=$gotoSyslogServer_select}
                                    </select>
                                    <label for="gotoSyslogServer">{t}Syslog server{/t}</label>
                              </div>
                        {/render}

                        {render acl=$gotoSwapServerACL}
                              <div class="input-field">
                                    <select name="gotoSwapServer" id="gotoSwapServer" title="{t}Choose NFS file system to place swap files on{/t}" size=1>
                                          {html_options options=$swapservers selected=$gotoSwapServer_select}
                                    </select>
                                    <label for="gotoSwapServer">{t}Swap server{/t}</label>
                              </div>
                        {/render}

                        {render acl=$gotoSwapServerACL}
                              <div class="input-field">
                                    <select name="gotoTerminalPath" id="gotoTerminalPath" title="{t}Select NFS root file system to use{/t}" size=1>
                                          {html_options options=$nfsservers selected=$gotoTerminalPath_select}
                                    </select>
                                    <label for="gotoTerminalPath">{t}Root server{/t}</label>
                              </div>
                        {/render}

                        {if $member_of_ogroup}
                              {render acl=$gotoNtpServerACL}
                                    <label>
                                          <p>
                                                <input type="checkbox" name="inheritTimeServer" {if $inheritTimeServer} checked {/if}
                                                onClick="javascript:
                                                      changeState('gotoNtpServerSelected');
                                                      changeState('gotoNtpServers');
                                                      changeState('addNtpServer');
                                                      changeState('delNtpServer');">
                                                <span>{t}Inherit time server attributes{/t}</span>
                                          </p>
                                    </label>
                              {/render}
                        {else}
                              <label>
                                    <p>
                                          <input type="checkbox" name="option_disabled" disabled>
                                          <span>{t}Inherit time server attributes{/t}</span>
                                    </p>
                              </label>
                        {/if}

                        {render acl=$gotoNtpServerACL}
                              <div class="input-field">
                                    <select name="gotoNtpServerSelected[]" id="gotoNtpServerSelected" title="{t}Choose server to use for synchronizing time{/t}" size=1 multiple {if $inheritTimeServer} disabled {/if}>
                                          {html_options options=$gotoNtpServer_select}
                                    </select>
                                    <label for="gotoNtpServerSelected">{t}NTP server{/t}</label>
                              </div>

                              <div class="input-field add">
                                    <select name="gotoNtpServers" id="gotoNtpServers" size=1 {if $inheritTimeServer} disabled {/if}>
                                          {html_options values=$ntpservers output=$ntpservers}
                                    </select>

                                    <button class="btn-small" type="submit" name="addNtpServer" id="addNtpServer" {if $inheritTimeServer} disabled {/if}>{msgPool type=addButton}</button>

		                        <button class="btn-small" type="submit" name="delNtpServer" id="delNtpServer" {if $inheritTimeServer} disabled {/if}>{msgPool type=delButton}</button>
                              </div>
                        {/render}
                  </div>
            </div>

            {if $cn neq 'default' || $fai_activated && $si_activated || $member_of_ogroup}
                  <hr class="divider">

                  <div class="row">
                        {if $cn neq 'default'}
                              <div class="col s12 xl6">
                                    {$netconfig}
                              </div>
                        {/if}

                        <div class="col s12 xl6">
                              {if $fai_activated && $si_activated}
                                    <h3>{t}Action{/t}</h3>

                                    {render acl=$gotoNtpServerACL}
                                          <div class="input-field add">
                                                <select name="saction" id="saction" title="{t}Select action to execute for this terminal{/t}" size=1>
                                                      <option disabled>&nbsp;</option>
                                                      {html_options options=$actions}
                                                </select>
                                                <button class="btn-small" type="submit" name="action">{t}Execute{/t}</button>
                                          </div>
                                    {/render}
                              {/if}

                              {if $member_of_ogroup}
                                    <div class="member-of-ogroup">
                                          <div class="input-field">
                                                <button class="btn-small" type="submit" name="inheritAll">{t}Inherit all{/t}</button>
                                          </div>
                                    </div>
                              {/if}
                        </div>
                  </div>
            {/if}
      </div>

      <input type="hidden" name="termgeneric_posted" value="1">
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
