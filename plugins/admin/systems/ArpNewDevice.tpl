<h2><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Integrating unknown devices{/t}</h2>
<p>
 {t}The current device has been detected by the ARP monitor used by GOsa. You can integrate this device into your running DHCP/DNS infrastructure by submitting this form. The device entry will disapear from the list of the systems and move to the DNS/DHCP configuration.{/t}
</p>
<table summary="" style="width:100%">
<tr>
 <td style>
  <LABEL for="cn">
   {t}DNS name{/t}{$must}
  </LABEL>
 </td>
 <td style='width:35%;border-right:1px solid #aaa'>
  <input name="cn" id="cn" size=18 maxlength=60 value="{$cn}">
 </td>
 <td style='width:15%'>
  <LABEL for="description">
   {t}Description{/t}
  </LABEL>
 </td>
 <td style='width:35%'>
  <input name="description" id="description" size=18 maxlength=60 value="{$description}">
 </td>
</tr>
</table>
<br>
<p class="seperator"></p>
{$netconfig}

<input type='hidden' name='ArpNewDevice_posted' value='1'>

<p class="seperator"></p>
