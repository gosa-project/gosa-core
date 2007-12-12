<h2><img alt="P" class="center" src="images/select_printer.png" align="middle">&nbsp;{t}Printer driver{/t}</h2>
<table summary="" width="100%">
	<tr>
		<td width="50%" style="vertical-align:top">
			{t}Model{/t}: <i>{$ppdString}</i>&nbsp;
			<input type="submit" name="SelectPPD" value="{t}Select{/t}">
		</td>
		<td style="border-left: 1px solid rgb(160, 160, 160);padding-left:10px;">
			{t}New driver{/t}&nbsp;
			<input type="file" value="" name="NewPPDFile">
			<input type="submit" name="SubmitNewPPDFile" value="{t}Upload{/t}">
		</td>
	</tr>
</table>
{if $showOptions eq 1}
<p class="seperator">&nbsp;</p>
<h2><img alt="L" class="center" src="images/lamp.png" align="middle">&nbsp;{t}Options{/t}</h2>
{$properties}
{/if}
<p class="plugbottom">
	<input type="hidden" name="PPDDisSubmitted" value="1">
	<input type="submit" name="SavePPD" value="{t}Apply{/t}">
	<input type="submit" name="ClosePPD" value="{t}Cancel{/t}">
</p>
