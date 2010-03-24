<table style="width:100%;" summary="{t}POSIX settings{/t}">
 <tr>
  <td style="width:50%">

   <h3>{t}Generic{/t}</h3>
   <table summary="{t}Generic settings{/t}">
    <tr>
     <td><label for="homeDirectory">{t}Home directory{/t}</label>{$must}</td>
     <td>
      {render acl=$homeDirectoryACL checkbox=$multiple_support checked=$use_homeDirectory}
      <input type='text' id="homeDirectory" name="homeDirectory" size=40 maxlength=120 value="{$homeDirectory}">
      {/render}
     </td>
    </tr>
    <tr>
     <td><label for="loginShell">{t}Shell{/t}</label></td>
     <td>
      {render acl=$loginShellACL checkbox=$multiple_support checked=$use_loginShell}
      <select id="loginShell" size="1" name="loginShell">
       {html_options values=$shells output=$shells selected=$loginShell}
      </select> 
      {/render}
     </td>
    </tr>
    <tr>
     <td><label for="primaryGroup">{t}Primary group{/t}</label></td>
     <td>
      {render acl=$gidNumberACL checkbox=$multiple_support checked=$use_primaryGroup}
      <select id="primaryGroup" size="1" name="primaryGroup">
       {html_options options=$secondaryGroups selected=$primaryGroup}
      </select> 
      {/render}
     </td>
    </tr>
    {if !$multiple_support}
    <tr>
     <td>{t}Status{/t}</td>
     <td>{$status}</td>
    </tr>
    {if $gotoLastSystemLogin}
    <tr>
     <td>{t}Last logon{/t}</td>
     <td>{$gotoLastSystemLogin}</td>
    </tr>
    {/if}
    {/if}
   </table>
   
   {if !$multiple_support}
   
   <table summary="{t}Account settings{/t}">
    <tr>
     <td>
      {render acl=$force_idsACL}
      <input id="force_ids" type=checkbox name="force_ids" value="1" {$force_ids} 
      onclick="{$onClickIDS}">
      {/render}
     </td>
     <td>
      <label for="force_ids">{t}Force UID/GID{/t}</label>
     </td>
     <td style="width:20px;"></td>
     <td>
      <label for="uidNumber">{t}UID{/t}</label>
     </td>
     <td>
      {render acl=$uidNumberACL}
      <input type='text' id="uidNumber" name="uidNumber" size=9 maxlength=9 {$forceMode} value="{$uidNumber}">
      {/render}
     </td>
    </tr>
    <tr>
     <td colspan="3"></td>
     <td>
      <label for="gidNumber">{t}GID{/t}</label>
     </td>
     <td>
      {render acl=$gidNumberACL}
      <input type='text' id="gidNumber" name="gidNumber" size=9 maxlength=9 {$forceMode} value="{$gidNumber}">
      {/render}
     </td>
    </tr>
   </table>
   {/if}
  </td>
  <td class='left-border'>   &nbsp;
  </td>
  <td>
   <h3>{t}Group membership{/t}
   </h3>
   {if $groups eq "too_many_for_nfs"}
   <b style="color:red">{t}(Warning: more than 16 groups are not supported by NFS!){/t}</b>
   <br>
   {/if}
   {render acl=$groupMembershipACL}
   <select style="width:100%; height:130px;" name="group_list[]" size=16 multiple >
    {if !$multiple_support}
    {html_options options=$groupMembership}
    {else}
    {foreach from=$groupMembership item=group key=key}
    <option value="{$key}">{$group}&nbsp;({t}Common group{/t})</option>
    {/foreach}
    {foreach from=$groupMembership_some item=group key=key}
    <option value="{$key}" 
    style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;'>{$group}&nbsp;({t}Groups differ{/t})</option>
    {/foreach}
    {/if}
   </select>
   {/render}
   <br>
   {render acl=$groupMembershipACL}
   <button type='submit' name='edit_groupmembership'>{msgPool type=addButton}</button>&nbsp;
   
   {/render}
   {render acl=$groupMembershipACL}
   <button type='submit' name='delete_groupmembership'>{msgPool type=delButton}</button>
   
   {/render}
  </td>
 </tr>
</table>

<hr>

<table style="width:100%" summary="{t}Account permissions{/t}" >
 <tr>
  <td style='width:50%'>
   {if $sshPublicKey == 1}
     {render acl=$sshPublicKeyACL}
     <h3>{t}SSH keys{/t}</h3>
     <button type='submit' name='edit_sshpublickey'>{t}Edit public ssh keys...{/t}</button>
     {/render}
   {/if}

   <hr>
   {include file="$pwmode.tpl"}
  </td>
  <td class='left-border'>   &nbsp;
  </td>
  <td>
   <h3>{t}System trust{/t}
   </h3>
   {if !$multiple_support}
   {t}Trust mode{/t}&nbsp;
   {render acl=$trustmodeACL}
   <select name="trustmode" id="trustmode" size=1 
   onChange="changeSelectState('trustmode', 'wslist'); 
   changeSelectState('trustmode', 'add_ws'); 
   changeSelectState('trustmode', 'del_ws');">
   {html_options options=$trustmodes selected=$trustmode}
</select>
{/render}
{render acl=$trustmodeACL}
<select style="width:100%" id="wslist" name="workstation_list[]" size=8 multiple {$trusthide}>
 {html_options values=$workstations output=$workstations}
 {if $emptyArrAccess}
 <option disabled>&nbsp;</option>
 {/if}
</select>
{/render}
<br>
{render acl=$trustmodeACL}
<button {$trusthide} type='submit' name='add_ws' id="add_ws">{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$trustmodeACL}
<button {$trusthide} type='submit' name='delete_ws' id="del_ws">{msgPool type=delButton}</button>

{/render}

{else}
<input type="checkbox" name="use_trustmode" {if $use_trustmode} checked {/if}
class="center" onClick="$('div_trustmode').toggle();">
{t}Trust mode{/t}&nbsp;
<div {if !$use_trustmode} style="display: none;" {/if} id="div_trustmode">
 {render acl=$trustmodeACL}
 <select name="trustmode" id="trustmode" size=1 
 onChange="changeSelectState('trustmode', 'wslist'); 
 changeSelectState('trustmode', 'add_ws'); 
 changeSelectState('trustmode', 'del_ws');">
 {html_options options=$trustmodes selected=$trustmode}
</select>
{/render}
{render acl=$trustmodeACL}
<select style="width:100%" id="wslist" name="workstation_list[]" size=8 multiple {$trusthide}>
 {html_options values=$workstations output=$workstations}
 {if $emptyArrAccess}
 <option disabled>&nbsp;</option>
 {/if}
</select>
{/render}
<br>
{render acl=$trustmodeACL}
<button type='submit' name='add_ws' id="add_ws">{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$trustmodeACL}
<button type='submit' name='delete_ws' id="del_ws">{msgPool type=delButton}</button>

{/render}
</div>
{/if}
</td>
</tr>
</table>
{if $multiple_support}
<input type="hidden" name="posix_mulitple_edit" value="1">
{/if}

<input type="hidden" name="posixTab" value="posixTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
<!-- // First input field on page
focus_field('homeDirectory');
-->
</script>
