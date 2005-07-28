<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of applications{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
      {$applicationshead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$applications}
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
    {t}This menu allows you to add, edit and remove selected applications. You may want to use the range selector on top of the application listbox, when working with a large number of applications.{/t}
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
    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
		<tr>
			<td>
				<LABEL for="regex"><img alt="" src="{$search_image}" align=middle title='{t}Display applications matching{/t}'>&nbsp;</LABEL>
			</td>
    		<td>
				<input style='width:99%' type='text' id="regex" name='regex' maxlength='20' value='{$regex}' title='{t}Regular expression for matching application names{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
    {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
