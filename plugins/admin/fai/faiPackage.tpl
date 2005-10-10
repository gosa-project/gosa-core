<table width="100%">
	<tr>
		<td width="50%" valign="top">
				<h2><img alt="" src="images/house.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$cn}" disabled id="cn">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="description">
							{t}Description{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$description}" {$description} name="description" id="description">
						</td>
					</tr>
				</table>
		</td>
		<td width="50%" valign="top">
			<h2><img alt="" src="images/house.png" align="middle" title="{t}Repository{/t}">&nbsp;{t}Repository{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							{t}Release{/t} : 
						</td>
						<td>
							{$release}
						</td>
					</tr>
					<tr>
						<td>
							{t}Section{/t} : 
						</td>
						<td>
							{$section}
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="mirror">
							{t}Mirror{/t}
							</LABEL>
						</td>
						<td>
							<select name="FAIdebianMirrorS" title="{t}mirror{/t}">
								{html_options options=$mirrors selected=$mirror}
							</select>
							<input type="submit" value="{t}set{/t}" name="SetMirror">
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<table width="99%">
	<tr>
		<td> 
			<h2><img alt="" src="images/house.png" align="middle" title="{t}Used packages{/t}">&nbsp;{t}Used packages{/t}</h2>
			<br>
			<select id="usedPackages" name="usedPackages" title="{t}Choosen packages{/t}" multiple style="width:100%;height:300px;">
       			{html_options options=$usedPackages}
      		</select>
			<br>
			<input type="submit" name="Addpkg" value="+" {$OptionsACL}>
			<input type="submit" name="Delpkg" value="-"  {$OptionsACL}>
			<input type="submit" name="Conpkg" value="Configure"  {$OptionsACL}>
		</td>
	</tr>
</table>
