<h2><img alt="" src="images/fai_partitionTable.png" align="middle">&nbsp;{t}Edit share{/t}</h2>
<table summary="{t}NFS setup{/t}" width="100%">
	<tr>
		<td width="45%">
		<!--Table left-top-->
			<table >
				<tr>
					<td>
						{t}Name{/t} {$must}
					</td>
					<td>
						<input type="text" name="name" value="{$name}" {$nameACL}>
					</td>
				</tr>
				<tr>
					<td>
						{t}Description{/t}
					</td>
					<td>
						<input type="text" size="40" name="description" value="{$description}">
					</td>
				</tr>
				<tr>
					<td>
						{t}Path{/t}
					</td>
					<td>
						<input type="text" size="40" name="path" value="{$path}">
					</td>
				</tr>
			</table>
		</td>
		<td style="border-left:1px solid #A0A0A0">

		<!--Table right-top-->
			<table>
				<tr>
					<td>
						{t}Type{/t}
					</td>
					<td>
						<select size="1" name="type">
   							{html_options options=$types selected=$type}
						</select>	
					</td>
				</tr>
				<tr>
					<td>
						{t}Charset{/t}
					</td>
					<td>
						<select size="1" name="charset">
   							{html_options options=$charsets selected=$charset}
						</select>	
					</td>
				</tr>
				<tr>
				    <td>
					{t}Option{/t}
				    </td>
				    <td>
								<input type="text" name="option" value="{$option}">
				    </td>
				</tr>
			</table>
		</td>
	</tr>
	<tr>
		<td colspan=2>
			<p class="seperator">&nbsp;</p>
		</td>
	</tr>
	<tr>
		<td colspan=2 style="text-align:right">
			<input type="submit" name="NFSsave" value="{t}Save{/t}">
			<input type="submit" name="NFScancel" value="{t}Cancel{/t}">
		</td>
	</tr>
</table>
