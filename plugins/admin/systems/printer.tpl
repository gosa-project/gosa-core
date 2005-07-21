<table summary="" width="100%">
 <tr>
  <td style="vertical-align:top; width:50%;">
	<table summary="">
	 <tr>
	  <td><LABEL for="cn" >{t}Printer name{/t}</LABEL>{$must}</td>
	  <td><input name="cn" id="cn" size=20 maxlength=60 value="{$cn}"></td>
	 </tr>
	 <tr>
	  <td><LABEL for="description">{t}Description{/t}</LABEL></td>
	  <td><input id="description" name="description" size=25 maxlength=80 value="{$description}"></td>
	 </tr>
	 <tr>
	  <td><LABEL for="l">{t}Location{/t}</LABEL></td>
	  <td><input id="l" name="l" size=25 maxlength=80 value="{$l}"></td>
	 </tr>
	 <tr>
          <td colspan=2>&nbsp;</td>
	 </tr>
 	 <tr>
	  <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
	  <td>
	   <select size="1" name="base" title="{t}Choose subtree to place terminal in{/t}">
	    {html_options options=$bases selected=$base_select}
	   </select>
	  </tr>
	</table>
  </td>
  <td style="vertical-align:top">
	<table summary="">
	 <tr>
	  <td><LABEL for="labeledURI">{t}Printer URL{/t}</LABEL></td>
	  <td><input id="labeledURI" name="labeledURI" size=25 maxlength=80 value="{$labeledURI}"></td>
	 </tr>
	 <tr>
	  <td><LABEL for="gotoPrinterPPD">{t}Path to PPD{/t}</LABEL></td>
	  <td><input id="gotoPrinterPPD" name="gotoPrinterPPD" size=25 maxlength=80 value="{$gotoPrinterPPD}"></td>
	 </tr>
	</table>
  </td>
 </tr>
</table>

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

{include file="$netconfig"}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
