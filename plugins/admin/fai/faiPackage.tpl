<table width="100%" summary="">
	<tr>
		<td width="50%" valign="top" style="border-right:1px solid #A0A0A0">
				<h2><img class="center" alt="" src="images/forward.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$cn}" size="45" disabled id="cn">
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="description">
							{t}Description{/t}
							</LABEL>
						</td>
						<td>
							<input value="{$description}" size="45" {$descriptionACL} name="description" id="description">
						</td>
					</tr>
				</table>
		</td>
		<td width="50%" valign="top">
			<h2><img class="center" alt="" src="images/network.png" align="middle" title="{t}Repository{/t}">&nbsp;{t}Repository{/t}</h2>
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
						  {t}Install method{/t} :
						</td>
						<td>
							<select name="FAIinstallMethod" {$cnACL} >
							  {html_options options=$FAIinstallMethods output=$FAIinstallMethod selected=$FAIinstallMethod}
							</select>
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<table width="99%" summary="">
	<tr>
		<td> 
			<h2><img class="center" alt="" src="images/fai_packages.png" align="middle" title="{t}Used packages{/t}">&nbsp;{t}Used packages{/t}</h2>
			<br>
			<select id="usedPackages" name="usedPackages[]" title="{t}Choosen packages{/t}" multiple style="width:100%;height:300px;">
       			{html_options options=$usedPackages}
      		</select>
			<br>
			<input type="submit" name="Addpkg" value="{t}Add{/t}" {$OptionsACL} {$cnACL}>
			<input type="submit" name="Delpkg" value="{t}Remove{/t}"  {$OptionsACL} {$cnACL}>
			<input type="submit" name="Conpkg" value="{t}Configure{/t}"  {$OptionsACL} >
			<input type="submit" name="Markpkg" value="{t}Toggle remove flag{/t}"  {$OptionsACL} {$cnACL}>
		</td>
	</tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.description.focus();
  -->
</script>

