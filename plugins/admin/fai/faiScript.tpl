<table width="100%" summary="">
	<tr>
		<td width="50%" valign="top">
				<h2><img class="center" alt="" src="images/fai_small.png" align="middle" title="{t}Generic{/t}">&nbsp;{t}Generic{/t}</h2>
				<table summary="" cellspacing="4">
					<tr>
						<td>
							<LABEL for="cn">
							{t}Name{/t}{$must}
							</LABEL>
						</td>
						<td>
{render acl=$cnACL}
							<input value="{$cn}"  maxlength="80" size="45" disabled id="cn">
{/render}
						</td>
					</tr>
					<tr>
						<td>
							<LABEL for="description">
							{t}Description{/t}
							</LABEL>
						</td>
						<td>
{render acl=$descriptionACL}
							<input size="45" maxlength="80" value="{$description}" name="description" id="description" >
{/render}
						</td>
					</tr>
				</table>
		</td>
	</tr>
</table>
<p class="seperator">&nbsp;</p>
<table width="100%" summary="">
	<tr>
		<td>
				<h2><img class="center" alt="" src="images/fai_script.png" align="middle" title="{t}Objects{/t}">&nbsp;
						{t}List of scripts{/t}
				</h2>
				<table width="100%" summary="" style="border:1px solid #B0B0B0; " cellspacing=0 cellpadding=0>
				<tr>
					<td>
						{$Entry_divlist}
{if $sub_object_is_addable}
						<input type="submit" name="AddSubObject"     value="{t}Add{/t}"	title="{t}Add{/t}" >
{else}
						<input type="submit" name="AddSubObject"     value="{t}Add{/t}"	title="{t}Add{/t}" disabled>
{/if}
					</td>
				</tr>
				</table>
		</td>
	</tr>
</table>
<input type="hidden" value="1" name="FAIscript_posted" >
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
<!--
	focus_field("cn","description");
-->
</script>

