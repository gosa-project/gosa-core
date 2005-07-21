<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     <LABEL for="grouplist">{t}List of groups{/t}</LABEL> {$hint}
    </p>
  </div>
  <div class="contentboxb">
    <p class="contentboxb">
     <select id="grouplist" style="width:500px; height:450px;" name="grouplist" size=15 ondblclick="this.form.edit_helper.value='1'; this.form.submit()">
      {html_options options=$grouplist}
		<option disabled>&nbsp;</option>
    </select>
    <br>
    <input type=submit value="{t}New{/t}..." name="new_group">&nbsp;
    <input type=submit value="{t}Edit{/t}" name="select_group">&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_group">&nbsp;
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
     {t}This menu allows you to add, edit and remove selected groups. You may want to use the range selector on top of the group listbox, when working with a large number of groups.{/t}
    </p>
    <p class="contentboxb">
      {t}-Edit- and -New...- will execute an assistant to aid you in editing group properties. -Delete- will ask for confirmation before removing groups.{/t}
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
	<input type=checkbox id="primarygroups" name="primarygroups" value="1" {$primarygroups} onClick="mainform.submit()" 
			title="{t}Select to see groups that are primary groups of users{/t}">
		<LABEL for="primarygroups">{t}Show primary groups{/t}</LABEL>
		<br>
    <input type=checkbox id="sambagroups" name="sambagroups" value="1" {$sambagroups} onClick="mainform.submit()" 
		title="{t}Select to see groups that have samba groups mappings{/t}">
		<LABEL for="sambagroups">{t}Show samba groups{/t}</LABEL>
		<br>
    <input type=checkbox id="appgroups" name="appgroups" value="1" {$appgroups} onClick="mainform.submit()" 
		title="{t}Select to see groups that have applications configured{/t}">
		<LABEL for="appgroups">{t}Show application groups{/t}</LABEL>
		<br>
    <input type=checkbox id="mailgroups" name="mailgroups" value="1" {$mailgroups} onClick="mainform.submit()" 
		title="{t}Select to see groups that have mail settings{/t}">
		<LABEL for="mailgroups">{t}Show mail groups{/t}</LABEL>
		<br>
    <input type=checkbox id="functionalgroups" name="functionalgroups" value="1" {$functionalgroups} onClick="mainform.submit()" 
		title='{t}Select to see normal groups that have only functional aspects{/t}'>
		<LABEL for="functionalgroups">{t}Show functional groups{/t}</LABEL>
		<br>
            </td>
        </tr>
    </table>

    
    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
        <tr>
            <td>
		<input type=checkbox name="subsearch" value="1" {$subsearch} onClick="mainform.submit()" title="{t}Dive into LDAP subtrees when searching{/t}">{t}Search in subtrees{/t}<br>
			</td>
		</tr>
	</table>
	
    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td width="50%">
				<img alt="" src="{$tree_image}" align=middle>&nbsp;
				<LABEL for="depselect">{t}Display groups of department{/t}</LABEL>
			</td>
    		<td>
				<select id="depselect" name="depselect" size=1 onChange="mainform.submit()" 
						title="{t}Choose the department the search will be based on{/t}">
      				{html_options options=$deplist selected=$depselect}
    			</select>
			</td>
		</tr>
	</table>
    <table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td width="50%"><img alt="" src="{$search_image}" align=middle>&nbsp;
				<LABEL for="regex">{t}Display groups matching{/t}</LABEL>
			</td>
    		<td>
				<input type='text' id="regex" name='regex' maxlength='20' value='{$regex}' 
					title='{t}Regular expression for matching group names{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td width="50%"><img alt="" src="{$search_image}" align=middle>&nbsp;
				<LABEL for="guser">{t}Display groups of user{/t}</LABEL>
			</td>
    		<td>
				<input id="guser" type='text' name='guser' maxlength='20' value='{$guser}' 
					title='{t}User name of which groups are shown{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
