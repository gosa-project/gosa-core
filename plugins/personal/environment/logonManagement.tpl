<h2><img class="center" alt="" src="images/fai_script.png" align="middle">&nbsp;{t}Logon script management{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

    <table summary="{t}Logon script management{/t}" width="100%">
    	<tr>
			<td width="50%" style="vertical-align:top;border-right:1px solid #B0B0B0">
					<table summary="{t}Logon script settings{/t}">
						<tr>
							<td><LABEL for="LogonName">{t}Skript name{/t}</LABEL>
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
				            	<select name="LogonPriority" id="LogonPriority">
                					{html_options values=$LogonPriorityKeys output=$LogonPrioritys}
                				</select>
							</td>
						</tr>
					</table>
			</td>
			<td style="vertical-align:top">
					<table summary="{t}Logon script flags{/t}">
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
	<p class="seperator">&nbsp;</p>
	<table width="100%" summary="">
		<tr>
			<td colspan="2">
			<h2><img class="center" alt="" src="images/fai_script.png" align="middle">&nbsp;{t}Script{/t}</h2>
					<table width="100%" summary="{t}Logon script{/t}">
						<tr>
							<td>
								<textarea style="width:100%;height:400px;" name="LogonData">{$LogonData}</textarea>
							</td>
						</tr>
						<tr>
							<td>
								<input type="file" name="importFile" id="importFile">
								<input type="submit" value="{t}Import{/t}" name="StartImport" >
							</td>
						</tr>
					</table>
			</td>
		</tr>
	</table>	

	<p class="seperator">&nbsp;</p>
    <p align="right">
    <input type="submit" name="LogonSave"     value="{t}Apply{/t}">
    <input type="submit" name="LogonCancel"   value="{t}Cancel{/t}">
    </p>

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('LogonName');
  -->
</script>

