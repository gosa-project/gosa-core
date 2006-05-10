<h1>{t}Repository settings{/t}</h1>
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
<table summary="">
<tr>
	<td height="30">
		<b><LABEL for="release">{t}Release{/t}</LABEL></b>
	</td>
	<td>
		<select name="FAIdebianReleaseS" title="{t}Release{/t}" onChange='document.mainform.submit();'>
			{if $sections eq ""}
			<option value="">{t}please choose a release...{/t}</option>
			{/if}
			{html_options options=$releases selected=$release}
		</select>
		&nbsp;
		<input type="submit" value="{t}Refresh{/t}" name="SetRepository">
	</td>
</tr>
{if $sections ne ""}
<tr>
	<td style="vertical-align:top">
        <b>{t}Sections for this release{/t}</b>
	</td>
	<td style="vertical-align:top">
		{$sections} 
	</td>
</tr>
{/if}
</table>
<input type="hidden" name="YesItIsPostedPackages" value="1">
<p class="seperator" style="margin-bottom:5px">&nbsp;
</p>

<div align="right" style="align:right;">
    <input name="SaveObjectNew" value="{t}Continue{/t}" type="submit" {if $sections eq ""}disabled{/if}>
    <input name="edit_cancel" value="{t}Cancel{/t}" type="submit">
</div>

