<h2><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}</h2>
<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <table summary="">
    <tr>
     {if $cn eq 'default'}
     <td colspan=2>{t}Terminal template{/t}</td>
     {else}
     <td><LABEL for="cn">{t}Terminal name{/t}</LABEL>{$must}</td>
     <td>
      <input name="cn" id="cn" size=18 maxlength=60 value="{$cn}">
     </td>
     {/if}
    </tr>
    <tr>
     <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
     <td>
      <select size="1" id="base" name="base" title="{t}Choose subtree to place terminal in{/t}">
       {html_options options=$bases selected=$base_select}
      </select>
		<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
    </tr>
   </table>
  </td>
  <td style="vertical-align:top;border-left:1px solid #A0A0A0;" >
   <table summary="">
    <tr>
     <td><LABEL for="gotoMode">{t}Mode{/t}</LABEL></td>
     <td>
      <select id="gotoMode" name="gotoMode" title="{t}Select terminal mode{/t}" {$gotoModeACL}>
       {html_options options=$modes selected=$gotoMode_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoSyslogServer">{t}Syslog server{/t}</LABEL></td>
     <td>
      <select name="gotoSyslogServer" id="gotoSyslogServer" title="{t}Choose server to use for logging{/t}" {$gotoSyslogServerACL}>
       {html_options values=$syslogservers output=$syslogservers selected=$gotoSyslogServer_select}
      </select>
     </td>
    </tr>
	</table>
  </td>
 </tr>
</table>
<p class="seperator" style="margin:0px; padding:0px;">&nbsp;</p>
<table style="width:100%;">
 <tr>
  <td style="width:50%;vertical-align:top;">
   <table summary="">
    <tr>
     <td><LABEL for="gotoTerminalPath">{t}Root server{/t}</LABEL></td>
     <td>
      <select name="gotoTerminalPath" id="gotoTerminalPath" title="{t}Select NFS root filesystem to use{/t}" {$gotoTerminalPathACL}>
       {html_options values=$nfsservers output=$nfsservers selected=$gotoTerminalPath_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoSwapServer">{t}Swap server{/t}</LABEL></td>
     <td>
      <select name="gotoSwapServer" id="gotoSwapServer" title="{t}Choose NFS filesystem to place swap files on{/t}" {$gotoSwapServerACL}>
       {html_options values=$nfsservers output=$nfsservers selected=$gotoSwapServer_select}
      </select>
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0;">
	<table width="100%">
    <tr>
	 <td colspan="2">
		<input type="checkbox" value="1" name="inheritTimeServer"
			{if $inheritTimeServer } checked {/if} 
			onClick="javascript: document.mainform.submit();">{t}Inherit time server attributes{/t}
	 </td>
	</tr>
	<tr>
     <td colspan="2" style='padding-left:14px;padding-top:5px;'><LABEL for="gotoNtpServerSelected">{t}NTP server{/t}</LABEL><br>
      <select name="gotoNtpServerSelected[]" id="gotoNtpServerSelected" multiple size=5 style="width:100%;"
			title="{t}Choose server to use for synchronizing time{/t}" {$gotoNtpServerACL} {if $inheritTimeServer} disabled {/if}>
       {html_options options=$gotoNtpServer_select}
      </select>
	 <br>
      <select name="gotoNtpServers" id="gotoNtpServers" {$gotoNtpServerACL} {if $inheritTimeServer} disabled {/if} >
       {html_options values=$ntpservers output=$ntpservers }
      </select>
		<input type="submit" name="addNtpServer" value="{t}Add{/t}"		id="addNtpServer" {$gotoNtpServerACL}
		 {if $inheritTimeServer} disabled {/if}>
		<input type="submit" name="delNtpServer" value="{t}Delete{/t}"	id="delNtpServer" {$gotoNtpServerACL}
		 {if $inheritTimeServer} disabled {/if}>
     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>
<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

{$netconfig}

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>
<h2><img class="center" alt="" align="middle" src="images/rocket.png"> {t}Action{/t}</h2>
<table summary="">
 <tr>
  <td>
   <select size="1" name="saction" {$actionACL} title="{t}Select action to execute for this terminal{/t}">
    <option disabled>&nbsp;</option>
    {html_options options=$actions}
   </select>
  </td>
  <td>
   <input type=submit name="action" value="{t}Execute{/t}">
  </td>
 </tr>
</table>


<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
