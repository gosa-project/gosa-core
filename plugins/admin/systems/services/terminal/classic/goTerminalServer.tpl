<h3>{t}Terminal service{/t}</h3>

{render acl=$goXdmcpIsEnabledACL}
 <input type="checkbox" value="true" name="goXdmcpIsEnabled" id="Temporariid" {if $goXdmcpIsEnabled} checked {/if}>
{/render}
{t}Temporary disable login{/t}

<h3>{t}Supported session types{/t}</h3>
{foreach from=$gotoSessionTypes item=item key=key}
 <input class="center" type="checkbox" name="gotoSessionType_{$key}" 
  {if $item} checked {/if}>&nbsp;{$key} <br>
{/foreach}

<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
<input type="hidden" name="goTerminalServerPosted" value="1">
