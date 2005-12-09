<div style="font-size: 18px;">
    {t}Repository settings{/t}
</div>
<br>
<p class="seperator">
{t}To add packages to your package list you have to setup the repository settings first.{/t}
<br>
{t}Packages are usually stored on different servers called mirrors. These mirrors have different types of releases (for example woody/sarge/etch), which finally contain packages sorted by section.{/t}
<br>
<br>
{t}Section and release can't be changed in future dialogs, but the mirror can be changed by editing the entry.{/t}
<br>
<br>
</p>
<br>
<p class="seperator"><b>
{t}First select the preferred release, then the section and finally the mirror.{/t}</b>
<br>
<br>
</p>
<table>
<tr>
	<td height="30">
		<h2>
			<LABEL for="release">{t}Following releases are available{/t}</LABEL>: 
		</h2>
	</td>
	<td>
		<select name="FAIdebianReleaseS" title="{t}Release{/t}">
			{html_options options=$releases values=$releaseKeys selected=$release}
		</select>
	</td>
</tr>
<tr>
	<td height="30">
        <h2>{t}Sections for this release{/t}: </h2> 
		
	</td>
	<td>
		{$sections} 
		<input type="submit" value="{t}set{/t}" name="SetRepository">
	</td>
</tr>
</table>
<p class="seperator">&nbsp;
</p>

<div align="right" style="align:right;">
    <input name="SaveObjectNew" value="{t}Continue{/t}" type="submit" {$goon}>
    <input name="edit_cancel" value="{t}Cancel{/t}" type="submit">
</div>

