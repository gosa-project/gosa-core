<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:50%;" >
  <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px;">
     {t}List of devices{/t}
    </p>
  </div>
  <div class="contentboxb">
      {$devicehead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$devices}
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="border-bottom:1px solid #B0B0B0;height:20px;padding:0px;">
    <p class="contentboxh" style="font-size:12px;padding:0px;"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb" style="padding:5px;">
    {t}This dialog allows you to attach a device to your currently edited computer.{/t}
   </div>
   <br>
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px;"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
<tr>
<td><LABEL for="regex"><img alt="{t}Display devices matching{/t}" src="{$search_image}" align=middle></label></td>
<td width="99%">
<input type='text' style='width:99%' name='device_regex' maxlength='20' value='{$device_regex}' title='{t}Regular expression for matching device names{/t}' onChange="mainform.submit()">
</td>
</tr>
</table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
<p class="seperator">&nbsp;</p>
<div align="right">
	<p>
		<input type="submit" name="SelectDeviceSave" value="{t}Use{/t}">
		<input type="submit" name="SelectDeviceCancel" value="{t}Cancel{/t}">
	</p>
</div>
