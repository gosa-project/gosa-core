<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of groups{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
      {$groupshead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$groups}
  </div>

  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
     {t}This menu allows you to add, edit or remove selected groups. You may want to use the range selector on top of the group listbox, when working with a large number of groups.{/t}
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
    
<table summary=""  style="width:100%;border-top:1px solid #B0B0B0;"> 
	<tr>
		<td>
    <input type=checkbox name="usergroups" value="1" {$usergroups} onClick="mainform.submit()">{t}Show groups containing users{/t}<br>
    <input type=checkbox name="groupgroups" value="1" {$groupgroups} onClick="mainform.submit()">{t}Show groups containing groups{/t}<br>
    <input type=checkbox name="appgroups" value="1" {$appgroups} onClick="mainform.submit()">{t}Show groups containing applications{/t}<br>
    <input type=checkbox name="depgroups" value="1" {$depgroups} onClick="mainform.submit()">{t}Show groups containing departments{/t}<br>
    <input type=checkbox name="servergroups" value="1" {$servergroups} onClick="mainform.submit()">{t}Show groups containing servers{/t}<br>
    <input type=checkbox name="wsgroups" value="1" {$wsgroups} onClick="mainform.submit()">{t}Show groups containing workstations{/t}<br>
    <input type=checkbox name="tcgroups" value="1" {$tcgroups} onClick="mainform.submit()">{t}Show groups containing terminals{/t}<br>
    <input type=checkbox name="prtgroups" value="1" {$prtgroups} onClick="mainform.submit()">{t}Show groups containing printers{/t}<br>
		</td>
	</tr>
</table>    

    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;"><tr><td><img alt="" src="{$search_image}" align=middle title='{t}Display groups matching{/t}'></td>
    <td><input type='text' name='regex' style='width:99%' maxlength='20' value='{$regex}' title='{t}Regular expression for matching group names{/t}' onChange="mainform.submit()"></td></tr></table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
