 <table cellspacing="0" cellpadding="0" summary="{t}Event scheduling options{/t}">
	<tr>
		<td>{t}Year{/t}</td>
		<td>{t}Month{/t}</td>
		<td>{t}Day{/t}</td>
    <td>&nbsp;&nbsp;</td>
		<td>{t}Hour{/t}</td>
		<td>{t}Minute{/t}</td>
		<td>{t}Second{/t}</td>
	</tr>
	<tr>
		<td>
			<select name="time_year" onChange="document.mainform.submit();" size=1>
				{html_options values=$years options=$years selected=$time_year}
			</select>&nbsp;
		</td>
		<td>
			<select name="time_month" onChange="document.mainform.submit();" size=1>
				{html_options values=$months options=$months selected=$time_month}
			</select>&nbsp;
		</td>
		<td>
			<select name="time_day" size=1>
				{html_options values=$days options=$days selected=$time_day}
			</select>&nbsp;
		</td>
    <td>&nbsp;</td>
		<td>
			<select name="time_hour" size=1>
				{html_options values=$hours options=$hours selected=$time_hour}
			</select>&nbsp;
		</td>
		<td>
			<select name="time_minute" size=1>
				{html_options values=$minutes options=$minutes selected=$time_minute}
			</select>&nbsp;
		</td>
		<td>
			<select name="time_second" size=1>
				{html_options values=$seconds options=$seconds selected=$time_second}
			</select>
		</td>
	</tr >
</table>
<br>
<table width="100%" summary="{t}Periodical jobs{/t}">
  <tr>
    <td colspan="2">
      <b>{t}Periodical job{/t}</b>
      <input class='center' type="checkbox" name='activate_periodical_job' value='1' {if $activate_periodical_job} checked {/if}
        onClick="changeState('periodValue'); changeState('periodType');">
    </td>
	</tr>
  <tr>
    <td>{t}Job interval{/t}</td>
    <td>
      <input {if !$activate_periodical_job} disabled {/if}
          size="4" type='text' id='periodValue' value='{$periodValue}' name='periodValue'>
      <select name='periodType' id="periodType" {if !$activate_periodical_job} disabled {/if} size=1>
        {html_options options=$periodTypes selected=$periodType}
      </select>
    </td>
	</tr>
</table>

