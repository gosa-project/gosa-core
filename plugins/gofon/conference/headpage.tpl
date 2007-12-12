<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:600px">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of conference rooms{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
      {$conferencehead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$conferences}
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
     {t}This menu allows you to create, delete and edit selected phone conferences. Having a large number of phone conferences, you might prefer the range selectors on top of the conferences list.{/t}
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
    <table summary="" width="100%" style="border-top:1px solid #B0B0B0;"><tr><td><LABEL for='filter'><img class="center" alt="Search" src="{$search_image}" align="middle" title="{t}Display users matching{/t}"></LABEL></td>
    <td width="99%"><input id='filter' style='width:99%' type='text' name='regexit' maxlength='20' value='{$regex}' title='{t}Regular expression for        matching user names{/t}' onChange="mainform.submit()"></td></tr></table>
   {$apply}
   </div>


  </td>
</tr>
</table summary="">

<input type="hidden" name="ignore">
