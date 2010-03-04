<div class="contentboxh">
 <p class="contentboxh">
 {image path="images/launch.png" align="right"}{t}Filter{/t}
 
 </p>
</div>

<div class="contentboxb">
 
 <div style="border-top:1px solid #AAAAAA"></div>
 
 {$SERVER}&nbsp;{t}Show servers{/t}<br>
 {$WORKSTATION}&nbsp;{t}Show workstations{/t}<br>
 {$TERMINAL}&nbsp;{t}Show terminals{/t}<br>
 
 <div style="border-top:1px solid #AAAAAA"></div>
 
 {$SCOPE}
 
 <table style="width:100%;border-top:1px solid #B0B0B0;"
  summary="{t}List filter settings{/t}">
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
   <td colspan="2" align="right"  style="background:#EEEEEE;border-top:1px solid #B0B0B0;">
    {$APPLY}
   </td>
  </tr>
 </table>
</div>
