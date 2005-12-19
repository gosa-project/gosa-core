<h2>
		<input type="checkbox" id="proxy" name="proxy" value="B" {$proxyState} {$proxyAccountACL} 
			onClick="
				{$changeA}
				
				changeTripleSelectState('proxy', 'filterB', 'quota_size'); 
				changeTripleSelectState('proxy', 'filterB', 'quota_unit'); 
				changeTripleSelectState('proxy', 'filterB', 'gosaProxyQuotaPeriod');
			">
	{t}Proxy account{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
   <td>
    <table summary="" border=0 width="100%" cellpadding=0>
    <tr><td colspan=2>
    <input type="checkbox" name="filterF" id="filterF" value="F" {$filterF} {$gosaProxyAcctFlagsACL} {$pstate}>
    {t}Filter unwanted content (i.e. pornographic or violence related){/t}
    </td></tr>
     <tr>
      <td width="50%">
    <input type="checkbox" name="filterT" id="filterT" value="T" {$filterT} {$Working_allowedACL} {$pstate}
    	onClick="{$ProxyWorkingStateChange}">
    <LABEL for="startHour">{t}Limit proxy access to working time{/t}</LABEL>
    <br>
    <table summary="" style="margin-left:20px;">
     <tr>
      <td>
        <select size="1" id="startHour" name="startHour"
			{if $gosaProxyWorkingStartACL!="" || $Tstate!=""}
				disabled 	
			{/if}>
         {html_options values=$hours output=$hours selected=$starthour}
        </select>
        &nbsp;:&nbsp;
        <select size="1" id="startMinute" name="startMinute"
			{if $gosaProxyWorkingStartACL!="" || $Tstate!=""}
				disabled 	
			{/if}>
         {html_options values=$minutes output=$minutes selected=$startminute}
        </select>
        &nbsp;-&nbsp;
        <select size="1" id="stopHour" name="stopHour"
			{if $gosaProxyWorkingStopACL!="" || $Tstate!=""}
				disabled 	
			{/if}>
			{html_options values=$hours output=$hours selected=$stophour}
        </select>
        &nbsp;:&nbsp;
        <select size="1" id="stopMinute" name="stopMinute" 
			{if $gosaProxyWorkingStopACL!="" || $Tstate!=""}
				disabled 	
			{/if}>
         {html_options values=$minutes output=$minutes selected=$stopminute}
        </select>
      </td>
     </tr>
    </table>
      </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
     &nbsp;
   </td>
      <td>
    <input type="checkbox" id="filterB" name="filterB" value="B" {$filterB} 
		{if $pstate=="disabled" || $gosaProxyAcctFlagsACL=="disabled" || $gosaProxyQuotaACL=="disabled"}
			disabled 
		{/if} 
    	onClick="{$changeB}">
    <LABEL for="quota_size">{t}Restrict proxy usage by quota{/t}</LABEL>
    <br>
    <table summary="" style="margin-left:20px;">
     <tr>
      <td>
       <input name="quota_size" id="quota_size" size=7 maxlength=10 value="{$quota_size}"
		{if $Bstate=="disabled" || $gosaProxyQuotaACL=="disabled"}
			disabled 
		{/if}>
       &nbsp;
       <select size="1" name="quota_unit" id="quota_unit"
		{if $Bstate=="disabled" || $gosaProxyQuotaACL=="disabled"}
            disabled
        {/if}>
		{html_options options=$quota_unit selected=$quota_u}
       </select>
	   
       <LABEL for="gosaProxyQuotaPeriod">{t}per{/t}</LABEL>
       <select size="1" name="gosaProxyQuotaPeriod" id="gosaProxyQuotaPeriod"
		{if $Bstate=="disabled" || $gosaProxyQuotaACL=="disabled"}
            disabled
        {/if}>
        {html_options options=$quota_time selected=$gosaProxyQuotaPeriod}
       </select>
      </td>
     </tr>
    </table>
   </td>
   </tr>
   </table>
  </td>
 </tr>
</table>

