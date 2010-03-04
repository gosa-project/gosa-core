<div class="contentboxh">
 <p class="contentboxh">
 {image path="images/launch.png" align="right"}{t}Filter{/t}
 </p>
</div>

<div class="contentboxb">
 
 <div style="border-top:1px solid #AAAAAA"></div>
 
 {$PRIMARY}&nbsp;<label for='TEMPLATES'>{t}Show primary groups{/t}</label><br>
 {$SAMBA}&nbsp;<label for='TEMPLATES'>{t}Show samba groups{/t}</label><br>
 {$MAIL}&nbsp;<label for='TEMPLATES'>{t}Show mail groups{/t}</label><br>
 
 {$SCOPE}
 
 <table summary='{t}Filter options{/t}' summary="{t}Group selection filter{/t}" style="width:100%;border-top:1px solid #B0B0B0;">
  <tr>
   <td>
    <label for="NAME">
    {image path="images/lists/search.png"}&nbsp;Name
    </label>
   </td>
   <td>
    {$NAME}
   </td>
  </tr>
  <tr>
   <td width="100%" align="right" colspan="2" style="background:#EEEEEE;border-top:1px solid #B0B0B0;">
    {$APPLY}
   </td>
  </tr>
 </table>
</div>
