<h3>

{if $multiple_support}
	
 <input type="checkbox" name="use_proxy" value="1" onClick="changeState('proxy')" class="center" 
	{if $use_proxy} checked {/if}> 	
 <input type="checkbox" id="proxy" name="proxy" value="B" {$proxyState} class="center"
	{if !$use_proxy} disabled {/if}>

{else}

	{render acl=$proxyAccountACL}
	<input type="checkbox" id="proxy" name="proxy" value="B" {$proxyState}
	class="center" 
	onClick="

	{if $gosaProxyFlagF_W}
	changeState('filterF');
	{/if}

	{if $gosaProxyFlagT_W}
	changeState('filterT'); 
	changeTripleSelectState('proxy', 'filterT', 'startHour'); 
	changeTripleSelectState('proxy', 'filterT', 'startMinute'); 
	changeTripleSelectState('proxy', 'filterT', 'stopMinute'); 
	changeTripleSelectState('proxy', 'filterT', 'stopHour'); 
	{/if}
	{if $gosaProxyFlagB_W}
	changeState('filterB'); 
	changeTripleSelectState('proxy', 'filterB', 'quota_unit'); 
	changeTripleSelectState('proxy', 'filterB', 'quota_size');
	changeTripleSelectState('proxy', 'filterB', 'gosaProxyQuotaPeriod');
	{/if}
	">
	{/render}
{/if}
 {t}Proxy account{/t}</h3>

<table border=0 width="100%" cellpadding=0  summary="{t}Proxy configuration{/t}">
 <tr>
  <td colspan=2>
{render acl=$gosaProxyFlagFACL checkbox=$multiple_support checked=$use_filterF}
    <input type="checkbox" name="filterF" id="filterF" value="F" {$filterF} {$pstate} class="center">
{/render}
    {t}Filter unwanted content (i.e. pornographic or violence related){/t}
  </td>
 </tr>
 <tr>
  <td width="50%">

{render acl=$gosaProxyFlagTACL checkbox=$multiple_support checked=$use_filterT}
    <input type="checkbox" name="filterT" id="filterT" value="T" {$filterT} {$pstate}  onClick="javascript:
 {$ProxyWorkingStateChange}" class="center">
{/render}

    <LABEL for="startHour">{t}Limit proxy access to working time{/t}</LABEL>
    <br>
    <table style="margin-left:20px;"  summary="{t}Worktime restrictions{/t}">
     <tr>
      <td>

{render acl=$gosaProxyFlagTACL}
        <select size="1" id="startHour" name="startHour" {if $Tstate!=""} disabled {/if}  >
         {html_options values=$hours output=$hours selected=$starthour}
        </select>
{/render}
        &nbsp;:&nbsp;
{render acl=$gosaProxyFlagTACL}
        <select size="1" id="startMinute" name="startMinute" {if $Tstate!=""} disabled {/if}  >
         {html_options values=$minutes output=$minutes selected=$startminute}
        </select>
{/render}
        &nbsp;-&nbsp;
{render acl=$gosaProxyFlagTACL}
        <select size="1" id="stopHour" name="stopHour" {if $Tstate!=""} disabled {/if} >
   {html_options values=$hours output=$hours selected=$stophour}
        </select>
{/render}
        &nbsp;:&nbsp;
{render acl=$gosaProxyFlagTACL}
        <select size="1" id="stopMinute" name="stopMinute" {if $Tstate!=""} disabled {/if}>
         {html_options values=$minutes output=$minutes selected=$stopminute}
        </select>
{/render}
      </td>
     </tr>
    </table>


      </td>
      <td>
{render acl=$gosaProxyFlagBACL checkbox=$multiple_support checked=$use_filterB}
    <input type="checkbox" id="filterB" name="filterB" value="B" {$filterB} {if $pstate=="disabled"} disabled {/if} onClick="{$changeB}"
		class="center"
	>
{/render}
    <LABEL for="quota_size">{t}Restrict proxy usage by quota{/t}</LABEL>
    <br>
    <table style="margin-left:20px;"  summary="{t}Quota configuration{/t}">
     <tr>
      <td>
{render acl=$gosaProxyFlagBACL}
       <input type='text' name="quota_size" id="quota_size" size=7 maxlength=10 value="{$quota_size}" {if $Bstate!=""} disabled {/if} >
{/render}
       &nbsp;
{render acl=$gosaProxyFlagBACL}
       <select size="1" name="quota_unit" id="quota_unit" {if $Bstate!=""} disabled {/if} >
	{html_options options=$quota_unit selected=$quota_u}
       </select>
{/render}
    
       <LABEL for="gosaProxyQuotaPeriod">{t}per{/t}</LABEL>
{render acl=$gosaProxyFlagBACL}
       <select size="1" name="gosaProxyQuotaPeriod" id="gosaProxyQuotaPeriod" {if $Bstate!=""} disabled {/if} >
        {html_options options=$quota_time selected=$gosaProxyQuotaPeriod}
       </select>
{/render}
      </td>
     </tr>
    </table>
   </td>
   </tr>
   </table>

