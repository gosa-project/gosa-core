<h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Generic{/t}</h2>
<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>

 <tr>
   <td style="width:50%; vertical-align:top;">
    <table>
     <tr>
      <td>{t}List name{/t}{$must}</td>
      <td>
       <input name="cn" size=25 maxlength=60 {$mode} {$cnACL} value="{$cn}" title="{t}Name of blocklist{/t}">
      </td>
     </tr>
     <tr>
      <td>{t}Base{/t}{$must}</td>
      <td>
       <select size="1" name="base" {$selectmode} title="{t}Select subtree to place blocklist in{/t}">
        {html_options options=$bases selected=$base_select}
		<option disabled></option>
       </select>
      </td>
     </tr>
    </table>
   </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

   <td style="vertical-align:top;">
    <table>
     <tr>
       <td>{t}Type{/t}</td>
       <td>
        <select size="1" name="type" {$typeACL} title="{t}Select wether to filter incoming or outgoing calls{/t}">
        {html_options options=$types selected=$type}
		<option disabled></option>
        </select>
        </td>
      </tr>
      <tr>
       <td>{t}Description{/t}</td>
       <td>
         <input name="description" size=25 maxlength=60 {$descriptionACL} value="{$description}" title="{t}Descriptive text for this blocklist{/t}">
       </td>
      </tr>
    </table>
   </td>
 </tr>
</table>

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

<table>
 <tr>
   <td>
     <h2><img alt="" align="middle" src="images/false.png"> {t}Blocked numbers{/t}</h2>
     <select style="width:400px; height:200px;" name="numbers[]" size=15 multiple>
      {html_options values=$goFaxBlocklist output=$goFaxBlocklist}
	  <option disabled></option>
     </select>
     <br>
     <input name="number" size=25 maxlength=60 {$goFaxBlocklistACL}>&nbsp;
     <input type=submit value="{t}Add{/t}" {$goFaxBlocklistACL} name="add_number">&nbsp;
     <input type=submit value="{t}Delete{/t}" {$goFaxBlocklistACL} name="delete_number">
   </td>
   <td style="vertical-align:top;">
     <h2><img alt="" align="middle" src="images/info_small.png"> {t}Information{/t}</h2>
     <p>
      {t}Numbers can also contain wild cards.{/t}
     </p>
   </td>
 </tr>
</table>


<p class="plugbottom">
  <input type=submit name="edit_finish" value="{t}Finish{/t}">
  &nbsp;
  <input type=submit name="edit_cancel" value="{t}Cancel{/t}">
</p>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
