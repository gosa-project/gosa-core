{ if !$pg }
<h2>{t}Open-Xchange Account{/t} - {t}disabled, no Postgresql support detected{/t}</h2>
{else}
<h2>{if $tabbed eq 1}<input type="checkbox" name="oxchange" value="B" {$oxchangeState} {$oxchangeAccountACL} onCLick="changeState('OXAppointmentDays');changeState('OXTaskDays');changeState('OXTimeZone');" >{/if}{t}Open-Xchange account{/t}</h2>
<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=0 border=0>

 <!-- Headline container -->
 <tr>
   <td style="width:50%; vertical-align:top;">
     <table style="margin-left:4px;">
       <tr>
         <td colspan=2 style="vertical-align:top;">
           <b>{t}Remember{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}Appointment Days{/t}</td>
	 <td><input name="OXAppointmentDays" id="OXAppointmentDays" size=7 maxlength=7 {$OXAppointmentDaysACL} value="{$OXAppointmentDays}" {$fstate}> {t}days{/t}</td>
       </tr>
       <tr>
         <td>{t}Task Days{/t}</td>
	 <td><input name="OXTaskDays" id="OXTaskDays" size=7 maxlength=7 {$OXTaskDaysACL} value="{$OXTaskDays}" {$fstate}> {t}days{/t}</td>
       </tr>
     </table>
   </td>
   <td rowspan=2 style="border-left:1px solid #A0A0A0">
     &nbsp;
   </td>
   <td style="vertical-align:top;">
     <table>
       <tr>
         <td colspan=2 style="vertical-align:top;">
           <b>{t}User Information{/t}</b>
         </td>
       </tr>
       <tr>
         <td>{t}User Timezone{/t}</td>
	 <td><select size="1" name="OXTimeZone" id="OXTimeZone" {$OXTimeZoneACL} {$fstate}> 
	 {html_options values=$timezones output=$timezones selected=$OXTimeZone}
	 </select>
	 </td>
       </tr>
       <tr>
         <td></td>
	 <td></td>
       </tr>
     </table>
   </td>
 </tr>
</table>
{/if}
