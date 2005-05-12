<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of groups{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
    <p class="contentboxb">
     <select style="width:500px; height:450px;" name="grouplist" size=15 ondblclick="this.form.edit_helper.value='1'; this.form.submit()">
      {$groups}
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
     {t}This menu allows to add, edit or remove selected groups. You may want to use the range selector on top of the group listbox, when working with a large number of groups.{/t}
    </p>
    <p class="contentboxb">
      {t}-Edit- and -New group- will execute an assistant to aid you in editing group properties.-Delete- will ask for confirmation before removing groups.{/t}
    </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;">
     <table style="width:100%;">
      {$alphabet}
     </table>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;">
    <input type=checkbox name="emptygroups" value="1" {$emptygroups} onClick="mainform.submit()">{t}Show empty groups{/t}<br>
    <input type=checkbox name="usergroups" value="1" {$usergroups} onClick="mainform.submit()">{t}Show groups containing users{/t}<br>
    <input type=checkbox name="groupgroups" value="1" {$groupgroups} onClick="mainform.submit()">{t}Show groups containing groups{/t}<br>
    <input type=checkbox name="appgroups" value="1" {$appgroups} onClick="mainform.submit()">{t}Show groups containing applications{/t}<br>
    <input type=checkbox name="depgroups" value="1" {$depgroups} onClick="mainform.submit()">{t}Show groups containing departments{/t}<br>
    <input type=checkbox name="servergroups" value="1" {$servergroups} onClick="mainform.submit()">{t}Show groups containing servers{/t}<br>
    <input type=checkbox name="wsgroups" value="1" {$wsgroups} onClick="mainform.submit()">{t}Show groups containing workstations{/t}<br>
    <input type=checkbox name="tcgroups" value="1" {$tcgroups} onClick="mainform.submit()">{t}Show groups containing terminals{/t}<br>
    <input type=checkbox name="prtgroups" value="1" {$prtgroups} onClick="mainform.submit()">{t}Show groups containing printers{/t}<br>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;">
    <table width="100%"><tr><td width="50%"><img src="{$tree_image}" align=center>&nbsp;{t}Display groups of department{/t}</td>
    <td><select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
      {html_options options=$deplist selected=$depselect}
    </select></td></tr></table>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0">
    <table width="100%"><tr><td width="50%"><img src="{$search_image}" align=center>&nbsp;{t}Display groups matching{/t}</td>
    <td><input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching group names{/t}' onChange="mainform.submit()"></td></tr></table>
    </p>
   {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
