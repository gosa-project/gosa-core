<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:600px">
  <div class="contentboxh">
    <p class="contentboxh">
     {t}List of FAI objects (Fully Automatic Installation){/t}
    </p>
  </div>
  <div class="contentboxb">
      {$faihead}
  </div>
  <div style='height:4px;'></div>
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
      {$failist}
    <input type=hidden name="edit_helper">
  </div>
  </td>
  <td style="vertical-align:top;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
   <p class="contentboxb">
     {t}This menu allows you to create, delete and edit selected FAI obejcts. Having a large size of objects, you might prefer the range selectors on top of this list.{/t}
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
		<tr><td><input type="checkbox" name="ShowProfiles" 		{$ShowProfilesCHK}>{t}Show profiles{/t}</td></tr>
		<tr><td><input type="checkbox" name="ShowTemplates" 	{$ShowTemplatesCHK}>{t}Show templates{/t}</td></tr>
		<tr><td><input type="checkbox" name="ShowScripts" 		{$ShowScriptsCHK}>{t}Show scripts{/t}</td></tr>
		<tr><td><input type="checkbox" name="ShowHooks" 		{$ShowHooksCHK}>{t}Show hooks{/t}</td></tr>
		<tr><td><input type="checkbox" name="ShowVariables" 	{$ShowVariablesCHK}>{t}Show variables{/t}</td></tr>
		<tr><td><input type="checkbox" name="ShowPackages" 		{$ShowPackagesCHK}>{t}Show packages{/t}</td></tr>
		<tr><td><input type="checkbox" name="ShowPartitions"	{$ShowPartitionsCHK}>{t}Show partitions{/t}</td></tr>
	 </table>
     <table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
     <tr>
     <td>
     <LABEL for "regex"><img alt="{t}Display objects matching{/t}" src="{$search_image}" align=middle title='{t}Display objects matching{/t}'></LABEL>
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
</table summary="">

<input type="hidden" name="ignore">
