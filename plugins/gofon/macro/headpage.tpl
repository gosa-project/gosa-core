<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of macros{/t} {$hint}
    </p>
  </div>
  <div class="contentboxb">
      {$macroshead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$macros}
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top">
   <div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
    {t}This menu allows you to add, edit and remove selected macros. You may want to use the range selector on top of the macro listbox, when working with a large number of macros.{/t}
   </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
     <table style="width:100%;border-top:1px solid #B0B0B0;" summary="">
      {$alphabet}
     </table>
    <table style="width:100%;border-top:1px solid #B0B0B0;" summary="">
		<tr>
			<td>
				<img alt="{t}Display macros matching{/t}" src="{$search_image}" align=middle title='{t}Display macros matching{/t}'>
			</td>
    		<td width="99%">
				<input type='text' id='regex' name='regex' maxlength='20' value='{$regex}' style='width:99%;' title='{t}Regular expression for matching macro names{/t}' onChange="mainform.submit()">
			</td>
		</tr>
	</table>
    {$apply}
   </div>
  </td>
</tr>
</table>

<input type="hidden" name="ignore">
