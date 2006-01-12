<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding='4'>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     <label for="grouplist">{t}List of groups{/t}</label> {$hint}
    </p>
  </div>
  <div class="contentboxb">
      {$grouplisthead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$grouplist}
    <input type=hidden name="edit_helper">
  </div>

  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
     {t}This menu allows you to add, edit and remove selected groups. You may want to use the range selector on top of the group listbox, when working with a large number of groups.{/t}
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
	<input type="checkbox" id="primarygroups" name="primarygroups" value="1" {$primarygroups} onClick="mainform.submit()" 
			title="{t}Select to see groups that are primary groups of users{/t}">
		<label for="primarygroups">{t}Show primary groups{/t}</label>
		<br />
    <input type='checkbox' id="sambagroups" name="sambagroups" value="1" {$sambagroups} onClick="mainform.submit()" 
		title="{t}Select to see groups that have samba groups mappings{/t}">
		<label for="sambagroups">{t}Show samba groups{/t}</label>
		<br />
    <input type='checkbox' id="appgroups" name="appgroups" value="1" {$appgroups} onClick="mainform.submit()" 
		title="{t}Select to see groups that have applications configured{/t}">
		<label for="appgroups">{t}Show application groups{/t}</label>
		<br />
    <input type='checkbox' id="mailgroups" name="mailgroups" value="1" {$mailgroups} onClick="mainform.submit()" 
		title="{t}Select to see groups that have mail settings{/t}">
		<label for="mailgroups">{t}Show mail groups{/t}</label>
		<br />
    <input type='checkbox' id="functionalgroups" name="functionalgroups" value="1" {$functionalgroups} onClick="mainform.submit()" 
		title='{t}Select to see normal groups that have only functional aspects{/t}'>
		<label for="functionalgroups">{t}Show functional groups{/t}</label>
		<br />
            </td>
        </tr>
    </table>

    <table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td><label for="regex"><img alt="{t}Display groups matching{/t}" src="{$search_image}" align="left"></label></td>
    			<td style='width:99%'>
				<input type='text' id="regex" name='regex' maxlength='20' style="width:99%;" value='{$regex}' title='{t}Regular expression for matching group names{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
	<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td><label for="guser"><img alt="{t}Display groups of user{/t}" src="{$searchu_image}" align="left"></label></td>
    			<td style='width:99%'>
				<input id="guser" type='text' name='guser' maxlength='20' style="width:99%;" value='{$guser}' 
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
