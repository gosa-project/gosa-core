<div class="netatalk-wrapper">
    <div class="row">
        <div class="col s12 xl6">
          	<h3>{t}Netatalk configuration{/t}</h3>

			{render acl=$netatalkShareACL}
				<div class="input-field">
					<select id="apple_user_share" size="1" name="apple_user_share">
						{html_options options=$shares selected=$selectedshare}
					</select>
					<label for="apple_user_share">{t}Share{/t}</label>
				</div>
			{/render}

            {render acl=$netatalkUserHomepathACL}
				<div class="input-field">
					<input type='text' id="apple_user_homepath_raw" name="apple_user_homepath_raw" maxlength=65 value="{$apple_user_homepath_raw}">
					<label for="apple_user_homepath_raw">{t}Path{/t}</label>
				</div>
            {/render}
		</div>
	</div>
</div>

<input type="hidden" name="netatalkTab" value="netatalkTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  	 // First input field on page
	focus_field('apple_user_homeurl_raw');
</script>
