<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
     <b>{t}Select groups to add{/t} {$hint}</b>
    </p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
     <select style="width:500px; height:450px; margin-top:4px;" name="groups[]" size=15" multiple>
	{html_options options=$groups}
     </select>
     <input type=hidden name="edit_helper">
    </p>
   </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px"><img src="{$launchimage}" align="right" alt="[F]"><b>{t}Filters{/t}</b></p>
   </div>
   <div class="contentboxb" style="background-color:#F8F8F8">
    <p class="contentboxb" style="border-top:1px solid #B0B0B0; background-color:#F8F8F8">
     <table style="width:100%;">
      {$alphabet}
     </table>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0; background-color:#F8F8F8">
    <input type=checkbox name="primarygroups" value="1" {$primarygroups} onClick="mainform.submit()" title="{t}Select to see groups that are primary groups of users{/t}">{t}Show primary groups{/t}<br>
    <input type=checkbox name="sambagroups" value="1" {$sambagroups} onClick="mainform.submit()" title="{t}Select to see groups that have samba groups mappings{/t}">{t}Show samba groups{/t}<br>
    <input type=checkbox name="appgroups" value="1" {$appgroups} onClick="mainform.submit()" title="{t}Select to see groups that have applications configured{/t}">{t}Show application groups{/t}<br>
    <input type=checkbox name="mailgroups" value="1" {$mailgroups} onClick="mainform.submit()" title="{t}Select to see groups that have mail settings{/t}">{t}Show mail groups{/t}<br>
    <input type=checkbox name="functionalgroups" value="1" {$functionalgroups} onClick="mainform.submit()" title="{t}Select to see normal groups that have only functional aspects{/t}">{t}Show functional groups{/t}
 </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
    <table width="100%"><tr><td width="50%"><img alt="" src="{$tree_image}" align=middle>&nbsp;{t}Display groups of department{/t}</td>
    <td><select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
      {html_options options=$deplist selected=$depselect}
    </select></td></tr></table>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
    <table width="100%"><tr><td width="50%"><img alt="" src="{$search_image}" align=middle>&nbsp;{t}Display groups matching{/t}</td>
    <td><input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching group names{/t}' onChange="mainform.submit()"></td></tr></table>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0;background-color:#F8F8F8">
    <table width="100%"><tr><td width="50%"><img alt="" src="{$search_image}" align=middle>&nbsp;{t}Display groups of user{/t}</td>
    <td><input type='text' name='guser' maxlength='20' value='{$guser}' title='{t}User name of which groups are shown{/t}' onChange="mainform.submit()"></td></tr></table>
   </p>
   {$apply}
   </div>
  </td>
</tr>
</table>

<p class="plugbottom">
  <input type=submit name="add_groups_finish" value="{t}Add{/t}">
  &nbsp;
  <input type=submit name="add_groups_cancel" value="{t}Cancel{/t}">
</p>

