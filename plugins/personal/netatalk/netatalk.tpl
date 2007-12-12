<table summary="">
<tr>
	<td>
		<label for="apple_user_share">{t}Share{/t}</label>
	</td>
	<td>

{render acl=$netatalkShareACL}
		<select name="apple_user_share" id="apple_user_share">
			{html_options options=$shares selected=$selectedshare}
		</select>
{/render}
	</td>
<tr>
	<td>
		<label for="apple_user_homepath_raw">{t}Path{/t}</label>
	</td>
	<td>
{render acl=$netatalkUserHomepathACL}
		<input name="apple_user_homepath_raw" id="apple_user_homepath_raw" type="text" value="{$apple_user_homepath_raw}" size="25" maxlength="65"/>
{/render}
	</td>
</tr>
</table>

<input type="hidden" name="netatalkTab" value="netatalkTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('apple_user_homeurl_raw');
  -->
</script>
