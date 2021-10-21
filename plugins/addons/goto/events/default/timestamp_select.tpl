<div class="timestamp-wrapper">
    <div class="row">
        <div class="input-field col s2">
            <select name="time_year" onChange="document.mainform.submit();" size=1>
						{html_options values=$years options=$years selected=$time_year}
					</select>
            <label>{t}Year{/t}</label>
        </div>

        <div class="input-field col s2">
            <select name="time_month" onChange="document.mainform.submit();" size=1>
						{html_options values=$months options=$months selected=$time_month}
					</select>
            <label>{t}Month{/t}</label>
        </div>

        <div class="input-field col s2">
            <select name="time_day" size=1>
						{html_options values=$days options=$days selected=$time_day}
					</select>
            <label>{t}Day{/t}</label>
        </div>

        <div class="input-field col s2">
            <select name="time_hour" size=1>
						{html_options values=$hours options=$hours selected=$time_hour}
					</select>
            <label>{t}Hour{/t}</label>
        </div>

        <div class="input-field col s2">
            <select name="time_minute" size=1>
						{html_options values=$minutes options=$minutes selected=$time_minute}
					</select>
            <label>{t}Minute{/t}</label>
        </div>

        <div class="input-field col s2">
            <select name="time_second" size=1>
						{html_options values=$seconds options=$seconds selected=$time_second}
					</select>
            <label>{t}Second{/t}</label>
        </div>
    </div>

	<div class="row">
		<div class="col s12">
			<label>
				<p class="periodical-job">
					<input type="checkbox" name='activate_periodical_job' value='1' {if $activate_periodical_job} checked {/if}
						onClick="changeState('periodValue'); changeState('periodType');">
					<span>{t}Periodical job{/t}</span>
				</p>
			</label>
		</div>

		<div class="input-field col s6">
			<input {if !$activate_periodical_job} disabled {/if} size="4" type='text' id='periodValue' value='{$periodValue}' name='periodValue'>
			<label for="periodValue">{t}Job interval{/t}</label>
		</div>

		<div class="input-field col s6">
			<select name='periodType' id="periodType" {if !$activate_periodical_job} disabled {/if} size=1>
				{html_options options=$periodTypes selected=$periodType}
			</select>
		</div>
	</div>
</div>
