<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Create new hotplug entry{/t}</h2>
	
	<input type="hidden" name="dialogissubmitted" value="1">
	
	<table summary="{t}Create new hotplug entry{/t}">
		<tr>
			<td><LABEL for="HOT_name">{t}Name{/t}</LABEL>
			</td>
			<td>	
				<input type="text" value="{$HOT_name}" name="HOT_name" id="HOT_name">
			</td>
		</tr>
		<tr>
			<td><LABEL for="HOT_description">{t}Description{/t}</LABEL>
			</td>
			<td>
				<input type="text" value="{$HOT_description}" name="HOT_description" id="HOT_description">
			</td>
		</tr>
		<tr>
			<td><LABEL for="HOT_id">{t}ID{/t}</LABEL>
			</td>
			<td>
				<input type="text" value="{$HOT_id}" name="HOT_id" id="HOT_id">
			</td>
		</tr>
	</table>
	<p align="right">
	<input type="submit" name="HotPlugSave" 	value="{t}save{/t}">
	<input type="submit" name="HotPlugCancel" 	value="{t}Cancel{/t}">
	</p>
