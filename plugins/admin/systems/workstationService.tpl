<table style="width:100%;" summary="">
 <tr>
  <td style="width:33%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/keyboard.png"> {t}Keyboard{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="gotoXKbModel">{t}Model{/t}</LABEL></td>
     <td>
      <select id="gotoXKbModel" name="gotoXKbModel" title="{t}Choose keyboard model{/t}" {$gotoXKbModelACL}>
       {html_options options=$XKbModels selected=$gotoXKbModel_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXKbLayout">{t}Layout{/t}</LABEL></td>
     <td>
      <select id="gotoXKbLayout" name="gotoXKbLayout" title="{t}Choose keyboard layout{/t}" {$gotoXKbLayoutACL}>
       {html_options options=$XKbLayouts selected=$gotoXKbLayout_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXKbVariant">{t}Variant{/t}</LABEL></td>
     <td>
      <select id="gotoXKbVariant" name="gotoXKbVariant" title="{t}Choose keyboard variant{/t}" {$gotoXKbVariantACL}>
       {html_options options=$XKbVariants selected=$gotoXKbVariant_select}
      </select>
     </td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  
  <td style="vertical-align:top;width:32%">
   <h2><img class="center" alt="" align="middle" src="images/mouse.png"> {t}Mouse{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="gotoXMouseType">{t}Type{/t}</LABEL></td>
     <td>
      <select name="gotoXMouseType" id="gotoXMouseType" title="{t}Choose mouse type{/t}" {$gotoXMouseTypeACL}>
       {html_options options=$MouseTypes selected=$gotoXMouseType_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXMouseport">{t}Port{/t}</LABEL></td>
     <td>
      <select id="gotoXMouseport" name="gotoXMouseport" title="{t}Choose mouse port{/t}" {$gotoXMouseportACL}>
       {html_options options=$MousePorts selected=$gotoXMouseport_select}
      </select>
     </td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  
  <td style="vertical-align:top;width:33%">
   <h2><img class="center" alt="" align="middle" src="images/select_phone.png"> {t}Telephone hardware{/t}</h2>
   <table style="width:100%" border=0 summary="">
    <tr>
     <td>{t}Telephone{/t}&nbsp;
	  {$hardware_list}
     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>

<table style="width:100%;" summary="">
 <tr><td colspan=5><p class="plugbottom" style="text-align:left;"></p></td></tr>
 <tr>
   <td style="width:33%;vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/hardware.png"> {t}Graphic device{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="gotoXDriver">{t}Driver{/t}</LABEL></td>
     <td>
      <select id="gotoXDriver" name="gotoXDriver" title="{t}Choose graphic driver that is needed by the installed graphic board{/t}" {$gotoXDriverACL}>
       {html_options values=$XDrivers output=$XDrivers selected=$gotoXDriver_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXResolution">{t}Resolution{/t}</LABEL></td>
     <td>
      <select id="gotoXResolution" name="gotoXResolution" title="{t}Choose screen resolution used in graphic mode{/t}" {$gotoXResolutionACL}>
       {html_options options=$XResolutions selected=$gotoXResolution_select}
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXColordepth">{t}Color depth{/t}</LABEL></td>
     <td>
      <select id="gotoXColordepth" name="gotoXColordepth" title="{t}Choose colordepth used in graphic mode{/t}" {$gotoXColordepthACL}>
       {html_options options=$XColordepths selected=$gotoXColordepth_select}
      </select>
     </td>
    </tr>
   </table>
   </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

   <td style="width:32%; vertical-align:top;">
   <h2><img class="center" alt="" align="middle" src="images/display.png"> {t}Display device{/t}</h2>
   <table summary="">
    <tr>
     <td>{t}Type{/t}</td>
     <td>{if $gotoXMonitor==""}{t}unknown{/t}{/if}{$gotoXMonitor}</td>
    </tr>
    <tr>
    	<td><input type="checkbox" name="AutoSync" value="1" {$AutoSyncACL} {$AutoSyncCHK} onChange="changeState('gotoXHsync');changeState('gotoXVsync');"></td>
	<td>{t}Use DDC for automatic detection{/t}</td>
    </tr>
    <tr>
     <td><LABEL for="gotoXHsync">{t}HSync{/t}</LABEL></td>
     <td><input id="gotoXHsync" name="gotoXHsync" size=10 maxlength=60 {$gotoXHsyncACL} {$hiddenState}
                value="{$gotoXHsync}" title="{t}Horizontal refresh frequency for installed monitor{/t}"> kHz</td>
    </tr>
    <tr>
     <td><LABEL for="gotoXVsync">{t}VSync{/t}</LABEL></td>
     <td><input id="gotoXVsync"  name="gotoXVsync" size=10 maxlength=60 {$gotoXVsyncACL} {$hiddenState}
                value="{$gotoXVsync}" title="{t}Vertical refresh frequency for installed monitor{/t}"> Hz</td>
    </tr>
   </table>

   </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="width:33%; vertical-align:top;">
  

   <h2><img class="center" alt="" align="middle" src="images/scanner.png"> {t}Scan device{/t}</h2>
   <input type=checkbox name="gotoScannerEnable" value="1" title="{t}Select to start SANE scan service on terminal{/t}" {$gotoScannerEnable} {$gotoScannerEnableACL}>
   {t}Provide scan services{/t}
   <br>
	</td>
 </tr>
</table>


<div style="height:40px;"></div>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.gotoXKbModel.focus();
  -->
</script>




