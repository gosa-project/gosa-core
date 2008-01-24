<table style='width:100%'>
	<tr>
		<td colspan="2">
			<h2><img alt="" src="images/head.png" class="center" align="middle"> {t}Generic{/t}</h2>
		</td>
	</tr>
	<tr>
		<td>
			<table>
				<tr>
					<td>
						{t}Job ID{/t}
					</td>
					<td>	
						{$ID}
					</td>	
				</tr>
				<tr>
					<td>
						{t}Status{/t}
					</td>
					<td>	
						<input type="text" name="status" value="{$STATUS}">
					</td>	
				</tr>
 	            <tr>
                    <td>
                        {t}Mac{/t}
                    </td>
                    <td>
                        <input type="text" name="macaddress" value="{$MACADDRESS}">
                    </td>
                </tr>
				<tr>
					<td>
						{t}Header Tag{/t}
					</td>
					<td>
						<select name="HeaderTag">
							<option value="unknown">{t}Unknown{/t}</option>
							{html_options options=$Actions selected=$HEADERTAG} 
						</select>	
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

<input type="submit" name="test" value="Test submit()">
	
<p class='seperator'>&nbsp;</p>
<input type='hidden' name='goto_task_posted' value='1'>
<p style="text-align:right">
	<input type='submit' name='save_goto_task' value='{t}Ok{/t}'>
	<input type='submit' name='close_goto_task' value='{t}Cancel{/t}'>
</p>
