
<p>
<b>{t}Warning{/t}:</b>&nbsp;{t}Actions you choose here influence all systems in this object group. Additionally, all values editable here can be inherited by the clients assigned to this object group.{/t}
</p>

<hr>

<h3>{t}Generic{/t}</h3>

<table width="100%" summary="{t}System settings{/t}">
 <tr>
  <td style='width:50%;'><!-- Upper left -->

   <LABEL for="gotoNtpServerSelected">{t}NTP server{/t}</LABEL>
   <br>

   {render acl=$gotoNtpServerACL}
    <select name="gotoNtpServerSelected[]" id="gotoNtpServerSelected" 
      multiple size=5 style="width:100%;"						
      title="{t}Choose server to use for synchronizing time{/t}" 
      {if $inheritTimeServer} disabled {/if}>
     {html_options options=$gotoNtpServer_select}
    </select>
   {/render}

   <br>
   {render acl=$gotoNtpServerACL}
    <select name="gotoNtpServers" id="gotoNtpServers" {if $inheritTimeServer} disabled {/if}size=1>
     {html_options output=$gotoNtpServers values=$gotoNtpServers}
    </select>
   {/render}

   {render acl=$gotoNtpServerACL}
    <button type='submit' name='addNtpServer' id="addNtpServer"    
     {if ($inheritTimeServer) || (!$gotoNtpServers)}disabled{/if}
      >{msgPool type=addButton}</button>
   {/render}

   {render acl=$gotoNtpServerACL}
    <button type='submit' name='delNtpServer' id="delNtpServer"
      {if ($inheritTimeServer) || (!$gotoNtpServer_select)}disabled{/if}
      >{msgPool type=delButton}</button>
    {/render}

  </td>
  <td class='left-border'><!-- Upper right -->

   <table summary="{t}Goto settings{/t}"> 
    <tr>
     <td>{t}Mode{/t}</td>
     <td>
      {render acl=$gotoModeACL}
       <select name="gotoMode" title="{t}Select terminal mode{/t}" size=1>
        {html_options options=$modes selected=$gotoMode_select}
       </select>
      {/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoSyslogServer">{t}Syslog server{/t}</LABEL></td>
     <td>
      {render acl=$gotoSyslogServerACL}
       <select id="gotoSyslogServer" name="gotoSyslogServer" 
          title="{t}Choose server to use for logging{/t}" size=1>
        {html_options values=$syslogservers output=$syslogservers selected=$gotoSyslogServer_select}
       </select>
      {/render}
     </td>
    </tr>
    
    {if $is_termgroup}
     <tr>
      <td><LABEL for="gotoTerminalPath">{t}Root server{/t}</LABEL></td>
      <td>
       {render acl=$gotoTerminalPathACL}
        <select name="gotoTerminalPath" id="gotoTerminalPath" 
          title="{t}Select NFS root file system to use{/t}" size=1>
         {html_options options=$nfsservers selected=$gotoTerminalPath_select}
        </select>
       {/render}
      </td>
     </tr>
     <tr>
      <td><LABEL for="gotoSwapServer">{t}Swap server{/t}</LABEL></td>
      <td>
       {render acl=$gotoSwapServerACL}
        <select name="gotoSwapServer" id="gotoSwapServer" 
          title="{t}Choose NFS file system to place swap files on{/t}" size=1>
         {html_options options=$swapservers selected=$gotoSwapServer_select}
        </select>
       {/render}
      </td>
     </tr>
     
    {/if}
   </table>

  </td>
 </tr>
</table>

<hr>

<input type='checkbox' value='1' {if $members_inherit_from_group} checked {/if}name='members_inherit_from_group'>&nbsp;{t}Inherit all values to group members{/t}
<input name="workgeneric_posted" value="1" type="hidden">
<hr>

<h3>{t}Action{/t}</h3>

{render acl=$FAIstateACL}
 <select size="1" name="saction" title="{t}Select action to execute for this terminal{/t}">
  <option>&nbsp;</option>
  {html_options options=$actions}
 </select>
{/render}
{render acl=$FAIstateACL}
 <button type='submit' name='action'>{t}Execute{/t}</button>
{/render}
