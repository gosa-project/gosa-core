<input type="hidden" name="SubObjectFormSubmitted" value="1">

<h2><img alt="" src="images/house.png" valign="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
<table width="100%" summary="">
<tr>
	<td width="50%">
		{t}Name{/t}{$must}&nbsp;
		<input value="{$Object_cn}" name="Object_cn" size="13">&nbsp;&nbsp;
	</td>
	<td>
		{t}Description{/t}&nbsp;
		<input size="35" value="{$Object_description}" name="Object_description">
	</td>
</tr>
</table>

<p class="seperator">&nbsp;</p>

<table width="100%" summary="">
<tr>
  <td colspan=2><h2><img alt="" src="images/fai_template.png" valign="middle" title="{t}Template attributes{/t}">&nbsp;{t}Template attributes{/t}</h2></td>
</tr>
<tr>
  <td style="vertical-align:top;width:50%;border-right:1px solid #B0B0B0">
   <table>
    <tr>
		<td style="vertical-align:top">
			<LABEL for="Object_FAItemplateFile">
				{t}File{/t}{$must}&nbsp;
			</LABEL>
			</td>
		<td style="vertical-align:top">
			{$status}
			{if $Object_FAItemplatePath}
			  <a href="getbin.php?rand={$rand}"><img alt="{t}Save template{/t}..." title="{t}Save template{/t}..." src="images/save.png" border="0" align="center" /></a>
			{/if}
			<br>
			<br>
			<input type="file" name="FAItemplateFile" value="{$Object_FAItemplateFile}" id="Object_FAItemplateFile">
			&nbsp;<input type="submit" value="{t}Upload{/t}" name="TmpFileUpload">
			<br>
			<br>
			</td>
	</tr><tr>
			<td>
				<LABEL for="Object_FAItemplatePath">
					{t}Destination path{/t}{$must}&nbsp;
				</LABEL>
				</td>
			<td>
				<input type="text" name="Object_FAItemplatePath" value="{$Object_FAItemplatePath}" id="Object_FAItemplatePath" size="45">
				</td>
		</tr>
		</table>
	</td>
	<td>
	  <table>
		<tr>
	
		<td>
			<LABEL for="Object_user">
				{t}Owner{/t}{$must}&nbsp;
			</LABEL>
			</td>
		<td>
			<input type="text" name="Object_user" value="{$Object_user}" id="Object_user" size="15">
			</td>
	</tr><tr>
		<td style="vertical-align:top">
			<LABEL for="Object_group">
				{t}Group{/t}{$must}&nbsp;
			</LABEL>
			</td>
		<td>
			<input type="text" name="Object_group" value="{$Object_group}" id="Object_group" size="15">
			<br>
			<br>
			</td>
	</tr><tr>
		<td style="vertical-align:top">{t}Access{/t}{$must}&nbsp; </td>
		<td>
	  <table style="border:1px solid #B0B0B0">
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
			<td align="center"><input type="checkbox" name="u4" value="4" {$u4}></td>
			<td align="center"><input type="checkbox" name="u2" value="2" {$u2}></td>
			<td align="center"><input type="checkbox" name="u1" value="1"> {$u1}</td>
			<td>&nbsp;</td>
			<td align="center"><input type="checkbox" name="s4" value="4" {$s4}></td>
			<td>({t}SUID{/t})</td>
			</tr>

		<tr><td>{t}Group{/t}</td>
			<td align="center"><input type="checkbox" name="g4" value="4" {$g4}></td>
			<td align="center"><input type="checkbox" name="g2" value="2" {$g2}></td>
			<td align="center"><input type="checkbox" name="g1" value="1" {$g1}></td>
			<td>&nbsp;</td>
			<td align="center"><input type="checkbox" name="s2" value="2" {$s2}></td>
			<td>({t}GUID{/t})</td>
			</tr>

		<tr><td>{t}Others{/t}</td>
			<td align="center"><input type="checkbox" name="o4" value="4" {$o4}></td>
			<td align="center"><input type="checkbox" name="o2" value="2" {$o2}></td>
			<td align="center"><input type="checkbox" name="o1" value="1" {$o1}></td>
			<td>&nbsp;</td>
			<td align="center"><input type="checkbox" name="s1" value="1" {$s1}></td>
			<td>({t}sticky{/t})</td>
		</tr>
		</td>
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
	<input type="submit" value="{t}Save{/t}" 	name="SaveSubObject">&nbsp;
	<input type="submit" value="{t}Cancel{/t}" 	name="CancelSubObject">
</div>
</td>
</tr>
</table>
