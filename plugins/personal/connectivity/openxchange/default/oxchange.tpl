<div class="oxchange-wrapper">
  <div class="row">
    {if !$pg}
      <h3>{t}Open-Xchange account{/t} - {t}disabled, no PostgreSQL support detected or the specified database can't be reached.{/t}</h3>
    {else}
    
    <label>
      <input type="checkbox" name="oxchange" value="B" {$oxchangeState} {$oxchangeAccountACL} onCLick=
        "{if $OXAppointmentDays_W} 
          changeState('OXAppointmentDays');
        {/if}
        {if $OXTaskDays_W} 
          changeState('OXTaskDays');
        {/if}
        {if $OXTimeZone_W} 
          changeState('OXTimeZone'); 
        {/if}
        ">
        <span><h2>{t}Open-Xchange account{/t}</h2></span>
    </label>

    <div class="col s12 xl6">
      <h3>{t}Remember{/t}</h3>

      {render acl=$OXAppointmentDaysACL}	
      <div class="input-field">
        <input type='text' name="OXAppointmentDays" id="OXAppointmentDays" size=7 maxlength=7 value="{$OXAppointmentDays}" {$oxState} >
        <label for="OXAppointmentDays">{t}Appointment Days{/t} ({t}days{/t})</label>
      </div>
      {/render}

      {render acl=$OXTaskDaysACL}	
      <div class="input-field">
        <input type='text' name="OXTaskDays" id="OXTaskDays" size=7 maxlength=7 value="{$OXTaskDays}" {$oxState} >
        <label for="OXTaskDays">{t}Task Days{/t} ({t}days{/t})</label>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <h3>{t}User Information{/t}</h3>

      {render acl=$OXTimeZoneACL}	
      <div class="input-field">
        <select size="1" name="OXTimeZone" id="OXTimeZone" {$oxState} > 
          {html_options values=$timezones output=$timezones selected=$OXTimeZone}
        </select>
        <label>{t}User Timezone{/t}</label>
      </div>
      {/render}
      
    </div>
  </div>
</div>
{/if}
