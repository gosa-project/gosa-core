<div style="font-size: 18px;">
    {t}Repository settings{/t}
</div>
<br>
<p class="seperator">
{t}To add packages to your package list you have to setup the repository settings first.{/t}
<br>
{t}Packages may be stored on different servers called mirrors. These mirrors have different types of releases (for example woody/sarge), which finaly contain packages sorted by section.{/t}
<br>
<br>
{t}Section and release can't be changed in future dialogs, but the mirror can be changed by editing the entry.{/t}
<br>
<br>
</p>
<br>
<p class="seperator"><b>
{t}First select the prefered release then the section and finally the mirror.{/t}</b>
<br>
<br>
</p>
<table>
<tr>
	<td height="30">
		<h2>
			<LABEL for="release">{t}Following releases are available{/t}</LABEL> : 
		</h2>
	</td>
	<td>
		<select name="FAIdebianReleaseS" title="{t}Release{/t}" {$ReleaseACL}>
			{html_options options=$releases selected=$release}
		</select>
	</td>
</tr>
{if $DisRelease neq false}
<tr>
	<td height="30">
        <h2>{t}Sections for this release{/t} : </h2> 
		
	</td>
	<td>
		{$sections} 
	</td>
</tr>
{/if}
{if $DisSection neq false}
<tr>
	<td height="30">
		<h2><LABEL for="mirror">{t}Matching mirrors{/t}</LABEL> : </h2>
	</td>
	<td>
		<select name="FAIdebianMirrorS" title="{t}mirror{/t}" {$MirrorACL}>
			{html_options options=$mirrors selected=$mirror}
		</select>
	</td>
</tr>
{/if}
</table>
{if $ContinueACL neq ""}
<input type="submit" value="{t}set{/t}" name="SetRepository">
{/if}
<p class="seperator">&nbsp;
</p>

<div align="right" style="align:right;">
    <input name="SaveObjectNew" value="{t}Continue{/t}" type="submit" {$ContinueACL}>
    <input name="edit_cancel" value="{t}Cancel{/t}" type="submit">
</div>
<input type="hidden" name="ignore">

