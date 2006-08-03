<h2><img class="center" alt="" src="images/hotplug.png" align="middle">&nbsp;{t}Create new hotplug entry{/t}</h2>
	
	<input type="hidden" name="dialogissubmitted" value="1">
	
<table width="100%" summary="">
 <tr>
  <td width="50%" style="vertical-align:top">
	<table summary="{t}Create new hotplug device{/t}" style="border-right:1px solid #B0B0B0;width:100%">
		<tr>
			<td><LABEL for="HOT_name">{t}Device name{/t}</LABEL>{$must}
			</td>
			<td>	
				<input type="text" size=40 value="{$HOT_name}" name="HOT_name" id="HOT_name">
			</td>
		</tr>
		<tr>
			<td><LABEL for="HOT_description">{t}Description{/t}</LABEL>{$must}
			</td>
			<td>
				<input type="text" size=40 value="{$HOT_description}" name="HOT_description" id="HOT_description">
			</td>
		</tr>
	</table>
   </td>
   <td style="vertical-align:top">
	<table summary="">
		<tr>
			<td><LABEL for="HOT_vendor">{t}Vendor ID{/t}</LABEL>{$must}
			</td>
			<td>
				<input type="text" value="{$HOT_vendor}" name="HOT_vendor" id="HOT_vendor">
			</td>
		</tr>
		<tr>
			<td><LABEL for="HOT_id">{t}Device ID{/t}</LABEL>{$must}
			</td>
			<td>
				<input type="text" value="{$HOT_id}" name="HOT_id" id="HOT_id">
			</td>
			<td cols="2">&nbsp;</td>
		</tr>
		<tr>
			<td><LABEL for="HOT_produkt">{t}Product ID{/t}</LABEL>{$must}
			</td>
			<td>
				<input type="text" value="{$HOT_produkt}" name="HOT_produkt" id="HOT_produkt">
			</td>
		</tr>
	</table>
</table>
	<p class="seperator">&nbsp;</p>
	<p align="right">
	<input type="submit" name="HotPlugSave" 	value="{t}Save{/t}">
	<input type="submit" name="HotPlugCancel" 	value="{t}Cancel{/t}">
	</p>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.HOT_name.focus();
  -->
</script>

