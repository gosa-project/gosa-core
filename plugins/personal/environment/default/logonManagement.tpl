<div class="desk-script-managment card-content-scroll">
	<h2>{t}Log on script management{/t}</h2>

	<input type="hidden" name="dialogissubmitted" value="1">
	<div class="row">
		<div class="col s12 xl6">
			<div class="input-field">
				<input type="text" name="LogonName" id="LogonName" value="{$LogonName}" {$LogonNameACL}>
				<label for="LogonName">{t}Script name{/t}</label>
			</div>

			<div class="input-field">
				<input type="text" name="LogonDescription" id="LogonDescription" value="{$LogonDescription}" {$LogonNameACL}>
				<label for="LogonDescription">{t}Description{/t}</label>
			</div>

			<div class="input-field">
				<select size="1" id="LogonPriority" name="LogonPriority">
					{html_options values=$LogonPriorityKeys output=$LogonPrioritys selected=$LogonPriority}
				</select>
				<label for="LogonPriority">{t}Priority{/t}</label>
			</div>
		</div>

		<div class="col s12 xl6">
			<div class="script-option">
				<label>
					<input type="checkbox" value="L" name="LogonLast" {$LogonLastCHK} id="LogonLast" />
					<span for="LogonLast">{t}Last script{/t}</span>
				</label>

				<label class="closing-elem">
					<input type="checkbox" value="O" name="LogonOverload" {$LogonOverloadCHK} id="LogonOverload" />
					<span for="LogonOverload">{t}Script can be replaced by user{/t}</span>
				</label>
			</div>
		</div>
	</div>

	<hr class="divider">

	<div class="row">
		<div class="col s12">
			<h3>{t}Script{/t}</h3>

			<textarea style="width:100%;height:400px;" name="LogonData">{$LogonData}</textarea>

			<div class="import-action">
				<div class="file-field input-field">
					<div class="btn-small">
						<span>{t}Browse{/t}</span>
						<input type="file" name="importFile" id="importFile">
					</div>
					<div class="file-path-wrapper">
						<input type="text" class="file-path validate">
					</div>
				</div>
				<button class="btn-small" type='submit' name='StartImport'>{t}Import{/t}</button>
			</div>

			<script language="JavaScript" type="text/javascript">
			<!-- // First input field on page
				focus_field('LogonName');
			-->
			</script>
		</div>
	</div>
</div>

<div class="plugin-actions card-action">
	<button class="btn-small primary" type='submit' name='LogonSave'>{msgPool type=applyButton}</button>
	<button class="btn-small primary" type='submit' name='LogonCancel'>{msgPool type=cancelButton}</button>
</div>
