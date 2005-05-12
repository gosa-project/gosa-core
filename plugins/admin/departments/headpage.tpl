<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of departments{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
    <p class="contentboxb">
     <select style="width:500px; height:450px;" name="deplist" size=15 ondblclick="this.form.edit_helper.value='1'; this.form.submit()">
      {html_options options=$departments}
     </select>
    <br>
    <input type=submit value="{t}New{/t}..." name="new_department">&nbsp;
    <input type=submit value="{t}Edit{/t}" name="select_department">&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_department">&nbsp;
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
     {t}This menu allows to create, delete and edit selected departments. Having a large size of departments, you might prefer the range selectors on top of the department list.{/t}
   </p>
   <p class="contentboxb">
     {t}-Edit- and -New department- will provide an assistant to aid you when performing changes on your departments. -Delete- will ask you for confirmation before really deleting anything.{/t}
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
    <table width="100%"><tr><td width="50%"><img src="{$tree_image}" align=center>&nbsp;{t}Display subdepartments of{/t}</td>
    <td><select name="depselect" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
      {html_options options=$deplist selected=$depselect}
    </select></td></tr></table>
    </p>
    <p class="contentboxb" style="border-top:1px solid #B0B0B0">
    <table width="100%"><tr><td width="50%"><img src="{$search_image}" align=center>&nbsp;{t}Display departments matching{/t}</td>
    <td><input type='text' name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching department names{/t}' onChange="mainform.submit()"></td></tr></table>
    </p>
    {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
