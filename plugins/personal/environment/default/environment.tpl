<input type="hidden" name="iamposted" value="1">

<div class="env-wrapper">

  <div class="row">
    <div class="col s12">
      <h3>{t}Profiles{/t}</h3>
    </div>

    <div class="col s12 xl6">
      {if $multiple_support}
        {render acl=$gotoProfileACL checkbox=$multiple_support checked=$use_useProfile}
          <label>
            <input type="checkbox" value="1" {if $useProfile} checked {/if}
              name="useProfile" id="useProfile">
            <span>{t}Use profile management{/t}</span>
          </label>
        {/render}

        <div class="profile-management">
          {render acl=$gotoProfileServerACL checkbox=$multiple_support checked=$use_gotoProfileServer}
            <div class="input-field">
              <select id="gotoProfileServer" name="gotoProfileServer" size=1>
                {foreach from=$gotoProfileServers key=key item=item}
                  <option {if $gotoProfileServer == $key} selected {/if}value='{$key}'>{$item.DISPLAY}</option>
                {/foreach}
              </select>
              <label>{t}Profile path{/t}</label>
            </div>
          {/render}

          {render acl=$gotoProfileQuotaACL checkbox=$multiple_support checked=$use_gotoProfileQuota}
            <div class="input-field">
              <input type="text" name="gotoProfileQuota" value="{$gotoProfileQuota}" size="6" id="gotoProfileQuota">
              <span>{t}Profile quota{/t} ({t}MB{/t})</span>
            </div>
          {/render}

          {render acl=$gotoProfileFlagCACL checkbox=$multiple_support checked=$use_gotoProfileFlagC}
            <label>
              <input class="center" type="checkbox" name="gotoProfileFlagC" value="C" {$gotoProfileFlagCCHK} id="gotoProfileFlagC">
              <span>{t}Cache profile locally{/t}</span>
            </label>
          {/render}
        </div>
      {else}
        {render acl=$gotoProfileACL}
          <label>
            <input type="checkbox" value="1" {$useProfileCHK} name="useProfile" id="useProfile" onclick="changeState('gotoProfileServer');changeState('gotoProfileFlagC');changeState('gotoProfileQuota');">
            <span>{t}Use profile management{/t}</span>
          </label>
        {/render}

        <div class="profile-management">
          {render acl=$gotoProfileServerACL}
            <div class="input-field">
              {if $gotoProfileServerWriteable}
                <select id="gotoProfileServer" name="gotoProfileServer" size=1>
                  {foreach from=$gotoProfileServers key=key item=item}
                    {if $item.ACL != ""}
                      <option {if $gotoProfileServer == $key} selected {/if}value='{$key}'>{$item.DISPLAY}</option>
                    {/if}
                  {/foreach}
                </select>
              {else}
                <select id="dummy_pS" name="dummy_pS" disabled  size=1>
                  <option>{$gotoProfileServer}&nbsp;</option>
                </select>
              {/if}
              <label>{t}Profile path{/t}</label>
            </div>
          {/render}

          {render acl=$gotoProfileQuotaACL}
            <div class="input-field">
              <input type="text" name="gotoProfileQuota" value="{$gotoProfileQuota}" size="6" id="gotoProfileQuota">
              <label for="gotoProfileQuota">{t}Profile quota{/t} ({t}MB{/t})</label>
            </div>
          {/render}

          {render acl=$gotoProfileFlagCACL}
            <label>
              <input class="center" type="checkbox" name="gotoProfileFlagC" value="C" {$gotoProfileFlagCCHK} id="gotoProfileFlagC">
              <span>{t}Cache profile locally{/t}</span>
            </label>
          {/render}
        </div>
      {/if}
    </div>

    <div class="col s12 xl6 {if $gotoProfileACL}alignment-correction {/if}">
      {if $kiosk_enabled}
        {if $multiple_support}
          <label>
            <input type="checkbox" name="use_kiosk_server" value="1" {if $use_kiosk_server} checked {/if} onClick="
              changeState('kiosk_server');
              changeState('kiosk_profile');">
            <span>{t}Server{/t}</span>
          </label>
        {/if}


        {render acl=$gotoKioskProfileACL}
          <div class="input-field">
            <select name='kiosk_server' id="kiosk_server" onChange='document.mainform.submit();'
              {if !$use_kiosk_server && $multiple_support} disabled {/if}>
              {html_options options=$kiosk_servers selected=$kiosk_server}
            </select>
            <label for="kiosk_server">{t}Kiosk profile{/t}</label>
          </div>
        {/render}

        {if $kiosk_server != "none"}
          {render acl=$gotoKioskProfileACL}
            <div class="input-field">
              <select name='kiosk_profile' id="kiosk_profile"
                {if !$use_kiosk_server && $multiple_support} disabled {/if}>
                {html_options values=$kiosk_profiles output=$kiosk_profiles selected=$kiosk_profile}
              </select>
              <label>{t}Profile{/t}</label>
            </div>
          {/render}
        {/if}
      {else}
        <div class="input-field add">
          <select name="dummy1" disabled id="dummy1" size=1>
            <option disabled>&nbsp;</option>
          </select>
          <label for="dummy1">{t}Kiosk profile{/t}</label>
          <button class="btn-small" type="submit" disabled name="dummy2">{t}Manage{/t}</button>
        </div>

        {if $multiple_support}
          <div class="input-field">
            <input type="checkbox" value="1" class="center" name="use_gotoXResolution"
              onClick="changeState('gotoXResolution');" {if $use_gotoXResolution} checked {/if}>
            <span>{t}Resolution{/t}</span>
          </div>
        {/if}

        {render acl=$gotoXResolutionACL}
          <div class="input-field">
            <select name="gotoXResolution" id="gotoXResolution" {if !$use_gotoXResolution && $multiple_support} disabled {/if} size=1>
              {html_options values=$gotoXResolutionKeys output=$gotoXResolutions selected=$gotoXResolution}
            </select>
          </div>
        {/render}
      {/if}

      {render acl=$gotoProfileFlagLACL checkbox=$multiple_support checked=$use_gotoProfileFlagL}
        <label>
          <input type="checkbox" name="gotoProfileFlagL" id="gotoProfileFlagL" value="L" {$gotoProfileFlagLCHK} class="center">
          <span>{t}Resolution changeable during session{/t}</span>
        </label>
      {/render}
    </div>
  </div>

  <hr class="divider">

  <div class="row">
      <div class="col s12 xl6">
        <h3>{t}Shares{/t}</h3>

        {render acl=$gotoShareACL}
          {$shareList}
        {/render}

        <div class="input-field add">
          {render acl=$gotoShareACL}
            <input type="text" size=15 name="ShareUser" value="%u" title="{t}User used to connect to the share{/t}"><span class="intermediate-char">@</span>
          {/render}

          {render acl=$gotoShareACL}
            <select name="gotoShareSelection" title="{t}Select a share{/t}" size=1>
              {html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
            </select>
          {/render}

          <span class="intermediate-char">:</span>

          {render acl=$gotoShareACL}
            <input type="text" size=15 name="gotoShareMountPoint" value="%H/Netzlaufwerke/%L/%S" title="{t}Mount path{/t}">
            <button class="btn-small" type='submit' name='gotoShareAdd'>{msgPool type=addButton}</button>
          {/render}
        </div>
      </div>

      <div class="col s12 xl6">
        <h3>{t}Log on scripts{/t}</h3>

        {render acl=$gotoLogonScriptACL}
          <div class="input-field">
            <select style="width:100%;" name="gotoLogonScript" multiple size=5 id="gotoLogonScript">
              {if $multiple_support}
                {foreach from=$gotoLogonScripts item=item key=key}
                  {if $item.UsedByAllUsers}
                    <option value="{$key}">{$item.LogonPriority}&nbsp;{$item.LogonName}&nbsp;[{$item.LogonDescription}] ({t}Used by all users{/t})</option>
                  {else}
                    <option style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;' value="{$key}">{$item.LogonPriority}&nbsp;{$item.LogonName}&nbsp;[{$item.LogonDescription}] ({t}Used by some users{/t})</option>
                  {/if}
                {/foreach}
              {else}
                {html_options values=$gotoLogonScriptKeys output=$gotoLogonScripts}
              {/if}
            </select>
          </div>
        {/render}

        {render acl=$gotoLogonScriptACL}
          <button class="btn-small" type='submit' name='gotoLogonScriptNew'>{msgPool type=addButton}</button>
          <button class="btn-small" type='submit' name='gotoLogonScriptEdit' {if $gotoLogonScriptKeysCnt ==0} disabled {/if}>
            {t}Edit{/t}
          </button>
          <button class="btn-small" type='submit' name='gotoLogonScriptDel' {if $gotoLogonScriptKeysCnt ==0} disabled {/if}>
            {msgPool type=delButton}
          </button>
        {/render}
    </div>
  </div>

  <hr class="divider">

  <div class="row">
    <div class="col s12 xl6">
      <h3>{t}Hot-plug devices{/t}</h3>

      {render acl=$gotoHotplugDeviceDNACL}
        <div class="input-field">
          <select name="gotoHotplugDevice_post[]" size=5  style="width:100%;" id="gotoHotplugDevice_post" multiple>
            {if $multiple_support}
              {foreach from=$gotoHotplugDevices item=item key=key}
                {if $item.UsedByAllUsers}
                  <option value="{$key}">{$item.name}&nbsp;[{$item.description}] ({t}Used by all users{/t})</option>
                {else}
                  <option style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;' value="{$key}">{$item.name}&nbsp;[{$item.description}] ({t}Used by some users{/t})</option>
                {/if}
              {/foreach}
            {else}
              {html_options values=$gotoHotplugDeviceKeys output=$gotoHotplugDevices}
            {/if}
          </select>
        </div>
      {/render}

      {render acl=$gotoHotplugDeviceDNACL}
        <button class="btn-small" type='submit' name='gotoHotplugDeviceUse'>{msgPool type=addButton}</button>
        <button class="btn-small" type='submit' name='gotoHotplugDeviceDel' {if !$gotoHotplugDevices} disabled {/if}>{msgPool type=delButton}</button>
      {/render}
    </div>

    <div class="col s12 xl6">
      {if $multiple_support}
        <label>
          <input type="checkbox" name="use_gotoPrinter" value="1" {if $use_gotoPrinter} checked {/if} class="center" onClick="$('div_gotoPrinter').toggle();">
          <span>{t}Printer{/t}</span>
        </label>

        <div id="div_gotoPrinter" {if !$use_gotoPrinter} style="display: none;" {/if}>
          <b>{t}Using this option will overwrite the complete printer settings for all currently edited objects!{/t}</b>

          {render acl=$gotoPrinterACL}
            <div class="input-field">
              <select style="width:100%;" name="gotoPrinterSel[]" multiple size=5 id="gotoPrinter">
                {html_options options=$gotoPrinter}
              </select>
            </div>
          {/render}

          {render acl=$gotoPrinterACL}
            <button class="btn-small" type='submit' name='gotoPrinterAdd'>{msgPool type=addButton}</button>
            <button class="btn-small" type='submit' name='gotoPrinterDel' {if !$gotoPrinter} disabled {/if}>{msgPool type=delButton}</button>
            <button class="btn-small" type='submit' name='gotoPrinterEdit' {if !$gotoPrinter} disabled {/if}>{t}Toggle administrator{/t}</button>
            <button class="btn-small" type='submit' name='gotoPrinterDefault' {if !$gotoPrinter||$is_group} disabled {/if}>{t}Toggle default{/t}</button>
          {/render}
        </div>
      {else}
        <h3>{t}Printer{/t}</h3>

        {render acl=$gotoPrinterACL}
          <div class="input-field">
            <select style="width:100%;" name="gotoPrinterSel[]" multiple size=5 id="gotoPrinter">
              {html_options options=$gotoPrinter}
            </select>
          </div>
        {/render}

        {render acl=$gotoPrinterACL}
          <button class="btn-small" type='submit' name='gotoPrinterAdd'>{msgPool type=addButton}</button>
          <button class="btn-small" type='submit' name='gotoPrinterDel' {if !$gotoPrinter} disabled {/if}>{msgPool type=delButton}</button>
          <button class="btn-small" type='submit' name='gotoPrinterEdit' {if !$gotoPrinter} disabled {/if}>{t}Toggle administrator{/t}</button>
          <button class="btn-small" type='submit' name='gotoPrinterDefault' {if !$gotoPrinter||$is_group} disabled {/if}>{t}Toggle default{/t}</button>
        {/render}
       {/if}
    </div>
  </div>
</div>

{if $multiple_support}
  <input type="hidden" name="environment_multiple_posted" value="1">
{/if}

<script language="JavaScript" type="text/javascript">
  // First input field on page
  focus_field('useProfile');
</script>
