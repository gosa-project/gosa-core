<table style="width:100%;" summary="{t}Workstation service{/t}">
 <tr>
  <td style='width:33%; '>

   <h3>{t}Keyboard{/t}</h3>
   <table summary="{t}Keyboard{/t}">
    <tr>
     <td><LABEL for="gotoXKbModel">{t}Model{/t}</LABEL></td>
     <td>

{render acl=$gotoXKbModelACL}
      <select id="gotoXKbModel" name="gotoXKbModel" title="{t}Choose keyboard model{/t}" size=1>
       {html_options options=$XKbModels selected=$gotoXKbModel_select}
      </select>
{/render}

     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXKbLayout">{t}Layout{/t}</LABEL></td>
     <td>

{render acl=$gotoXKbLayoutACL}
      <select id="gotoXKbLayout" name="gotoXKbLayout" title="{t}Choose keyboard layout{/t}" size=1>
       {html_options options=$XKbLayouts selected=$gotoXKbLayout_select}
      </select>
{/render}

     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXKbVariant">{t}Variant{/t}</LABEL></td>
     <td>

{render acl=$gotoXKbVariantACL}
      <select id="gotoXKbVariant" name="gotoXKbVariant" title="{t}Choose keyboard variant{/t}" size=1>
       {html_options options=$XKbVariants selected=$gotoXKbVariant_select}
      </select>
{/render}

     </td>
    </tr>
   </table>

  </td>

  <td class='left-border'>

   &nbsp;
  </td>
  
  <td style='width:32%'>

   <h3>{t}Mouse{/t}</h3>
   <table summary="{t}Mouse{/t}">
    <tr>
     <td><LABEL for="gotoXMouseType">{t}Type{/t}</LABEL></td>
     <td>

{render acl=$gotoXMouseTypeACL}
      <select name="gotoXMouseType" id="gotoXMouseType" title="{t}Choose mouse type{/t}" size=1>
       {html_options options=$MouseTypes selected=$gotoXMouseType_select}
      </select>
     </td>
{/render}

    </tr>
    <tr>
     <td><LABEL for="gotoXMouseport">{t}Port{/t}</LABEL></td>
     <td>

{render acl=$gotoXMouseportACL}
      <select id="gotoXMouseport" name="gotoXMouseport" title="{t}Choose mouse port{/t}" size=1>
       {html_options options=$MousePorts selected=$gotoXMouseport_select}
      </select>
{/render}

     </td>
    </tr>
   </table>

  </td>

  <td class='left-border'>

   &nbsp;
  </td>
  
  <td style='width:33%'>

   <h3>{t}Telephone hardware{/t}</h3>
   <table style="width:100%" border=0 summary="{t}Telephone hardware{/t}">
    <tr>
     <td>{t}Telephone{/t}&nbsp;

{render acl=$goFonHardwareACL}
	  {$hardware_list}
{/render}

     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>

<hr>
<table style="width:100%;" summary="{t}Graphic device{/t}">
 <tr>
   <td style='width:33%;'>
   <h3>{t}Graphic device{/t}</h3>
   <table summary="{t}Graphic device{/t}">
    <tr>
     <td><LABEL for="gotoXDriver">{t}Driver{/t}</LABEL></td>
     <td>

{render acl=$gotoXDriverACL}
      <select id="gotoXDriver" name="gotoXDriver" title="{t}Choose graphic driver that is needed by the installed graphic board{/t}" size=1>
       {html_options options=$XDrivers selected=$gotoXDriver_select}
      </select>
{/render}

     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXResolution">{t}Resolution{/t}</LABEL></td>
     <td>

{render acl=$gotoXResolutionACL}
      <select id="gotoXResolution" name="gotoXResolution" title="{t}Choose screen resolution used in graphic mode{/t}" size=1>
       {html_options options=$XResolutions selected=$gotoXResolution_select}
      </select>
{/render}

     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXColordepth">{t}Color depth{/t}</LABEL></td>
     <td>

{render acl=$gotoXColordepthACL}
      <select id="gotoXColordepth" name="gotoXColordepth" title="{t}Choose color depth used in graphic mode{/t}" size=1>
       {html_options options=$XColordepths selected=$gotoXColordepth_select}
      </select>
{/render}

     </td>
    </tr>
   </table>
   </td>

  <td class='left-border'>

   &nbsp;
  </td>

   <td style='width:32%; '>

   <h3>{t}Display device{/t}</h3>
   <table summary="{t}Display device{/t}">
    <tr>
     <td>{t}Type{/t}</td>
     <td>{if $gotoXMonitor==""}{t}unknown{/t}{/if}{$gotoXMonitor}</td>
    </tr>
    <tr>
    	<td>

{render acl=$AutoSyncACL}
	 <input type="checkbox" name="AutoSync" value="1" {$AutoSyncCHK} onChange="changeState('gotoXHsync');changeState('gotoXVsync');">
{/render}

        </td>
	<td>{t}Use DDC for automatic detection{/t}</td>
    </tr>
    <tr>
     <td><LABEL for="gotoXHsync">{t}Horizontal synchronization{/t}</LABEL></td>
     <td>

{render acl=$gotoXHsyncACL}
	<input id="gotoXHsync" name="gotoXHsync" size=10 maxlength=60 {$hiddenState} type='text'
                value="{$gotoXHsync}" title="{t}Horizontal refresh frequency for installed monitor{/t}"> kHz
{/render}

     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoXVsync">{t}Vertical synchronization{/t}</LABEL></td>
     <td>

{render acl=$gotoXVsyncACL}
	<input id="gotoXVsync"  name="gotoXVsync" size=10 maxlength=60 {$hiddenState} type='text'
                value="{$gotoXVsync}" title="{t}Vertical refresh frequency for installed monitor{/t}"> Hz
{/render}

     </td>
    </tr>
   </table>

   </td>
  <td class='left-border'>

   &nbsp;
  </td>

  <td style='width:33%; '>

  

   <h3>{t}Scan device{/t}</h3>

{render acl=$gotoScannerEnableACL}
   <input type=checkbox name="gotoScannerEnable" value="1" title="{t}Select to start SANE scan service on terminal{/t}" {$gotoScannerEnable}>
{/render}

   {t}Provide scan services{/t}
	</td>
 </tr>
</table>

<input type='hidden' name='workservicePosted' value='1'>
<div style="height:40px;"></div>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('gotoXKbModel');
  -->
</script>




