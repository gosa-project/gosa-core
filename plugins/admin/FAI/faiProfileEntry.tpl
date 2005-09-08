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
		<tr><td><input onChange='document.mainform.submit()' 
								type="checkbox" name="SShowTemplates" 	{$SShowTemplatesCHK}>{t}Show only classes with templates{/t}</td></tr>
		<tr><td><input onChange='document.mainform.submit()'
								type="checkbox" name="SShowScripts" 		{$SShowScriptsCHK}>{t}Show only classes with scripts{/t}</td></tr>
		<tr><td><input onChange='document.mainform.submit()'
								type="checkbox" name="SShowHooks" 		{$SShowHooksCHK}>{t}Show only classes with hooks{/t}</td></tr>
		<tr><td><input onChange='document.mainform.submit()'
								type="checkbox" name="SShowVariables" 	{$SShowVariablesCHK}>{t}Show only classes with variables{/t}</td></tr>
		<tr><td><input onChange='document.mainform.submit()'
								type="checkbox" name="SShowPackages" 	{$SShowPackagesCHK}>{t}Show only classes with packages{/t}</td></tr>
		<tr><td><input onChange='document.mainform.submit()'
								type="checkbox" name="SShowPartitions"	{$SShowPartitionsCHK}>{t}Show only classes with partitions{/t}</td></tr>
	 </table>
     <table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
     <tr>
     <td>
     <LABEL for "regex"><img alt="{t}Display objects matching{/t}" src="{$search_image}" align=middle title='{t}Display objects matching{/t}'></LABEL>
     </td>
     <td width="99%">
     <input type='text' name='Sregex' maxlength='20' style='width:99%' value='{$Sregex}' id='filter' 
     title='{t}Regular expression for matching object names{/t}' onChange="mainform.submit()">
     </td>
     </tr>
     </table>
			    {$apply}
   </div>
  </td>
</tr>
</table summary="">
<div align="right" style="align:right;">
	<input name="SaveSubObject" value="Use" type="submit">
	<input name="CancelSubObject" value="Cancel" type="submit">
</div>
<input type="hidden" name="ignore">
