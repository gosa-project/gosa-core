
<h2>{t}Printer driver information file setup{/t}</h2>

<table summary="" width="50%">
	<tr>
		<td>
			{t}Current used information setup : {/t}
		</td>
		<td>
			{$ppdString}
		</td>
		<td>
			<input type="submit" name="SelectPPD" value="{t}Select{/t}">
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<br>
{$properties}
<p class="seperator">&nbsp;</p>
<br>
<input type="submit" name="SavePPD" value="{t}Save{/t}">
<input type="submit" name="ClosePPD" value="{t}Close{/t}">
