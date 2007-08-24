<table style='width:100%'>
	<tr>
		<td colspan="2">
			<h2><img alt="" src="images/head.png" class="center" align="middle"> {t}Generic{/t}</h2>
		</td>
	</tr>
	<tr>
		<td style='width:50%;border-right: 1px solid rgb(160, 160, 160);'>
			<table>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
						<input type="text" name="Comment" value="{$Comment}">
					</td>
				</tr>
				<tr>
					<td>
						{t}Object group membership{/t}
					</td>
					<td>
						<select name="OGroup">
							{html_options options=$OGroups selected=$OGroup}
						</select>
					</td>
				</tr>
				{if $Action == "initial_install"}
				<tr>
					<td><input class='center' {if $configure_dns} checked {/if} id='configure_dns'
						type='checkbox' name='configure_dns' value='1' onClick="changeState('Zone');">
						<label for='configure_dns'>{t}Configure DNS{/t}</label>
					</td>
					<td>
						<select name="Zone" id="Zone" {if !$configure_dns} disabled {/if}>
							{html_options options=$Zones selected=$Zone}
						</select>
					</td>
				</tr>
				<tr>
					<td><input class='center' {if $configure_dhcp} checked {/if} id='configure_dhcp' 
							type='checkbox' name='configure_dhcp' value='1' onClick="changeState('Section');">
						<label for='configure_dhcp'>{t}Configure DHCP{/t}</label>
					</td>
					<td>
						<select name="Section" id="Section" {if !$configure_dhcp} disabled {/if}>
							{html_options options=$Sections selected=$Section}
						</select>
					</td>
				</tr>
				{/if}
			</table>
		</td>
		<td style='vertical-align:top'>
			<table>
				<tr>
					<td>
						{t}Action{/t}
					</td>
					<td>
						<select name="Action" onChange="document.mainform.submit();">
							{html_options options=$Actions selected=$Action}
						</select>
						{if !$JS}
							<input type='image' src='images/list_reload.png' class='center'>
						{/if}
					</td>
				</tr>
			</table>

		</td>
	</tr>
</table>
{if $Action == "initial_install"}
<p class='seperator'>&nbsp;</p>
<table style='width:100%;'>
	<tr>
		<td colspan="2" style='padding-top:5px;'>
			<h2><img alt="" src="images/select_workstation.png" class="center" align="middle"> {t}Systems{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
            <table  summary="" style="border:1px solid #B0B0B0;width:100%; " cellspacing=0 cellpadding=0>
                <tr>
                    <td>
						{$Target_list}	
					</td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td style="width:50%; border-right: 1px solid #A0A0A0;">
			<table>
				<tr>
					<td>{t}MAC address{/t}{$must}
					</td>
					<td><input type="text" name="task_MAC" value="">	
					</td>
				</tr>
				<tr>
					<td>{t}Name{/t}
					</td>
					<td><input type="text" name="task_Name" value="">	
					</td>
				</tr>
				<tr>
					<td>{t}IP address{/t}
					</td>
					<td><input type="text" name="task_IP" value="">	
						<input type="submit" name="add_target" value="{t}Add{/t}">
					</td>
				</tr>
			</table>
		</td>
		<td style='vertical-align:top'>
			<table>
				<tr><td>{t}CVS import from file{/t}</td></tr>
				<tr>
					<td>
            &nbsp;
					</td>
				</tr>
				<tr>
					<td>
						<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
			            <input name="import_file" type="file" size="20" maxlength="255" accept="*.csv">
						<input type="submit" name="add_import" value="{t}Import{/t}">
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

{else}
<p class='seperator'>&nbsp;</p>
<table style='width:100%;'>
	<tr>
		<td colspan="2">
			<h2><img alt="" src="images/time.png" class="center" align="middle"> {t}Schedule{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
			{t}Minute{/t}
				<select name='Minute'>
				{html_options options=$Minutes selected=$Minute}
				</select>
			{t}Hour{/t}
				<select name='Hour'>
				{html_options options=$Hours selected=$Hour}
				</select>
			{t}Day{/t}
				<select name='Day'>
				{html_options options=$Days selected=$Day}
				</select>
			{t}Month{/t}
				<select name='Month'>
				{html_options options=$Months selected=$Month}
				</select>
			{t}Week day{/t}
				<select name='Weekday'>
				{html_options options=$Weekdays selected=$Weekday}
				</select>
		</td>
	</tr>
</table>
<p class='seperator'>&nbsp;</p>
<table style='width:100%;'>
	<tr>
		<td colspan="2" style='padding-top:5px;'>
			<h2><img alt="" src="images/select_workstation.png" class="center" align="middle"> {t}Target systems{/t}</h2>
		</td>
	</tr>
	<tr>
		<td colspan="2">
            <table  summary="" style="border:1px solid #B0B0B0;width:600px; " cellspacing=0 cellpadding=0>
                <tr>
                    <td>
						{$Target_list}	
					</td>
				</tr>
			</table>
						<input type="text" name="target_text" value="">
						<input type="submit" name="add_target" value="{t}Add{/t}">
						<input type="submit" name="add_from_list" value="{t}Add from list{/t}">
		</td>
	</tr>
</table>
{/if}
<p class='seperator'>&nbsp;</p>
<input type='hidden' name='goto_task_posted' value='1'>
<p style="text-align:right">
	<input type='submit' name='save_goto_task' value='{t}Ok{/t}'>
{if !$new}
	<input type='submit' name='apply_goto_task' value='{t}Apply{/t}'>
{/if}
	<input type='submit' name='close_goto_task' value='{t}Cancel{/t}'>
</p>
