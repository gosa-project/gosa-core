<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Logon management{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

    <table summary="{t}Logon script management{/t}" width="100%">
    	<tr>
			<td width="50%">
					<table summary="{t}Logon script settings{/t}">
						<tr>
							<td><LABEL for="LogonName">{t}Name{/t}</LABEL>
							</td>
							<td>
								<input type="text" value="{$LogonName}" name="LogonName" {$LogonNameACL} id="LogonName">
							</td>
						</tr>
						<tr>
							<td><LABEL for="LogonDescription">{t}Description{/t}</LABEL>
							</td>
							<td>
								<input type="text" value="{$LogonDescription}" name="LogonDescription" id="LogonDescription"> 
							</td>
						</tr>
						<tr>
							<td><LABEL for="LogonPriority">{t}Priority{/t}</LABEL>
							</td>
							<td>
				            	<select name="LogonPriority" id="LogonPriority">
                					{html_options values=$LogonPriorityKeys output=$LogonPrioritys}
                				</select>
							</td>
						</tr>
					</table>
			</td>
			<td>
					<table summary="{t}Logon script flags{/t}">
						<tr>
							<td>
								<input type="checkbox" value="L" name="LogonLast" {$LogonLastCHK} id="LogonLast">
							</td>
							<td><LABEL for="LogonLast">{t}Last proceeded script{/t}</LABEL>
							</td>
						</tr>
						<tr>
							<td>
								<input type="checkbox" value="O" name="LogonOverload" {$LogonOverloadCHK} id="LogonOverload">
							</td>
							<td>
								<LABEL for="LogonOverload">{t}Overloadable{/t}</LABEL>
							</td>
						</tr>
						<tr>
							<td>
								<LABEL for="importFile">{t}Import{/t}</LABEL>
							</td>
							<td>
								<input type="file" name="importFile" id="importFile">
								<input type="submit" value="{t}Import{/t}" name="StartImport" >
							</td>
						</tr>
					</table>
			</td>
		</tr>
		<tr>
			<td colspan="2">
					<table width="80%" summary="{t}Logon script{/t}">
						<tr>
							<td>
								<textarea style="width:100%;height:400px;" name="LogonData">{$LogonData}</textarea>
							</td>
						</tr>
					</table>
			</td>
		</tr>
	<table>	

    <p align="right">
    <input type="submit" name="LogonSave"     value="{t}Use{/t}">
    <input type="submit" name="LogonCancel"   value="{t}Cancel{/t}">
    </p>
