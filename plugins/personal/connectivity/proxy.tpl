<h2>
 <input type="checkbox" id="proxy" name="proxy" value="B" {$proxyState} {$proxyAccountACL} 
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
 {t}Proxy account{/t}</h2>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>
 <tr>
   <td>
    <table summary="" border=0 width="100%" cellpadding=0>
    <tr><td colspan=2>

{render acl=$gosaProxyFlagFACL}
    <input type="checkbox" name="filterF" id="filterF" value="F" {$filterF} {$pstate} >
{/render}
    {t}Filter unwanted content (i.e. pornographic or violence related){/t}
    </td></tr>
     <tr>
      <td width="50%">

{render acl=$gosaProxyFlagTACL}
    <input type="checkbox" name="filterT" id="filterT" value="T" {$filterT} {$pstate}  onClick="javascript:
 {$ProxyWorkingStateChange}">
{/render}

    <LABEL for="startHour">{t}Limit proxy access to working time{/t}</LABEL>
    <br>
    <table summary="" style="margin-left:20px;">
     <tr>
      <td>

{render acl=$gosaProxyFlagTACL}
        <select size="1" id="startHour" name="startHour" {if $Tstate!="" || $pstate!=""} disabled {/if}  >
         {html_options values=$hours output=$hours selected=$starthour}
        </select>
{/render}
        &nbsp;:&nbsp;
{render acl=$gosaProxyFlagTACL}
        <select size="1" id="startMinute" name="startMinute" {if $Tstate!="" || $pstate!=""} disabled {/if}  >
         {html_options values=$minutes output=$minutes selected=$startminute}
        </select>
{/render}
        &nbsp;-&nbsp;
{render acl=$gosaProxyFlagTACL}
        <select size="1" id="stopHour" name="stopHour" {if $Tstate!="" || $pstate!=""} disabled {/if} >
   {html_options values=$hours output=$hours selected=$stophour}
        </select>
{/render}
        &nbsp;:&nbsp;
{render acl=$gosaProxyFlagTACL}
        <select size="1" id="stopMinute" name="stopMinute" {if $Tstate!="" || $pstate!=""} disabled {/if}>
         {html_options values=$minutes output=$minutes selected=$stopminute}
        </select>
{/render}
      </td>
     </tr>
    </table>
      </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
     &nbsp;
   </td>
      <td>
{render acl=$gosaProxyFlagBACL}
    <input type="checkbox" id="filterB" name="filterB" value="B" {$filterB} {if $pstate=="disabled"} disabled {/if} onClick="{$changeB}">
{/render}
    <LABEL for="quota_size">{t}Restrict proxy usage by quota{/t}</LABEL>
    <br>
    <table summary="" style="margin-left:20px;">
     <tr>
      <td>
{render acl=$gosaProxyFlagBACL}
       <input name="quota_size" id="quota_size" size=7 maxlength=10 value="{$quota_size}" {if $Tstate!="" || $pstate!=""} disabled {/if} >
{/render}
       &nbsp;
{render acl=$gosaProxyFlagBACL}
       <select size="1" name="quota_unit" id="quota_unit" {if $Tstate!="" || $pstate!=""} disabled {/if} >
	{html_options options=$quota_unit selected=$quota_u}
       </select>
{/render}
    
       <LABEL for="gosaProxyQuotaPeriod">{t}per{/t}</LABEL>
{render acl=$gosaProxyFlagBACL}
       <select size="1" name="gosaProxyQuotaPeriod" id="gosaProxyQuotaPeriod" {if $Tstate!="" || $pstate!=""} disabled {/if} >
        {html_options options=$quota_time selected=$gosaProxyQuotaPeriod}
       </select>
{/render}
      </td>
     </tr>
    </table>
   </td>
   </tr>
   </table>
  </td>
 </tr>
</table>

