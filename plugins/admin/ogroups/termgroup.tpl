{t}All actions you choose here influence <b>all</b> machines in this object group. Be aware that rebooting machines may not make people happy who are currently working at these machines.{/t}
<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>
<h2><img class="center" alt="" align="middle" src="images/rocket.png"> {t}Action{/t}</h2>

<table width="100%">
	<tr>
		<td style='width:50%;'>
		<!-- Upper left -->
		 	<table summary="">
				<tr>
				 <td>{t}Mode{/t}</td>
				 <td>
				  <select name="gotoMode" title="{t}Select terminal mode{/t}" {$gotoModeACL}>
				   {html_options options=$modes selected=$gotoMode_select}
				  </select>
				 </td>
				</tr>
			   </table>
		</td>
		<td style="vertical-align:top;border-left:1px solid #A0A0A0;">
		<!-- Upper right -->
				
				<table summary="">
				 <tr>
				  <td>
				   <select size="1" name="saction" {$actionACL} title="{t}Select action to execute for this terminal{/t}">
					<option>&nbsp;</option>
					{html_options options=$actions}
				   </select>
				  </td>
				  <td>
				   <input type=submit name="action" value="{t}Execute{/t}">
				  </td>
				 </tr>
				</table>
	
		</td>
	</tr>
	<tr>
		<td colspan=2>
			<p class="seperator">&nbsp;</p>
		</td>
	</tr>
	<tr>
		<td>
		<!-- Bottom left -->			

			   <table width="100%">
				<tr>
				 <td colspan="2">
					<input type="checkbox" value="1" name="inheritTimeServer"
						{if $inheritTimeServer } checked {/if}
						onClick="javascript:
								changeState('gotoNtpServerSelected');
								changeState('gotoNtpServers');
								changeState('addNtpServer');
								changeState('delNtpServer');">{t}Inherit time server attributes{/t}
				 </td>
				</tr>
				<tr>
				 <td colspan="2" style='padding-left:14px;padding-top:5px;'><LABEL for="gotoNtpServerSelected">{t}NTP server{/t}</LABEL><br>
				  <select name="gotoNtpServerSelected[]" id="gotoNtpServerSelected" multiple size=5 style="width:100%;"
						title="{t}Choose server to use for synchronizing time{/t}" {$gotoNtpServerACL} {if $inheritTimeServer} disabled {/if}>
				   {html_options options=$gotoNtpServer_select}
				  </select>
				 <br>
				  <select name="gotoNtpServers" id="gotoNtpServers" {$gotoNtpServerACL} {if $inheritTimeServer} disabled {/if} >
				   {html_options output=$gotoNtpServers values=$gotoNtpServers}
				  </select>
					<input type="submit" name="addNtpServer" value="{t}Add{/t}"     id="addNtpServer" {$gotoNtpServerACL}
					 {if $inheritTimeServer} disabled {/if}>
					<input type="submit" name="delNtpServer" value="{t}Delete{/t}"  id="delNtpServer" {$gotoNtpServerACL}
					 {if $inheritTimeServer} disabled {/if}>
				 </td>
				</tr>
			   </table>


		</td>
		<td style="vertical-align:top;border-left:1px solid #A0A0A0;">
		<!-- Bottom right -->			
				<table>
				<tr>
				 <td><LABEL for="gotoSyslogServer">{t}Syslog server{/t}</LABEL></td>
				 <td>
				  <select id="gotoSyslogServer" name="gotoSyslogServer" title="{t}Choose server to use for logging{/t}" {$gotoSyslogServerACL}>
				   {html_options values=$syslogservers output=$syslogservers selected=$gotoSyslogServer_select}
				  </select>
				 </td>
				</tr>
				</table>
		</td>
	</tr>
</table>
<input name="workgeneric_posted" value="1" type="hidden">






