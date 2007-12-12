<table summary="" style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;"  cellpadding=0 cellspacing=0 border=0>

 <tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
  <td style="padding:5px;">{t}FAI object tree{/t}</td>
 </tr>

{if $readable}
 <tr>
	<td style="padding-left:5px;padding-top:5px;padding-bottom:12px;border-bottom:1px solid #B0B0B0">
		<input type="image" src="images/list_reload.png" name="reloadList" class="center">	
		{t}Reload class and release configuration from parent object.{/t}
	</td>
 </tr>
 <tr style="background-color: #E8E8E8; ">
	<td>
 {$objectList}
	</td>
 </tr>
{else}
<tr>
	<td style='padding:6px;'>
		{t}You are not allowed to view the fai summary.{/t}
	</td>
</tr>
{/if}
</table>

