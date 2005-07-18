<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
 <tr>
   <td style="vertical-align:top;">
     <h2><img alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}</h2>
     
     <table summary="">
      <tr>
       <td><LABEL for="ou">{t}Name of department{/t}</LABEL>{$must}</td>
       <td><input id="ou" name="ou" size=25 maxlength=60 value="{$ou}" title="{t}Name of subtree to create{/t}"></td>
      </tr>
      <tr>
       <td><LABEL for="description">{t}Description{/t}</LABEL>{$must}</td>
       <td>
        <input id="description" name="description" size=25 maxlength=60 {$descriptionACL} value="{$description}" title="{t}Descriptive text for department{/t}">
       </td>
      </tr>
      <tr>
       <td><LABEL for="businessCategory">{t}Category{/t}</LABEL></td>
       <td>
        <input id="businessCategory" name="businessCategory" size=25 maxlength=60 {$businessCategoryACL} value="{$businessCategory}" title="{t}Category for this subtree{/t}">
       </td>
      </tr>
      <tr>
        <td colspan="2"><br></td>
      </tr>
      <tr>
        <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>

        <td>
         <select id="base" size="1" name="base" title="{t}Choose subtree to place department in{/t}"> 
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
       <td><LABEL for="st">{t}State{/t}/</LABEL></td>
       <td><input iD="st" name="st" size=25 maxlength=60 {$stACL} value="{$st}" title="{t}State where this subtree is located{/t}"></td>
      </tr>
      <tr>
       <td><LABEL for="l">{t}Location{/t}</LABEL></td>
       <td><input id="l" name="l" size=25 maxlength=60 {$lACL} value="{$l}" title="{t}Location of this subtree{/t}"></td>
      </tr>
      <tr>
       <td style="vertical-align:top;"><LABEL for="postalAddress">{t}Address{/t}</LABEL></td>
       <td><textarea id="postalAddress" name="postalAddress" rows=3 cols=22 {$postalAddressACL} title="{t}Postal address of this subtree{/t}">{$postalAddress}</textarea>
      </tr>
      <tr>
       <td><LABEL for="telephoneNumber">{t}Phone{/t}</LABEL></td>
       <td><input id="telephoneNumber" name="telephoneNumber" size=25 maxlength=60 {$telephoneNumberACL} value="{$telephoneNumber}" title="{t}Base telephone number of this subtree{/t}"></td>
      </tr>
      <tr>
       <td><LABEL for="facsimileTelephoneNumber">{t}Fax{/t}</LABEL></td>
       <td><input id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=25 maxlength=60 {$facsimileTelephoneNumberACL} value="{$facsimileTelephoneNumber}" title="{t}Base facsimile telephone number of this subtree{/t}"></td>
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
