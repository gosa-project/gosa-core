<table style='width:100%'>
	<tr>
		<td colspan="2">
			<h2><img alt="" src="images/head.png" class="center" align="middle"> {t}Job details{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style="width:50%;">
			<table>
                <tr><td>{t}Job ID{/t}</td><td>{$ID}</td></tr>
				<tr>
					<td>
						{t}Header Tag{/t}
					</td>
					<td>
						<select name="HeaderTag" onChange="document.mainform.submit();">
							{html_options options=$Actions selected=$HEADERTAG} 
						</select>	
						<input type='image' src="images/list_reload.png"
						 alt="{t}Reload{/t}" title="{t}Reload{/t}" class="center">
					</td>
				</tr>
				<tr>
					<td>&nbsp;
					</td>
				</tr>
				<tr>
					<td style="vertical-align: top;">	
						{t}Schedule Execution{/t}
					</td>
					<td>
						<table cellspacing="0" cellpadding="0">
							<tr>
								<td>{t}Jahr{/t}</td>
								<td>{t}Monat{/t}</td>
								<td>{t}Tag{/t}</td>
								<td>{t}Stunde{/t}</td>
								<td>{t}Minute{/t}</td>
								<td>{t}Sekunde{/t}</td>
							</tr>
							<tr>
								<td>	
									<select name="time_year" onChange="document.mainform.submit();">
										{html_options values=$years options=$years selected=$time_year}
									</select>&nbsp;
								</td>
								<td>	
									<select name="time_month" onChange="document.mainform.submit();">
										{html_options values=$months options=$months selected=$time_month}
									</select>&nbsp;
								</td>
								<td>	
									<select name="time_day">
										{html_options values=$days options=$days selected=$time_day}
									</select>&nbsp;
								</td>
								<td>	
									<select name="time_hour">
										{html_options values=$hours options=$hours selected=$time_hour}
									</select>&nbsp;
								</td>
								<td>	
									<select name="time_minute">
										{html_options values=$minutes options=$minutes selected=$time_minute}
									</select>&nbsp;
								</td>
								<td>	
									<select name="time_second">
										{html_options values=$seconds options=$seconds selected=$time_second}
									</select>
								</td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</td>
		<td>
			<table>
{if $HEADERTAG == "ping"}
				<tr><td>{t}Status{/t}</td><td><input type="text" name="status" value="{$STATUS}"></td></tr>
 	            <tr><td>{t}Mac{/t}</td><td><input type="text" name="macaddress" value="{$MACADDRESS}"></td></tr>
{elseif $HEADERTAG == "sayHello"}
				<tr><td>{t}Status{/t}</td><td><input type="text" name="status" value="{$STATUS}"></td></tr>
 	            <tr><td>{t}Mac{/t}</td><td><input type="text" name="macaddress" value="{$MACADDRESS}"></td></tr>
{else}
				<tr><td>{t}Job type not implented{/t}</td></tr>
{/if}
			</table>
		</td>
	</tr>
</table>
	
<p class='seperator'>&nbsp;</p>
<input type='hidden' name='goto_task_posted' value='1'>
<p style="text-align:right">
	<input type='submit' name='save_goto_task' value='{t}Ok{/t}'>
	<input type='submit' name='close_goto_task' value='{t}Cancel{/t}'>
</p>
