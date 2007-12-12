<br> 
<div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
 <p class="contentboxh"><img src="{$branchimage}" align="right" alt="[F]">{t}Branches{/t}</p>
</div>
<div class="contentboxb">
 <table summary="" style="width:100%;">
  <tr>
   <td>
    {t}Current release{/t}&nbsp;
    <select name="selectedRelease" onChange="document.mainform.submit();">
     {html_options options=$releases selected=$selectedRelease}
    </select>
   </td>
  </tr>
 </table>
</div>
