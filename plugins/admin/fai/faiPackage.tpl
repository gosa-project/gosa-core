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
							<LABEL for="section">
							{t}Section{/t}
							</LABEL>
						</td>
						<td>
							<select name="section" title="{t}section{/t}">
								{html_options options=$sections selected=$section}
							</select>
							<input type="submit" value="{t}refresh{/t}" name="refresh1">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="release">
							{t}Release{/t}
							</LABEL>
						</td>
						<td>
							<select name="release" title="{t}release{/t}">
								{html_options options=$releases selected=$release}
							</select>
							<input type="submit" value="{t}refresh{/t}" name="refresh2">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="mirror">
							{t}Mirror{/t}
							</LABEL>
						</td>
						<td>
							<select name="mirror" title="{t}mirror{/t}">
								{html_options options=$mirrors selected=$mirror}
							</select>
							<input type="submit" value="{t}refresh{/t}" name="refresh3">
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
			<input type="submit" name="Addpkg" value="+">
			<input type="submit" name="Delpkg" value="-">
			<input type="submit" name="Conpkg" value="Configure">
		</td>
	</tr>
</table>






