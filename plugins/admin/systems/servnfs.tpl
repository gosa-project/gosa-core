<h2><img alt="" src="images/select_phone.png" align="middle">&nbsp;Freigabe bearbeiten</h2>
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
						<input type="text" name="description" value="{$description}">
					</td>
				</tr>
			</table>
		</td>
		<td>

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
			</table>
		</td>
	</tr>
	<tr>
		<td colspan=2>
			<p class="seperator">&nbsp;</p>
		</td>
	</tr>
	<tr>
		<td>
		<!--Table bottom-->
			<table>
				<tr>
					<td>
						{t}Path{/t}
					</td>
					<td>
						<input type="text" name="path" value="{$path}">
					</td>
				</tr>
			</table>
		</td>
		<td>
			<table>
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
</table>
<input type="submit" name="NFSsave" value="{t}Save{/t}">
<input type="submit" name="NFScancel" value="{t}Cancel{/t}">
