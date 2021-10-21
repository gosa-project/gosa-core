<div class="system-wrapper">
     <div class="system-workstation">
          <h3>{t}Properties{/t}</h3>

          <div class="row">
               <div class="col s12 xl6">
                    {if $cn eq 'wdefault'}
                         <p>{t}Workstation template{/t}</p>
                    {else}
                         {render acl=$cnACL}
                              <div class="input-field">
                                   <input type="text" name="cn" id="cn" value="{$cn}">
                                   <label for="cn">{t}Workstation name{/t}{$must}</label>
                              </div>
                         {/render}
                    {/if}

                    {render acl=$descriptionACL}
                         <div class="input-field">
                              <input type="text" name="description" id="description" value="{$description}" maxlength=60>
                              <label for="description">{t}Description{/t}</label>
                         </div>
                    {/render}

                    {render acl=$lACL}
                         <div class="input-field">
                              <input type="text" name="l" id="l" value="{$l}" maxlength=60>
                              <label for="l">{t}Location{/t}</label>
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
                              <select name="gotoMode" title="{t}Select terminal mode{/t}" size=1>
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

                    <hr class="divider">

                    {if $member_of_ogroup}
                         {render acl=$gotoNtpServerACL}
                              <label>
                                   <p>
                                        <input type="checkbox" name="inheritTimeServer" value="1" {if $inheritTimeServer} checked {/if}
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
                                   {html_options options=$gotoNtpServers}
                              </select>

                              <button class="btn-small" type="submit" name="addNtpServer" id="addNtpServer"
                              {if $inheritTimeServer} disabled {/if}>{msgPool type=addButton}</button>

                              <button class="btn-small" type="submit" name="delNtpServer" id="delNtpServer"
                              {if $inheritTimeServer} disabled {/if}>{msgPool type=delButton}</button>
                         </div>
                    {/render}
               </div>
          </div>
     </div>

     {if $cn neq 'wdefault' || $fai_activated && $si_activated || $member_of_ogroup}
          <hr class="divider">

          <div class="row">
               {if $cn neq 'wdefault'}
                    <div class="col s12 xl6">
                         {$netconfig}
                    </div>
               {/if}

               {if $fai_activated && $si_activated}
                    <div class="col s12 xl6">
                         <h3>{t}Action{/t}</h3>

                         {render acl=$FAIstateACL}
                              <div class="input-field add">
                                   <select name="saction" title="{t}Select action to execute for this terminal{/t}" size=1>
                                        <option>&nbsp;</option>
                                        {html_options options=$actions}
                                   </select>

                                   {if $currently_installing}
                                        {render acl=r}
                                             <button class="btn-small" type="submit" name="action">{t}Execute{/t}</button>
                                        {/render}
                                   {else}
                                        {render acl=$FAIstateACL}
                                             <button class="btn-small" type="submit" name="action">{t}Execute{/t}</button>
                                        {/render}
                                   {/if}
                              </div>
                         {/render}
                    </div>
               {/if}

               {if $member_of_ogroup}
                    <button class="btn-small" type="submit" name="inheritAll">{t}Inherit all values from group{/t}</button>
               {/if}
          </div>
     {/if}

     <input type="hidden" name="workgeneric_posted" value="1">
</div>

{if $cn eq 'wdefault'}

<!-- Place cursor -->
  <script language="JavaScript" type="text/javascript">
    <!-- // First input field on page
    focus_field('l');
    -->
  </script>
{else}
  <script language="JavaScript" type="text/javascript">
    <!-- // First input field on page
    focus_field('cn');
    -->
  </script>
{/if}
