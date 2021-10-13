<div class="system-wrapper">
     <div class="system-devices">
          <div class="row">
               <div class="col s12 xl4">
                    <h3>{t}Keyboard{/t}</h3>

                    {render acl=$gotoXKbModelACL}
                         <div class="input-field">
                              <select name="gotoXKbModel" id="gotoXKbModel" title="{t}Choose keyboard model{/t}" size=1>
                                   {html_options options=$XKbModels selected=$gotoXKbModel_select}
                              </select>
                              <label for="gotoXKbModel">{t}Model{/t}</label>
                         </div>
                    {/render}

                    {render acl=$gotoXKbLayoutACL}
                         <div class="input-field">
                              <select name="gotoXKbLayout" id="gotoXKbLayout" title="{t}Choose keyboard layout{/t}" size=1>
                                   {html_options options=$XKbLayouts selected=$gotoXKbLayout_select}
                              </select>
                              <label for="gotoXKbLayout">{t}Layout{/t}</label>
                         </div>
                    {/render}

                    {render acl=$gotoXKbVariantACL}
                         <div class="input-field">
                              <select name="gotoXKbVariant" id="gotoXKbVariant" title="{t}Choose keyboard variant{/t}" size=1>
                                   {html_options options=$XKbVariants selected=$gotoXKbVariant_select}
                              </select>
                              <label for="gotoXKbVariant">{t}Variant{/t}</label>
                         </div>
                    {/render}
               </div>

               <div class="col s12 xl4">
                    <h3>{t}Mouse{/t}</h3>

                    {render acl=$gotoXMouseTypeACL}
                         <div class="input-field">
                              <select name="gotoXMouseType" id="gotoXMouseType" title="{t}Choose mouse type{/t}" size=1>
                                   {html_options options=$MouseTypes selected=$gotoXMouseType_select}
                              </select>
                              <label for="gotoXMouseType">{t}Type{/t}</label>
                         </div>
                    {/render}

                    {render acl=$gotoXMouseportACL}
                         <div class="input-field">
                              <select name="gotoXMouseport" id="gotoXMouseport" title="{t}Choose mouse port{/t}" size=1>
                                   {html_options options=$MousePorts selected=$gotoXMouseport_select}
                              </select>
                              <label for="gotoXMouseport">{t}Port{/t}</label>
                         </div>
                    {/render}
               </div>

               <div class="col s12 xl4">
                    <h3>{t}Telephone hardware{/t}</h3>

                    {render acl=$goFonHardwareACL}
                         <div class="input-field">
                              {$hardware_list}
                              <label for="goFonHardware">{t}Telephone{/t}</label>
                         </div>
                    {/render}
               </div>
          </div>

          <hr class="divider">

          <div class="row">
               <div class="col s12 xl4">
                    <h3>{t}Graphic device{/t}</h3>

                    {render acl=$gotoXDriverACL}
                         <div class="input-field">
                              <select name="gotoXDriver" id="gotoXDriver" title="{t}Choose graphic driver that is needed by the installed graphic board{/t}" size=1>
                                   {html_options options=$XDrivers selected=$gotoXDriver_select}
                              </select>
                              <label for="gotoXDriver">{t}Driver{/t}</label>
                         </div>
                    {/render}

                    {render acl=$gotoXResolutionACL}
                         <div class="input-field">
                              <select name="gotoXResolution" id="gotoXResolution" title="{t}Choose screen resolution used in graphic mode{/t}" size=1>
                                   {html_options options=$XResolutions selected=$gotoXResolution_select}
                              </select>
                              <label for="gotoXResolution">{t}Resolution{/t}</label>
                         </div>
                    {/render}

                    {render acl=$gotoXColordepthACL}
                         <div class="input-field">
                              <select name="gotoXColordepth" id="gotoXColordepth" title="{t}Choose color depth used in graphic mode{/t}" size=1>
                                   {html_options options=$XColordepths selected=$gotoXColordepth_select}
                              </select>
                              <label for="gotoXColordepth">{t}Color depth{/t}</label>
                         </div>
                    {/render}
               </div>

               <div class="col s12 xl4">
                    <h3>{t}Display device{/t}</h3>

                    <div class="display-type">
                         <p>{t}Type{/t}:</p>
                         {if $gotoXMonitor==""}
                              <p>{t}unknown{/t}</p>
                         {/if}

                         {$gotoXMonitor}
                    </div>

                    {render acl=$AutoSyncACL}
                         <label>
                              <input type="checkbox" name="AutoSync" value="1" {$AutoSyncCHK} onChange="changeState('gotoXHsync');changeState('gotoXVsync');">
                              <span>{t}Use DDC for automatic detection{/t}</span>
                         </label>
                    {/render}

                    {render acl=$gotoXHsyncACL}
                         <div class="input-field add">
                              <input type="text" name="gotoXHsync" id="gotoXHsync" value="{$gotoXHsync}" title="{t}Horizontal refresh frequency for installed monitor{/t}" maxlength=60>
                              <label for="gotoXHsync">{t}Horizontal synchronization{/t}</label>
                              <span>kHz</span>
                         </div>
                    {/render}

                    {render acl=$gotoXVsyncACL}
                         <div class="input-field add">
                              <input type="text" name="gotoXVsync" id="gotoXVsync" value="{$gotoXVsync}" title="{t}Vertical refresh frequency for installed monitor{/t}" maxlength=60 {$hiddenState}>
                              <label for="gotoXVsync">{t}Vertical synchronization{/t}</label>
                              <span>Hz</span>
                         </div>
                    {/render}
               </div>

               <div class="col s12 xl4">
                    <h3>{t}Scan device{/t}</h3>

                    {render acl=$gotoScannerEnableACL}
                         <label>
                              <input type="checkbox" name="gotoScannerEnable" value="1" {$gotoScannerEnable} title="{t}Select to start SANE scan service on terminal{/t}">
                              <span>{t}Provide scan services{/t}</span>
                         </label>
                    {/render}
               </div>
          </div>
     </div>

     <input type="hidden" name="workservicePosted" value="1">
</div>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('gotoXKbModel');
  -->
</script>
