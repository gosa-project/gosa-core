<br>
<b>{t}Warning{/t}:</b> {t}Actions you choose here influence all systems in this object group. Additionally, all values editable here can be inherited by the clients assigned to this object group.{/t}
<br>
<br>
<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>
<h2><img class="center" alt="" align="middle" src="images/select_terminal.png"> {t}Generic{/t}</h2>

<table width="100%">
	<tr>
		<td style='width:50%;'>
		<!-- Upper left -->
			   <table width="100%">
				<tr>
				 <td colspan="2" style='padding-top:5px;'><LABEL for="gotoNtpServerSelected">{t}NTP server{/t}</LABEL><br>
{render acl=$gotoNtpServerACL}
				  <select name="gotoNtpServerSelected[]" id="gotoNtpServerSelected" multiple size=5 style="width:100%;"
						title="{t}Choose server to use for synchronizing time{/t}" {if $inheritTimeServer} disabled {/if}>
				   {html_options options=$gotoNtpServer_select}
				  </select>
{/render}
				 <br>
{render acl=$gotoNtpServerACL}
				  <select name="gotoNtpServers" id="gotoNtpServers" {if $inheritTimeServer} disabled {/if} >
				   {html_options output=$gotoNtpServers values=$gotoNtpServers}
				  </select>
{/render}
{render acl=$gotoNtpServerACL}
					<input type="submit" name="addNtpServer" value="{t}Add{/t}"     id="addNtpServer"
					 {if ($inheritTimeServer) || (!$gotoNtpServers)} disabled {/if}>
{/render}

{render acl=$gotoNtpServerACL}
					<input type="submit" name="delNtpServer" value="{t}Delete{/t}"  id="delNtpServer"
					 {if ($inheritTimeServer) || (!$gotoNtpServer_select)} disabled {/if} >
{/render}
				 </td>
				</tr>
			   </table>


		</td>
		<td style="vertical-align:top;border-left:1px solid #A0A0A0;">
		<!-- Upper right -->
				
		 	<table summary="">
				<tr>
				 <td>{t}Mode{/t}</td>
				 <td>
{render acl=$gotoModeACL}
				  <select name="gotoMode" title="{t}Select terminal mode{/t}">
				   {html_options options=$modes selected=$gotoMode_select}
				  </select>
{/render}
				 </td>
				</tr>
				<tr><td colspan="2">&nbsp;</td></tr>
				<tr>
				 <td><LABEL for="gotoSyslogServer">{t}Syslog server{/t}</LABEL></td>
				 <td>
{render acl=$gotoSyslogServerACL}
				  <select id="gotoSyslogServer" name="gotoSyslogServer" title="{t}Choose server to use for logging{/t}">
				   {html_options values=$syslogservers output=$syslogservers selected=$gotoSyslogServer_select}
				  </select>
{/render}
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

		</td>
		<td style="vertical-align:top;border-left:1px solid #A0A0A0;">
		<!-- Bottom right -->			
		</td>
	</tr>
</table>
<input name="workgeneric_posted" value="1" type="hidden">


<h2><img class="center" alt="" align="middle" src="images/rocket.png"> {t}Action{/t}</h2>
				<table summary="">
				 <tr>
				  <td>
{render acl=$FAIstateACL}
				   <select size="1" name="saction" {$FAIstateACL} title="{t}Select action to execute for this terminal{/t}">
					<option>&nbsp;</option>
					{html_options options=$actions}
				   </select>
{/render}
				  </td>
				  <td>
{render acl=$FAIstateACL}
				   <input type=submit name="action" value="{t}Execute{/t}">
{/render}
				  </td>
				 </tr>
				</table>
	




