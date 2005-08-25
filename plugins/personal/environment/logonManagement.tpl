<h2><img alt="" src="images/house.png" align="middle">&nbsp;{t}Logon management{/t}</h2>

    <input type="hidden" name="dialogissubmitted" value="1">

    <table summary="" width="100%">
    	<tr>
			<td width="50%">
					<table>
						<tr>
							<td>{t}Name{/t}
							</td>
							<td>
								<input type="text" value="{$LogonName}" name="LogonName">
							</td>
						</tr>
						<tr>
							<td>{t}Priority{/t}
							</td>
							<td>
				            	<select name="LogonPriority">
                					{html_options values=$LogonPriorityKeys output=$LogonPrioritys}
                				</select>
							</td>
						</tr>
					</table>
			</td>
			<td>
					<table>
						<tr>
							<td>
								<input type="checkbox" value="L" name="LogonLast" {$LogonLastCHK}>
							</td>
							<td>{t}Last proceeded script{/t}
							</td>
						</tr>
						<tr>
							<td>
								<input type="checkbox" value="O" name="LogonOverload" {$LogonOverloadCHK}>
							</td>
							<td>{t}Overloadable{/t}
							</td>
						</tr>
					</table>
			</td>
		</tr>
		<tr>
			<td colspan="2">
					<table width="80%">
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
