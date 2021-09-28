{if !$pg}
  <h3>{t}Open-Xchange account{/t} - {t}disabled, no PostgreSQL support detected or the specified database can't be reached.{/t}</h3>
{else}
  <h3>
<input type="checkbox" name="oxchange" value="B" 
	{$oxchangeState} {$oxchangeAccountACL} 
	onCLick="	
	{if $OXAppointmentDays_W} 
		changeState('OXAppointmentDays');
	{/if}
	{if $OXTaskDays_W} 
		changeState('OXTaskDays');
	{/if}
	{if $OXTimeZone_W} 
		changeState('OXTimeZone'); 
	{/if}
	">
{t}Open-Xchange account{/t}</h3>


<table style='width:100%; ' summary="{t}Open-Xchange configuration{/t}">


 <!-- Headline container -->
 <tr>
   <td style='width:50%; '>

     <table style="margin-left:4px;" summary="{t}Open-Xchange configuration{/t}">
       <tr>
         <td colspan="2">

           <b>{t}Remember{/t}</b>
         </td>
       </tr>
       <tr>
         <td><LABEL for="OXAppointmentDays">{t}Appointment Days{/t}</LABEL></td>
	 <td>

{render acl=$OXAppointmentDaysACL}	
<input type='text' name="OXAppointmentDays" id="OXAppointmentDays" size=7 maxlength=7 value="{$OXAppointmentDays}" {$oxState} >
{/render}
	 {t}days{/t}</td>
       </tr>
       <tr>
         <td><LABEL for="OXTaskDays">{t}Task Days{/t}</LABEL></td>
	 <td>

{render acl=$OXTaskDaysACL}	
<input type='text' name="OXTaskDays" id="OXTaskDays" size=7 maxlength=7 value="{$OXTaskDays}" {$oxState} >
{/render}

	 {t}days{/t}
	</td>
       </tr>
     </table>
   </td>
   <td class='left-border' rowspan="2">

     &nbsp;
   </td>
   <td>

     <table summary="{t}Open-Xchange configuration{/t}">
       <tr>
         <td colspan="2">

           <b>{t}User Information{/t}</b>
         </td>
       </tr>
       <tr>
         <td><LABEL for="OXTimeZone">{t}User Timezone{/t}</LABEL></td>
	 <td>

{render acl=$OXTimeZoneACL}	
<select size="1" name="OXTimeZone" id="OXTimeZone" {$oxState} > 
 {html_options values=$timezones output=$timezones selected=$OXTimeZone}
 </select>
{/render}

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
