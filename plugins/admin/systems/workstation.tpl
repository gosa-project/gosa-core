<h2><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}</h2>
<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top;">
   <table summary="">
    <tr>
     {if $cn eq 'wdefault'}
     <td colspan=2>{t}Workstation template{/t}</td>
     {else}
     <td><LABEL for="cn">{t}Workstation name{/t}</LABEL>{$must}</td>
     <td>
      <input name="cn" id="cn" size=18 maxlength=60 value="{$cn}" {$cnACL}>
     </td>
     {/if}
    </tr>
    <tr>
     <td><LABEL for="cn">{t}Location{/t}</LABEL></td>
     <td>
      <input name="l" id="l" size=18 maxlength=60 value="{$l}" {$lACL}>
     </td>
    </tr>
    <tr>
     <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
     <td>
      <select size="1" id="base" name="base" title="{t}Choose subtree to place terminal in{/t}" {$baseACL}>
       {html_options options=$bases selected=$base_select}
      </select>
	    {if $baseACL == ""}
            <input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
        {else}
            <img src="images/folder_gray.png" class="center" title="{t}Select a base{/t}">
        {/if}
    </tr>
   </table>
{if $member_of_ogroup}
   <br>
   <br>
   <br>
   <input type='submit' name='inheritAll' value='{t}Inherit all{/t}'>
{/if}
  </td>
  <td  style="vertical-align:top;border-left:1px solid #A0A0A0;">
   <table summary="">
    <tr>
     <td>{t}Mode{/t}</td>
     <td>
      <select name="gotoMode" title="{t}Select terminal mode{/t}" {$gotoModeACL}>
       {html_options options=$modes selected=$gotoMode_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoSyslogServer">{t}Syslog server{/t}</LABEL></td>
     <td>
      <select id="gotoSyslogServer" name="gotoSyslogServer" title="{t}Choose server to use for logging{/t}" {$gotoSyslogServerACL}>
       {html_options options=$syslogservers selected=$gotoSyslogServer_select}
      </select>
     </td>
    </tr>
   </table>
<p class="seperator">&nbsp;</p>
   <table width="100%">
    <tr>
     <td colspan="2">

{if $member_of_ogroup}

        <input type="checkbox" value="1" name="inheritTimeServer"  {$gotoNtpServerACL}
            {if $inheritTimeServer } checked {/if}
            onClick="javascript:
                    changeState('gotoNtpServerSelected');
                    changeState('gotoNtpServers');
                    changeState('addNtpServer');
                    changeState('delNtpServer');">{t}Inherit time server attributes{/t}
{else}
		<input disabled type='checkbox' name='option_disabled'>{t}Inherit time server attributes{/t}
{/if}
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
       {html_options options=$gotoNtpServers}
      </select>
        <input type="submit" name="addNtpServer" value="{t}Add{/t}"     id="addNtpServer" {$gotoNtpServerACL}
         {if $inheritTimeServer} disabled {/if}>
        <input type="submit" name="delNtpServer" value="{t}Delete{/t}"  id="delNtpServer" {$gotoNtpServerACL}
         {if $inheritTimeServer} disabled {/if}>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

{if $cn neq 'wdefault'}
<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

{$netconfig}
{/if}

{if $fai_activated}
<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>
<h2><img class="center" alt="" align="middle" src="images/rocket.png"> {t}Action{/t}</h2>
<table summary="">
 <tr>
  <td>
   <select size="1" name="saction" {$actionACL} title="{t}Select action to execute for this terminal{/t}">
    <option>&nbsp;</option>
    {html_options options=$actions}
   </select>
  </td>
  <td>
   <input type=submit name="action" value="{t}Execute{/t}">
  </td>
 </tr>
</table>
{/if}
<input type="hidden" name="workgeneric_posted" value="1">
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

