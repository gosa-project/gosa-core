<h2><img src="images/fai_small.png" alt=''>&nbsp;{t}Repository{/t}</h2>

<table width="100%" summary=''>
	<tr>
		<td width="50%" valign="top" style="border-right:1px solid #A0A0A0">
			<table summary=''>
				<tr>
					<td>{t}Parent server{/t}
					</td>
					<td>
						<select name="ParentServer" {$ParentServerACL}>
							{html_options options=$ParentServers values=$ParentServerKeys selected=$ParentServer} 
						</select>
					</td>
				</tr>
				<tr>
					<td>{t}Release{/t}
					</td>
					<td>
						<input type="text" value="{$Release}" name="Release" {$ReleaseACL}>
					</td>
				</tr>
				<tr>
					<td>{t}URL{/t}
					</td>
					<td>
						<input type="text" size="40" value="{$Url}" name="Url" {$UrlACL}>
					</td>
				</tr>
			</table>
		</td>
		<td>
			{t}Sections{/t}<br>
			{$Sections}
			<input type="text" 	name="SectionName" value="" style='width:100%;'>
			<input type="submit" 	name="AddSection"  value="{t}Add{/t}">
		</td>
	</tr>
</table>
<p class="plugbottom">
  <input type=submit name="repository_setup_save" value="{t}Apply{/t}">
  &nbsp;
  <input type=submit name="repository_setup_cancel" value="{t}Cancel{/t}">
</p>

