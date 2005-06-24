<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
 <tr>
   <td style="vertical-align:top;">
     <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}</h2>
     
     <table summary="">
      <tr>
       <td>{t}Name of department{/t}{$must}</td>
       <td><input name="ou" size=25 maxlength=60 value="{$ou}" title="{t}Name of subtree to create{/t}"></td>
      </tr>
      <tr>
       <td>{t}Description{/t}{$must}</td>
       <td>
        <input name="description" size=25 maxlength=60 {$descriptionACL} value="{$description}" title="{t}Descriptive text for department{/t}">
       </td>
      </tr>
      <tr>
       <td>{t}Category{/t}</td>
       <td>
        <input name="businessCategory" size=25 maxlength=60 {$businessCategoryACL} value="{$businessCategory}" title="{t}Category for this subtree{/t}">
       </td>
      </tr>
      <tr>
        <td colspan="2"><br></td>
      </tr>
      <tr>
        <td>{t}Base{/t}{$must}</td>

        <td>
         <select size="1" name="base" title="{t}Choose subtree to place department in{/t}"> 
          {html_options options=$bases selected=$base_select}
         </select>
	</td>
       </tr>
     </table>

   </td>
   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   <td>
     <h2><img alt="" align="middle" src="images/house.png"> {t}Location{/t}</h2>

     <table summary="">
      <tr>
       <td>{t}State{/t}</td>
       <td><input name="st" size=25 maxlength=60 {$stACL} value="{$st}" title="{t}State where this subtree is located{/t}"></td>
      </tr>
      <tr>
       <td>{t}Location{/t}</td>
       <td><input name="l" size=25 maxlength=60 {$lACL} value="{$l}" title="{t}Location of this subtree{/t}"></td>
      </tr>
      <tr>
       <td style="vertical-align:top;">{t}Address{/t}</td>
       <td><textarea name="postalAddress" rows=3 cols=22 {$postalAddressACL} title="{t}Postal address of this subtree{/t}">{$postalAddress}</textarea>
      </tr>
      <tr>
       <td>{t}Phone{/t}</td>
       <td><input name="telephoneNumber" size=25 maxlength=60 {$telephoneNumberACL} value="{$telephoneNumber}" title="{t}Base telephone number of this subtree{/t}"></td>
      </tr>
      <tr>
       <td>{t}Fax{/t}</td>
       <td><input name="facsimileTelephoneNumber" size=25 maxlength=60 {$facsimileTelephoneNumberACL} value="{$facsimileTelephoneNumber}" title="{t}Base facsimile telephone number of this subtree{/t}"></td>
      </tr>
     </table>

   </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.ou.focus();
  -->
</script>
