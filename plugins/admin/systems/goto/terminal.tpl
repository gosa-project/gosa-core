<h3>{t}Properties{/t}</h3>
<table summary="{t}Terminal{/t}" style="width:100%;">
 <tr>
  <td style='width:50%; '>

   <table summary="{t}Generic settings{/t}">
    <tr>
     {if $cn eq 'default'}
     <td colspan=2>{t}Terminal template{/t}</td>
     {else}
     <td><LABEL for="cn">{t}Terminal name{/t}</LABEL>{$must}</td>
     <td>
{render acl=$cnACL}
      <input type='text' name="cn" id="cn" size=18 value="{$cn}">
{/render}
     </td>
     {/if}
    </tr>
    <tr>
     <td><LABEL for="description">{t}Description{/t}</LABEL></td>
     <td>
{render acl=$descriptionACL}
      <input type='text' name="description" id="description" size=18 maxlength=60 value="{$description}">
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
     <td>

{render acl=$baseACL}
       {$base}
{/render}
     </td>
    </tr>
   </table>

   <hr>
   {$host_key}

  </td>
  <td class='left-border'>

   <table summary="{t}Server and mode settings{/t}" style='width:100%'>
    <tr>
     <td style='width:20%'><LABEL for="gotoMode">{t}Mode{/t}</LABEL></td>
     <td>
{render acl=$gotoModeACL}
      <select id="gotoMode" name="gotoMode" title="{t}Select terminal mode{/t}" size=1>
       {html_options options=$modes selected=$gotoMode_select}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoSyslogServer">{t}Syslog server{/t}</LABEL></td>
     <td>
{render acl=$gotoSyslogServerACL}
      <select name="gotoSyslogServer" id="gotoSyslogServer" title="{t}Choose server to use for logging{/t}" size=1>
       {html_options options=$syslogservers selected=$gotoSyslogServer_select}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoSwapServer">{t}Swap server{/t}</LABEL></td>
     <td>
{render acl=$gotoSwapServerACL}
      <select name="gotoSwapServer" id="gotoSwapServer" title="{t}Choose NFS file system to place swap files on{/t}" size=1>
       {html_options options=$swapservers selected=$gotoSwapServer_select}
      </select>
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoTerminalPath">{t}Root server{/t}</LABEL></td>
      <td>
{render acl=$gotoTerminalPathACL}
      <select name="gotoTerminalPath" id="gotoTerminalPath" title="{t}Select NFS root file system to use{/t}" size=1>
       {html_options options=$nfsservers selected=$gotoTerminalPath_select}
      </select>
{/render}
     </td>
    </tr>
   </table>

   <hr> 

{if $member_of_ogroup}
{render acl=$gotoNtpServerACL}
		<input type="checkbox" value="1" name="inheritTimeServer"
			{if $inheritTimeServer} checked {/if} 
			onClick="javascript:
					changeState('gotoNtpServerSelected');
					changeState('gotoNtpServers');
					changeState('addNtpServer');
					changeState('delNtpServer');">{t}Inherit time server attributes{/t}
{/render}
{else}
	<input disabled type='checkbox' name='option_disabled'>{t}Inherit time server attributes{/t}
{/if}
     <LABEL for="gotoNtpServerSelected">{t}NTP server{/t}</LABEL><br>
{render acl=$gotoNtpServerACL}
      <select name="gotoNtpServerSelected[]" id="gotoNtpServerSelected" multiple size=5 style="width:100%;"
			title="{t}Choose server to use for synchronizing time{/t}" {if $inheritTimeServer} disabled {/if}>
       {html_options options=$gotoNtpServer_select}
      </select>
{/render}
	 <br>
{render acl=$gotoNtpServerACL}
      <select name="gotoNtpServers" id="gotoNtpServers" {if $inheritTimeServer} disabled {/if}  size=1>
       {html_options values=$ntpservers output=$ntpservers}
      </select>
{/render}
{render acl=$gotoNtpServerACL}
		<button type='submit' name='addNtpServer' id="addNtpServer"
		 {if $inheritTimeServer} disabled {/if}>{msgPool type=addButton}</button>
{/render}
{render acl=$gotoNtpServerACL}
		<button type='submit' name='delNtpServer' id="delNtpServer"
		 {if $inheritTimeServer} disabled {/if}
      >{msgPool type=delButton}</button>
{/render}

  </td>
 </tr>
</table>

{if $cn neq 'default'}
<hr>

{$netconfig}
{/if}
<hr>

{if $fai_activated && $si_activated}
<h3>{t}Action{/t}</h3>
        {render acl=$gotoNtpServerACL}
          <select size="1" name="saction" title="{t}Select action to execute for this terminal{/t}">
          <option disabled>&nbsp;</option>
          {html_options options=$actions}
        </select>
        {/render}
        {render acl=$gotoNtpServerACL}
        <button type='submit' name='action'>{t}Execute{/t}</button>
        {/render}
{/if}
{if $member_of_ogroup}
   <button type='submit' name='inheritAll'>{t}Inherit all{/t}</button>

{/if}

<input type="hidden" name="termgeneric_posted" value="1">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
