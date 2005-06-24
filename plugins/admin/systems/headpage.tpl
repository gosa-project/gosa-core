<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of systems{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
    <p class="contentboxb">
     <select style="width:500px; height:450px;" name="termlist" size=15 ondblclick="this.form.edit_helper.value='1'; this.form.submit()">
      {$terminals}
    </select>
    <br>
    <input type=submit value="{t}New{/t}..." name="new_terminal">&nbsp;
    <input type=submit value="{t}Edit{/t}" name="select_terminal">&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_terminal">&nbsp;
    <input type=submit value="{t}Set password{/t}" name="set_password">&nbsp;
    <input type=hidden name="edit_helper">
   </p>
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
    {t}This menu allows you to add, remove and change the properties of specific systems. You can only add systems which have already been started once.{/t}
   </p>
   <p>
    {t}-Edit- will execute an assistant to aid you in changing the system properties. -Delete- will ask for confirmation before deleting systems.{/t}
   </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td>
	<input type=checkbox name="servers" value="1" {$servers} onClick="mainform.submit()" title="{t}Select to see servers{/t}">{t}Show servers{/t}<br>
    <input type=checkbox name="thins" value="1" {$thins} onClick="mainform.submit()" title="{t}Select to see Linux terminals{/t}">{t}Show terminals{/t}<br>
    <input type=checkbox name="workstations" value="1" {$workstations} onClick="mainform.submit()" title="{t}Select to see Linux workstations{/t}">{t}Show workstations{/t}<br>
    <input type=checkbox name="winstations" value="1" {$winstations} onClick="mainform.submit()" title="{t}Select to see MicroSoft Windows based workstations{/t}">{t}Show windows based workstations{/t}<br>
    <input type=checkbox name="printers" value="1" {$printers} onClick="mainform.submit()" title="{t}Select to see network printers{/t}">{t}Show network printers{/t}<br>
    <input type=checkbox name="phones" value="1" {$phones} onClick="mainform.submit()" title="{t}Select to see VOIP phones{/t}">{t}Show phones{/t}<br>
    <input type=checkbox name="netdev" value="1" {$netdev} onClick="mainform.submit()" title="{t}Select to see network devices{/t}">{t}Show network devices{/t}<br>
		</td>
	</tr>
</table>    
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width="50%"><img alt="" src="{$tree_image}" align=middle>
			&nbsp;{t}Display systems of department{/t}
		</td>
	    <td>
			<select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
			      {html_options options=$deplist selected=$depselect}
		    </select>
		</td>
	</tr>
</table>
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width="50%"><img alt="" src="{$search_image}" align=middle>
			&nbsp;{t}Display systems matching{/t}
		</td>
	    <td>
			<input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching system names{/t}' onChange="mainform.submit()">
		</td>
	</tr>
</table>
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width="50%">
			<img alt="" src="{$search_image}" align=middle>
				&nbsp;{t}Display terminal(s) of user{/t}
			</td>
		    <td>
				<input type='text' name='user' maxlength='20' value='{$user}' title='{t}User name of which terminal(s) are shown{/t}' onChange="mainform.submit()">
			</td>
	</tr>
</table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
