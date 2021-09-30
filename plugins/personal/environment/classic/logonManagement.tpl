<h3>{t}Log on script management{/t}</h3>

    <input type="hidden" name="dialogissubmitted" value="1">

    <table summary="{t}Log on script management{/t}" width="100%">
    	<tr>
			<td class='right-border'>

					<table summary="{t}Log on script settings{/t}">
						<tr>
							<td><LABEL for="LogonName">{t}Script name{/t}</LABEL>
							</td>
							<td>
								<input type="text" size=20 value="{$LogonName}" name="LogonName" {$LogonNameACL} id="LogonName">
							</td>
						</tr>
						<tr>
							<td><LABEL for="LogonDescription">{t}Description{/t}</LABEL>
							</td>
							<td>
								<input type="text" size=40 value="{$LogonDescription}" name="LogonDescription" id="LogonDescription"> 
							</td>
						</tr>
						<tr>
							<td><LABEL for="LogonPriority">{t}Priority{/t}</LABEL>
							</td><td>
				            	<select name="LogonPriority" id="LogonPriority" size=1>
                					{html_options values=$LogonPriorityKeys output=$LogonPrioritys selected=$LogonPriority}
                				</select>
							</td>
						</tr>
					</table>
			</td>
			<td>

					<table summary="{t}Log on script flags{/t}">
						<tr>
							<td>
								<input type="checkbox" value="L" name="LogonLast" {$LogonLastCHK} id="LogonLast">
							<LABEL for="LogonLast">{t}Last script{/t}</LABEL>
							</td>
						</tr>
						<tr>
							<td>
								<input type="checkbox" value="O" name="LogonOverload" {$LogonOverloadCHK} id="LogonOverload">
								<LABEL for="LogonOverload">{t}Script can be replaced by user{/t}</LABEL>
							</td>
						</tr>
					</table>
			</td>
		</tr>
	</table>
	
  <hr>

	<h3>{t}Script{/t}</h3>
	<table width="100%" summary="{t}Log on script{/t}">
		<tr>
			<td>
				<textarea style="width:100%;height:400px;" name="LogonData">{$LogonData}</textarea>
			</td>
		</tr>
		<tr>
			<td>
				<input type="file" name="importFile" id="importFile">
				<button type='submit' name='StartImport'>{t}Import{/t}</button>
			</td>
		</tr>
	</table>

  <hr>
  <div class="plugin-actions">
    <button type='submit' name='LogonSave'>{msgPool type=applyButton}</button>
    <button type='submit' name='LogonCancel'>{msgPool type=cancelButton}</button>
  </div>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('LogonName');
  -->
</script>

