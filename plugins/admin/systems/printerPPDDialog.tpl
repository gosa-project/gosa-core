<table summary="" width="100%">
	<tr>
		<td width="50%">
			<h2><img src="images/house.png">&nbsp;{t}Printer driver information file setup{/t}</h2>
			{t}Current used information setup{/t} :
			{$ppdString}<br>
			<input type="submit" name="SelectPPD" value="{t}Select{/t}">
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);padding-left:10px;">
			<h2><img src="images/house.png" alt="">&nbsp;{t}Upload new PPD file{/t} :</h2>
			<input type="file" value="" name="NewPPDFile">
			<input type="submit" name="SubmitNewPPDFile" value="{t}Upload{/t}">
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<br>
<h2><img src="images/house.png">&nbsp;{t}Options{/t}</h2>
{$properties}
<p class="plugbottom">
	<input type="hidden" name="PPDDisSubmitted" value="1">
	<input type="submit" name="SavePPD" value="{t}Apply{/t}">
	<input type="submit" name="ClosePPD" value="{t}Cancel{/t}">
</p>
