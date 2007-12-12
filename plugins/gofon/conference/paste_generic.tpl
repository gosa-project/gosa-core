<table summary="">
	<tr>
		<td>
			<LABEL for="cn">
				{t}Conference name{/t}
			</LABEL>
			{$must}
		</td>
		<td>
			<input id="cn" name="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of conference to create{/t}">
		</td>
	</tr>
	<tr>
		<td>
			{t}Phone number{/t}
			{$must}
		</td>
		<td>
			<input name="telephoneNumber" value="{$telephoneNumber}" size=15>
		</td>
	</tr>
</table>

