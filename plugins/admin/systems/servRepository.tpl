<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
<tr>
  <td style="vertical-align:top;width:600px">
  <div class="contentboxh" style="height:20px;">
    <p class="contentboxh" style="font-size:12px">
     {t}List of configured repositories.{/t}
    </p>
  </div>
  <div class="contentboxb">
  	<div style='background:#F0F0F9;padding:5px;'>
<input type="image" src="images/fai_new_packages.png" title="{t}Add repository{/t}" name="AddRepository">
	</div>
</div>
<div style='height:4px;'></div>	  
  <div class="contentboxb" style="border-top:1px solid #B0B0B0;">
{$Repositories}
    <input type=hidden name="edit_helper">
  </div>
  </td>
		<td style="vertical-align:top;">
   			<div class="contentboxh" style="height:20px;border-bottom:1px solid #B0B0B0">
    				<p class="contentboxh" style="font-size:12px">
					<img src="images/info_small.png" align="right" alt="[i]">
					{t}Information{/t}
				</p>
	   		</div>
   			<div class="contentboxb" style="padding:0px;margin:0px;background:#f0f0f0;">
   				<p class="contentboxb" style="border-color:#f0f0f0;">
     					{t}This menu allows you to create, delete and edit repository settings.{/t}
   				</p>
   			</div>
   			<br>
   			<div class="contentboxh" style="height:20px">
    				<p class="contentboxh" style="font-size:12px">
					<img src="images/small_filter.png" align="right" alt="[F]">
					{t}Filters{/t}
				</p>
   			</div>
   <div class="contentboxb">
     <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
      {$alphabet}
     </table>
     <table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
     <tr>
     <td>
     <img alt="{t}Display objects matching{/t}" src="{$search_image}" align='middle' title='{t}Display objects matching{/t}'/>
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
<input type="hidden" name="servRepository" value="1">
