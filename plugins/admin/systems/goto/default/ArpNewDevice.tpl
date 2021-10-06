<h3>{t}Integrating unknown devices{/t}</h3>
<p>
 {t}The current device has been detected by the ARP monitor used by GOsa. You can integrate this device into your running DHCP/DNS infrastructure by submitting this form. The device entry will disappear from the list of the systems and move to the DNS/DHCP configuration.{/t}
</p>
<table summary="{t}Integrating unknown devices{/t}" style="width:100%">
<tr>
 <td style>
  <LABEL for="cn">
   {t}DNS name{/t}{$must}
  </LABEL>
 </td>
 <td style='width:35%;' class='right-border'>

  <input type='text' name="cn" id="cn" size=18 maxlength=60 value="{$cn}">
 </td>
 <td style='width:15%'>
  <LABEL for="description">
   {t}Description{/t}
  </LABEL>
 </td>
 <td style='width:35%'>
  <input type='text' name="description" id="description" size=18 maxlength=60 value="{$description}">
 </td>
</tr>
</table>
<br>
<hr>
{$netconfig}

<hr>

<!--<h3>{t}GOto{/t}</h3>-->
<p>
<input type='checkbox' value='1' name='gotoIntegration'
    onChange="changeState('SystemType');changeState('ObjectGroup');"
    {if $gotoIntegration} checked {/if}>&nbsp;{t}GOto integration{/t}
</p>
<table summary="{t}Target type selection{/t}" style='width:100%'>
 <tr>
  <td style='width:49%'>
      {t}System type{/t}&nbsp;
	  <select {if !$gotoIntegration} disabled {/if}
      id="SystemType"
      name="SystemType" title="{t}System type{/t}" style="width:120px;"
			onChange="document.mainform.submit();">
       {html_options values=$SystemTypeKeys output=$SystemTypes selected=$SystemType}
      </select>
  </td>
  <td>
      {t}Choose an object group as template{/t}&nbsp;
	    <select {if !$gotoIntegration} disabled {/if}
        id="ObjectGroup"
        name="ObjectGroup" title="{t}Object group{/t}" style="width:120px;">
		  <option value='none'>{t}none{/t}</option>	
       {html_options options=$ogroups selected=$ObjectGroup}
      </select>
  </td>
 </tr>
</table>
<input type='hidden' name='ArpNewDevice_posted' value='1'>

<hr>
