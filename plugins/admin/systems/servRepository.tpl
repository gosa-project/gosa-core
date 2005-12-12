<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:600px">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of configured repositories.{/t}
    </p>
  </div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
{$Repositories}
<input type="submit" value="{t}Add{/t}" name="AddRepository">
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb" style="padding:0px;margin:0px;" ><p class="contentboxb">
     {t}This menu allows you to create, delete and edit repository settings.{/t}
   </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh">
			<img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}
	</p>
   </div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
     <table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
     <tr>
     <td>
     <label for="regex"><img alt="{t}Display objects matching{/t}" src="{$search_image}" align='middle' title='{t}Display objects matching{/t}'/>
     </td>
     <td width="99%">
     <input type='text' name='regex' maxlength='20' style='width:99%' value='{$regex}' id='filter' 
     title='{t}Regular expression for matching object names{/t}' onChange="mainform.submit()">
     </td>
     </tr>
     </table>
			  {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
