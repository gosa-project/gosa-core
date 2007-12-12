<script src="TreeMenu.js" language="JavaScript" type="text/javascript"></script>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:600px">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}DFS Shares{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
      {$dfshead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$tree}
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
     {t}This menu allows you to create, delete and edit selected dfs shares. Having a large numbers of dfs shares, you might prefer the range selectors on top of the dfs share list.{/t}
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
     <LABEL for "regex"><img alt="{t}Display dfs shares matching{/t}" src="{$search_image}" align=middle title='{t}Display dfs shares matching{/t}'></LABEL>
     </td>
     <td width="99%">
     <input type='text' name='regex' maxlength='20' style='width:99%' value='{*$regex*}' id='filter'
     title='{t}Regular expression for matching dfs share names{/t}' onChange="mainform.submit()">
     </td>
     </tr>
     </table>
                            {$apply}
   </div>
  </td>
</tr>
</table summary="">

<input type="hidden" name="ignore">
