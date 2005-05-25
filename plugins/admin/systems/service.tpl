<table style="width:100%;">
 <tr>
  <td style="width:33%; vertical-align:top;">
   <h2><img align="middle" src="images/keyboard.png"> {t}Keyboard{/t}</h2>
   <table>
    <tr>
     <td>{t}Model{/t}</td>
     <td>
      <select name="gotoXKbModel" title="{t}Choose keyboard model{/t}" {$gotoXKbModelACL}>
       {html_options values=$XKbModels output=$XKbModels selected=$gotoXKbModel_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Layout{/t}</td>
     <td>
      <select name="gotoXKbLayout" title="{t}Choose keyboard layout{/t}" {$gotoXKbLayoutACL}>
       {html_options values=$XKbLayouts output=$XKbLayouts selected=$gotoXKbLayout_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Variant{/t}</td>
     <td>
      <select name="gotoXKbVariant" title="{t}Choose keyboard variant{/t}" {$gotoXKbVariantACL}>
       {html_options values=$XKbVariants output=$XKbVariants selected=$gotoXKbVariant_select}
      </select>
     </td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  
  <td style="vertical-align:top;width:32%">
   <h2><img align="middle" src="images/mouse.png"> {t}Mouse{/t}</h2>
   <table>
    <tr>
     <td>{t}Type{/t}</td>
     <td>
      <select name="gotoXMouseType" title="{t}Choose mouse type{/t}" {$gotoXMouseTypeACL}>
       {html_options values=$MouseTypes output=$MouseTypes selected=$gotoXMouseType_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Port{/t}</td>
     <td>
      <select name="gotoXMouseport" title="{t}Choose mouse port{/t}" {$gotoXMouseportACL}>
       {html_options values=$MousePorts output=$MousePorts selected=$gotoXMouseport_select}
      </select>
     </td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  
  <td style="vertical-align:top;width:33%">
   <h2><img align="middle" src="images/select_phone.png"> {t}Telephone hardware{/t}</h2>
   <table style="width:100%" border=0>
    <tr>
     <td>{t}Telephone{/t}&nbsp;
	  {$hardware_list}
     </td>
    </tr>
   </table>

  </td>
 </t
</table>

<table style="width:100%;">
 <tr><td colspan=5><p class="plugbottom" style="text-align:left;"></p></td></tr>
 <tr>
   <td style="width:33%;vertical-align:top;">
   <h2><img align="middle" src="images/hardware.png"> {t}Graphic device{/t}</h2>
   <table>
    <tr>
     <td>{t}Driver{/t}</td>
     <td>
      <select name="gotoXDriver" title="{t}Choose graphic driver that is needed by the installed graphic board{/t}" {$gotoXDriverACL}>
       {html_options values=$XDrivers output=$XDrivers selected=$gotoXDriver_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Resolution{/t}</td>
     <td>
      <select name="gotoXResolution" title="{t}Choose screen resolution used in graphic mode{/t}" {$gotoXResolutionACL}>
       {html_options values=$XResolutions output=$XResolutions selected=$gotoXResolution_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Color depth{/t}</td>
     <td>
      <select name="gotoXColordepth" title="{t}Choose colordepth used in graphic mode{/t}" {$gotoXColordepthACL}>
       {html_options values=$XColordepths output=$XColordepths selected=$gotoXColordepth_select}
      </select>
     </td>
    </tr>
   </table>
   </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

   <td style="width:32%; vertical-align:top;">
   <h2><img align="middle" src="images/display.png"> {t}Display device{/t}</h2>
   <table>
    <tr>
     <td>{t}Type{/t}</td>
     <td>{$gotoXMonitor}</td>
    </tr>
    <tr>
     <td>{t}HSync{/t}</td>
     <td><input name="gotoXHsync" size=10 maxlength=60 {$gotoXHsyncACL}
                value="{$gotoXHsync}" title="{t}Horizontal refresh frequency for installed monitor{/t}"> kHz</td>
    </tr>
    <tr>
     <td>{t}VSync{/t}</td>
     <td><input name="gotoXVsync" size=10 maxlength=60 {$gotoXVsyncACL}
                value="{$gotoXVsync}" title="{t}Vertical refresh frequency for installed monitor{/t}"> Hz</td>
    </tr>
   </table>

   </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="width:33%; vertical-align:top;">
   <h2><img align="middle" src="images/server.png"> {t}Remote desktop{/t}</h2>
   <table>
    <tr>
     <td></td>
     <td>{t}Connect method{/t}</td>
     <td>
      <select name="gotoXMethod" title="{t}Choose method to connect to terminal server{/t}" {$gotoXMethodACL}>
       {html_options options=$XMethods selected=$gotoXMethod_select}
      </select>
     </td>
    </tr>
    <tr>
     <td></td>
     <td>{t}Terminal server{/t}</td>
     <td>
      <select name="gotoXdmcpServer" title="{t}Select specific terminal server to use{/t}" {$gotoXdmcpServerACL}>
       {html_options values=$xdmcpservers output=$xdmcpservers selected=$gotoXdmcpServer_select}
      </select>
     </td>
    </tr>
    <tr>
     <td></td>
     <td>{t}Font server{/t}</td>
     <td>
      <select name="gotoFontPath" title="{t}Select specific font server to use{/t}" {$gotoFontPathACL}>
       {html_options values=$fontservers output=$fontservers selected=$gotoFontPath_select}
      </select>
     </td>
    </tr>
   </table>
   
  </td>
 </tr>
</table>

<table style="width:100%;">
 <tr><td colspan=3><p class="plugbottom" style="margin-top:0px;"></p></td></tr>

 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2><img align="middle" src="images/printer.png"> {t}Print device{/t}</h2>
   <input type=checkbox name="gotoLpdEnable" value="1" {$gotoLpdEnable} {$gotoLpdEnableACL} title="{t}Select to start IPP based printing service on terminal{/t}">
   {t}Provide print services{/t}
   <br>

   <table>
    <tr>
     <td><div style="width:20px;"></div></td>
     <td>{t}Spool server{/t}</td>
     <td>
      <select name="gotoLpdServer" title="{t}Select NFS filesystem to place spool files on{/t}" {$gotoLpdServerACL}>
       {html_options values=$nfsservers output=$nfsservers selected=$gotoLpdServer_select}
      </select>
     </td>
    </tr>
  </table>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <td style="vertical-align:top;">
   <h2><img align="middle" src="images/scanner.png"> {t}Scan device{/t}</h2>
   <input type=checkbox name="gotoScannerEnable" value="1" title="{t}Select to start SANE scan service on terminal{/t}" {$gotoScannerEnable} {$gotoScannerEnableACL}>
   {t}Provide scan services{/t}
   <br>
   <table>
    <tr>
     <td><div style="width:20px;"></div></td>
     <td>{t}Model{/t}</td>
     <td>
      <select name="gotoScannerModel" title="{t}Select scanner driver to use{/t}" {$gotoScannerEnableACL}>
       {html_options values=$gotoScannerModels output=$gotoScannerModels selected=$gotoScannerModel_select}
      </select>
     </td>
    </tr>
  </table>

  </td>
 </tr>
</table>

<div style="height:40px;"></div>
