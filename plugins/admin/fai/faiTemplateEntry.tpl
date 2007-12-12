<input type="hidden" name="SubObjectFormSubmitted" value="1">

<h2><img class="middle" alt="" src="images/forward.png" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
<table width="100%" summary="">
<tr>
	<td width="50%">
		<table>
		  <tr>
		    <td>
		{t}File name{/t}{$must}&nbsp;
		    </td>
		    <td>
{render acl=$cnACL}
		<input value="{$cn}" name="cn" size="45" maxlength="80">&nbsp;&nbsp;
{/render}
		    </td>
		   </tr>
		  <tr>
			<td>
				<LABEL for="templatePath">
					{t}Destination path{/t}{$must}&nbsp;
				</LABEL>
				</td>
			<td>
{render acl=$FAItemplatePathACL}
				<input type="text" name="FAItemplatePath" value="{$templatePath}" id="templatePath" size="45">
{/render}
				</td>
		  </tr>
		</table>
	</td>
	<td>
		{t}Description{/t}&nbsp;
{render acl=$descriptionACL}
		<input size="45" maxlength="80" value="{$description}" name="description">
{/render}
	</td>
</tr>
</table>

<p class="seperator">&nbsp;</p>

<table width="100%" summary="">
<tr>
  <td colspan=2><h2><img class="middle" alt="" src="images/fai_template.png" title="{t}Template attributes{/t}">&nbsp;{t}Template attributes{/t}</h2></td>
</tr>
<tr>
  <td style="vertical-align:top;width:50%;border-right:1px solid #B0B0B0">
   <table summary="">
    <tr>
		<td style="vertical-align:top">
			<LABEL for="FAItemplateFile">
				{t}File{/t}{$must}&nbsp;
			</LABEL>
			</td>
		<td style="vertical-align:top" class="center">
			{$status}
			{if $bStatus}
			  <a href="getbin.php?rand={$rand}">
				<img class="center" alt="{t}Save template{/t}..." title="{t}Save template{/t}..." src="images/save.png" border="0" />
			  </a>
			{/if}
			<br>
			<br>
{render acl=$FAItemplateFileACL}
			<input type="file" name="FAItemplateFile" value="{$FAItemplateFile}" id="FAItemplateFile">
{/render}
{render acl=$FAItemplateFileACL}
			&nbsp;<input type="submit" value="{t}Upload{/t}" name="TmpFileUpload">
{/render}
			<br>
			<br>
			</td>
	</tr><tr>
			<td>
				{t}Full path{/t}&nbsp;
				</td>
			<td>
{render acl=$FAItemplatePathACL}
				<i>{$FAItemplatePath}</i>
{/render}
				</td>
		</tr>
		</table>
	</td>
	<td>
	  <table summary="">
		<tr>
	
		<td>
			<LABEL for="user">
				{t}Owner{/t}{$must}&nbsp;
			</LABEL>
			</td>
		<td>
{render acl=$FAIownerACL}
			<input type="text" name="user" value="{$user}" id="user" size="15">
{/render}
		</td>
	</tr><tr>
		<td style="vertical-align:top">
			<LABEL for="group">
				{t}Group{/t}{$must}&nbsp;
			</LABEL>
			</td>
		<td>
{render acl=$FAIownerACL}
			<input type="text" name="group" value="{$group}" id="group" size="15">
{/render}
			<br>
			<br>
		</td>
	</tr><tr>
		<td style="vertical-align:top">{t}Access{/t}{$must}&nbsp; </td>
		<td>
	  <table summary="" style="border:1px solid #B0B0B0">
	       <colgroup width="55" span="3">
	        </colgroup>
		<tr>
			<th>{t}Class{/t}</th>
			<th>{t}Read{/t}</th>
			<th>{t}Write{/t}</th>
			<th>{t}Execute{/t}</th>
			<th>&nbsp;</th>
			<th>{t}Special{/t}</th>
			<th>&nbsp;</th>
		</tr>
		<tr><td>{t}User{/t}</td>
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="u4" value="4" {$u4}></td>
{/render}
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="u2" value="2" {$u2}></td>
{/render}
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="u1" value="1" {$u1}></td>
{/render}
			<td>&nbsp;</td>
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="s4" value="4" {$s4}></td>
{/render}
			<td>({t}SUID{/t})</td>
			</tr>

		<tr><td>{t}Group{/t}</td>
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="g4" value="4" {$g4}></td>
{/render}
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="g2" value="2" {$g2}></td>
{/render}
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="g1" value="1" {$g1}></td>
{/render}
			<td>&nbsp;</td>
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="s2" value="2" {$s2}></td>
{/render}
			<td>({t}SGID{/t})</td>
			</tr>

		<tr><td>{t}Others{/t}</td>
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="o4" value="4" {$o4}></td>
{/render}
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="o2" value="2" {$o2}></td>
{/render}
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="o1" value="1" {$o1}></td>
{/render}
			<td>&nbsp;</td>
{render acl=$FAImodeACL}
			<td align="center"><input type="checkbox" name="s1" value="1" {$s1}></td>
{/render}
			<td>({t}sticky{/t})</td>
	</tr></table>
	
		</td></tr></table>
	</td>
</tr>
<tr>
<td colspan=2>
<br>
<p class="seperator">&nbsp;</p>
<br>
<div style="align:right;" align="right">
	<input type="submit" value="{t}Apply{/t}" 	name="SaveSubObject">&nbsp;
	<input type="submit" value="{t}Cancel{/t}" 	name="CancelSubObject">
</div>
</td>
</tr>
</table>

<input type='hidden' name='FAItemplateEntryPosted' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn','description');
  -->
</script>

