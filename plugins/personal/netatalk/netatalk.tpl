<table summary="">
<tr>
	<td>
		<label for="apple_user_share">{t}Share{/t}</label>
	</td>
	<td>
		<select name="apple_user_share" id="apple_user_share" {$netatalkShareACL} >
			{html_options options=$shares selected=$selectedshare}
	
			</select>
	</td>
<tr>
	<td>
		<label for="apple_user_homepath_raw">{t}Path{/t}</label>
	</td>
	<td>
		<input name="apple_user_homepath_raw" id="apple_user_homepath_raw" type="text" value="{$apple_user_homepath_raw}" size="25" maxlength="65" {$netatalkUserHomepathACL} />
	</td>
</tr>
</table>

<input type="hidden" name="netatalkTab" value="netatalkTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.apple_user_homeurl_raw.focus();
  -->
</script>
