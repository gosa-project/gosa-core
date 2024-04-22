<div class="content-wrapper card-content-scroll">
	<div class='default'>
		<h2>
			{t}Create your configuration file{/t}
		</h2>

		<div class="row valign-wrapper">
			<div class="col s10 xl10">
				<p>
					{$msg2}
				</p>
			</div>
			<div class="col s2 xl2">
				<button class="btn-small" type='submit' name='getconf'>{t}Download configuration{/t}</button>
			</div>
		</div>

		<hr class="divider">

		<p>
			{$msg3}
		</p>

		{if $webgroup == ""}
			{t}Depending on the user name your web server is running on:{/t}
			<div class="row valign-wrapper">
				<div class="col s6 xl6">
					<textarea class="copy-area" spellcheck="false" type="text" id="chrights1">sudo chown root:www-data {$CONFIG_DIR}/{$CONFIG_FILE}&#13;&#10;sudo chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}</textarea>
				</div>
				<div class="col s2 xl2">
					<button class="btn-extra-small tooltipped" data-postion="top" data-tooltip="Copy to clipboard" onclick="copy_to_clipboard('chrights1')">
						<i class="tiny material-icons">content_copy</i>
					</button>
				</div>
			</div>
			<p>or</p>
			<div class="row valign-wrapper">
				<div class="col s6 xl6">
					<textarea class="copy-area" spellcheck="false" type="text" id="chrights1">sudo chown root:apache {$CONFIG_DIR}/{$CONFIG_FILE}&#13;&#10;sudo chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}</textarea>
				</div>
				<div class="col s2 xl2">
					<button class="btn-extra-small tooltipped" data-postion="top" data-tooltip="Copy to clipboard" onclick="copy_to_clipboard('chrights1')">
						<i class="tiny material-icons">content_copy</i>
					</button>
				</div>
			</div>
		{else}
			<div class="row valign-wrapper">
				<div class="col s6 xl6">
					<textarea class="copy-area" spellcheck="false" type="text" id="chrights1">sudo chown root:{$webgroup} {$CONFIG_DIR}/{$CONFIG_FILE}&#13;&#10;sudo chmod 640 {$CONFIG_DIR}/{$CONFIG_FILE}</textarea>
				</div>
				<div class="col s2 xl2">
					<button class="btn-extra-small tooltipped" data-postion="top" data-tooltip="Copy to clipboard" onclick="copy_to_clipboard('chrights1')">
						<i class="tiny material-icons">content_copy</i>
					</button>
				</div>
			</div>
		{/if}

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

<script type="text/javascript">
	function copy_to_clipboard(id) {
		var copyText = document.getElementById(id);
		copyText.select();
		document.execCommand('copy')
	}

	document.querySelectorAll("textarea").forEach(element => {
		function autoResize(el) {
			el.style.height = el.scrollHeight + 'px';
		}
		autoResize(element)
	})
</script>
