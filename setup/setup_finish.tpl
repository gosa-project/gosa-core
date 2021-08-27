<div class="content-wrapper card-content-scroll">
	<div class='default'>
		<h2>
			{t}Create your configuration file{/t}
		</h2>
		<p>
			{$msg2}
		</p>

		{if $webgroup == ""}
			{t}Depending on the user name your web server is running on:{/t}
			<pre>
				chown root:www-data {$CONFIG_DIR}/{$CONFIG_FILE}
				chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}

				or

				chown root:apache {$CONFIG_DIR}/{$CONFIG_FILE}
				chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}
			</pre>
		{else}
			<pre>
			chown root:{$webgroup} {$CONFIG_DIR}/{$CONFIG_FILE}
			chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}
			</pre>
		{/if}

		<button class="btn-small" type='submit' name='getconf'>{t}Download configuration{/t}</button>

		{if $err_msg != ""}
            <hr class="divider">

            <div class="status-wrapper">
                <div class="row">
                    <div class="col s12 connection-setting">
                        <p class="label">{t}Status: {/t}</p>
                        <div class="connection-status red-text">{$err_msg}</div>
                    </div>
                </div>
            </div>
		{/if}
	</div>

	<input type='hidden' value='1' name='step8_posted'>
</div>
