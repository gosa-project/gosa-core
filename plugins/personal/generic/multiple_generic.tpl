<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">
 <tr>
  <td colspan="4">
    <img alt="" align="middle" src="images/head.png" class="center">
   	<h2>{t}Personal information{/t}</h2>
  </td>
 </tr>
 <tr>
  <td style="vertical-align:top">
   <table>
    <tr>
     <td width="147" height="200" bgcolor="gray">
      <img class='center' border="0" width="100%" src="plugins/users/images/default.jpg" alt="{t}Personal picture{/t}">
     </td>
    </tr>
   </table>
  </td>
  <td style="vertical-align:top;">
   <table>
    <tr>
     <td>
      <label for="academicTitle">
      {t}Academic title{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_academicTitle" value="1" {if $use_academicTitle} checked {/if}>
      <input type='text' id="academicTitle" name="academicTitle" size=25 maxlength=60 value="{$academicTitle}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="preferredLanguage">
      {t}Preferred langage{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_preferredLanguage" value="1" {if $use_preferredLanguage} checked {/if}>
      <select size="1" id="preferredLanguage" name="preferredLanguage">
       {html_options options=$preferredLanguage_list selected=$preferredLanguage}
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <div style="height:10px;">
      </div>
      <label for="base">
      {t}Base{/t}</label>
     </td>
     <td>
      <div style="height:10px;">
      </div>
	<input type="checkbox" name="use_base" value="1" {if $use_base} checked {/if}>
      <select id="base" size="1" name="base" title="{t}Choose subtree to place user in{/t}">
       {html_options options=$bases selected=$base_select}
      </select>
      <input type="image" name="chooseBase" src="images/lists/folder.png" class="center" title="{t}Select a base{/t}">
     </td>
    </tr>
   </table>
  </td>
  <td style="vertical-align:top;">
   <!-- Address, ... -->
   <table summary="" style="width:100%">
    <tr>
     <td style="vertical-align:top;">
      <label for="homePostalAddress">
      {t}Address{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_homePostalAddress" value="1" {if $use_homePostalAddress} checked {/if}>
      <textarea id="homePostalAddress" name="homePostalAddress" rows="3" style="width:100%">
      {$homePostalAddress}</textarea>
     </td>
    </tr>
    <tr>
     <td>
      <label for="homePhone">
      {t}Private phone{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_homePhone" value="1" {if $use_homePhone} checked {/if}>
      <input type='text' id="homePhone" name="homePhone" size=25 maxlength=60 value="{$homePhone}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="labeledURI">
      {t}Homepage{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_labeledURI" value="1" {if $use_labeledURI} checked {/if}>
      <input type='text' id="labeledURI" name="labeledURI" size=25 maxlength=60 value="{$labeledURI}">
     </td>
    </tr>
    <tr>
     <td colspan=2>
      <div style="height:20px;">
      </div>
     </td>
    </tr>
<!--
    <tr>
     <td>
      <label for="pw_storage">
      {t}Password storage{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_pw_storage" value="1" {if $use_pw_storage} checked {/if}>
      <select size="1" id="pw_storage" name="pw_storage" onchange='document.mainform.submit()'>
       {html_options values=$pwmode output=$pwmode selected=$pwmode_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <label for="edit_cert">
      {t}Certificates{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_edit_cert" value="1" {if $use_edit_cert} checked {/if}>
      <input id="edit_cert" type="submit" name="edit_cert" value="{t}Edit certificates{/t}...">
     </td>
    </tr>
-->
   </table>
  </td>
 </tr>
</table>
<p class="seperator">
&nbsp;</p>
<table summary="" class="line" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">
 <!-- Headline container -->
 <tr>
  <td colspan="3">
   <h2>
    <img alt="" align="middle" src="plugins/generic/images/house.png" class="center">
   &nbsp;{t}Organizational information{/t}</h2>
  </td>
 </tr>
 <!-- Base 2 container -->
 <tr>
  {if $governmentmode ne "true"}
  <!-- Company, ... -->
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td>
      <label for="o">
      {t}Organization{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_o" value="1" {if $use_o} checked {/if}>
      <input type='text' id="o" name="o" size=22 maxlength=60 value="{$o}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="ou">
      {t}Department{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_ou" value="1" {if $use_ou} checked {/if}>
      <input type='text' id="ou" name="ou" size=22 maxlength=60 value="{$ou}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="departmentNumber">
      {t}Department No.{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_departmentNumber" value="1" {if $use_departmentNumber} checked {/if}>
      <input type='text' id="departmentNumber" name="departmentNumber" size=22 maxlength=60 value="{$departmentNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="employeeNumber">
      {t}Employee No.{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_employeeNumber" value="1" {if $use_employeeNumber} checked {/if}>
      <input type='text' id="employeeNumber" name="employeeNumber" size=22 maxlength=60 value="{$employeeNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="employeeType">
      {t}Employee type{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_employeeType" value="1" {if $use_employeeType} checked {/if}>
      <input type='text' id="employeeType" name="employeeType" size=22 maxlength=60 value="{$employeeType}">
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <!-- Phone, ... -->
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td>
      <label for="roomNumber">
      {t}Room No.{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_roomNumber" value="1" {if $use_roomNumber} checked {/if}>
      <input type='text' id="roomNumber" name="roomNumber" size=22 maxlength=60 value="{$roomNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="telephoneNumber">
      {t}Phone{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_telephoneNumber" value="1" {if $use_telephoneNumber} checked {/if}>
      <input type='text' id="telephoneNumber" name="telephoneNumber" size=22 maxlength=60 value="{$telephoneNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="mobile">
      {t}Mobile{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_mobile" value="1" {if $use_mobile} checked {/if}>
      <input type='text' id="mobile" name="mobile" size=22 maxlength=60 value="{$mobile}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="pager">
      {t}Pager{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_pager" value="1" {if $use_pager} checked {/if}>
      <input type='text' id="pager" name="pager" size=22 maxlength=60 value="{$pager}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="facsimileTelephoneNumber">
      {t}Fax{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_facsimileTelephoneNumber" value="1" {if $use_facsimileTelephoneNumber} checked {/if}>
      <input type='text' id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=22 maxlength=60 value="{$facsimileTelephoneNumber}">
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <!-- Location, ... -->
  <td style="vertical-align:top;">
   <table summary="" style="width:100%">
    <tr>
     <td>
      <label for="l">
      {t}Location{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_l" value="1" {if $use_l} checked {/if}>
      <input type='text' id="l" name="l" size=22 maxlength=60 value="{$l}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="st">
      {t}State{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_st" value="1" {if $use_st} checked {/if}>
      <input type='text' id="st" name="st" size=22 maxlength=60 value="{$st}">
     </td>
    </tr>
    <tr>
     <td style="vertical-align:top;">
      <label for="postalAddress">
      {t}Address{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_postalAddress" value="1" {if $use_postalAddress} checked {/if}>
      <textarea id="postalAddress" name="postalAddress" rows=3 style="width:100%">
      {$postalAddress}</textarea>
     </td>
    </tr>
   </table>
  </td>
  {else} 
  <!-- Beschreibung, ... -->
  <td style="vertical-align:top;border-right:1px solid #b0b0b0">
   <table summary="">
    <tr>
     <td>
      <label for="vocation">
      {t}Vocation{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_vocation" value="1" {if $use_vocation} checked {/if}>
      <input type='text' id="vocation" name="vocation" size=22 maxlength=60 value="{$vocation}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="gouvernmentOrganizationalUnitDescription">
      {t}Unit description{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_gouvernmentOrganizationalUnitDescription" value="1" {if $use_gouvernmentOrganizationalUnitDescription} checked {/if}>
      <input id="gouvernmentOrganizationalUnitDescription" name="gouvernmentOrganizationalUnitDescription" size=22 maxlength=60
      value="{$gouvernmentOrganizationalUnitDescription}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="gouvernmentOrganizationalUnitSubjectArea">
      {t}Subject area{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_gouvernmentOrganizationalUnitSubjectArea" value="1" {if $use_gouvernmentOrganizationalUnitSubjectArea} checked {/if}>
      <input id="gouvernmentOrganizationalUnitSubjectArea" name="gouvernmentOrganizationalUnitSubjectArea" size=22 maxlength=60
      value="{$gouvernmentOrganizationalUnitSubjectArea}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="functionalTitle">
      {t}Functional title{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_functionalTitle" value="1" {if $use_functionalTitle} checked {/if}>
      <input name="functionalTitle" id="functionalTitle"  size=22 maxlength=60
      value="{$functionalTitle}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="role">
      {t}Role{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_role" value="1" {if $use_role} checked {/if}>
      <input type='text' id="role" name="role" size=22 maxlength=60 value="{$role}">
     </td>
    </tr>
   </table>
  </td>
  <!-- Phone, ... -->
  <td style="vertical-align:top;border-right:1px solid #b0b0b0">
   <table summary="">
    <tr>
     <td>
      <label for="gouvernmentOrganizationalPersonLocality">
      {t}Person locality{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_gouvernmentOrganizationalPersonLocality" value="1" {if $use_gouvernmentOrganizationalPersonLocality} checked {/if}>
      <input id="gouvernmentOrganizationalPersonLocality" name="gouvernmentOrganizationalPersonLocality" size=22 maxlength=60
      value="{$gouvernmentOrganizationalPersonLocality}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="gouvernmentOrganizationalUnit">
      {t}Unit{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_gouvernmentOrganizationalUnit" value="1" {if $use_gouvernmentOrganizationalUnit} checked {/if}>
      <input id="gouvernmentOrganizationalUnit" name="gouvernmentOrganizationalUnit" size=22 maxlength=60 
      value="{$gouvernmentOrganizationalUnit}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="street">
      {t}Street{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_street" value="1" {if $use_street} checked {/if}>
      <input type='text' name="street" id="street" size=22 maxlength=60 value="{$street}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="postalCode">
      {t}Postal code{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_postalCode" value="1" {if $use_postalCode} checked {/if}>
      <input type='text' name="postalCode" id="postalCode" size=22 maxlength=60 value="{$postalCode}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="houseIdentifier">
      {t}House identifier{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_houseIdentifier" value="1" {if $use_houseIdentifier} checked {/if}>
      <input id="houseIdentifier" name="houseIdentifier" size=22 maxlength=60
      value="{$houseIdentifier}">
     </td>
    </tr>
   </table>
  </td>
  <!-- Location, ... -->
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
     <td>
      <label for="roomNumber">
      {t}Room No.{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_roomNumber" value="1" {if $use_roomNumber} checked {/if}>
      <input type='text' id="roomNumber" name="roomNumber" size=22 maxlength=60 value="{$roomNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="telephoneNumber">
      {t}Phone{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_telephoneNumber" value="1" {if $use_telephoneNumber} checked {/if}>
      <input type='text' id="telephoneNumber" name="telephoneNumber" size=22 maxlength=60 value="{$telephoneNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="facsimileTelephoneNumber">
      {t}Fax{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_facsimileTelephoneNumber" value="1" {if $use_facsimileTelephoneNumber} checked {/if}>
      <input id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=22 maxlength=60 
      value="{$facsimileTelephoneNumber}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="ivbbLastDeliveryCollective">
      {t}Last delivery{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_ivbbLastDeliveryCollective" value="1" {if $use_ivbbLastDeliveryCollective} checked {/if}>
      <input name="ivbbLastDeliveryCollective" size=22 maxlength=60 id="ivbbLastDeliveryCollective" 
      value="{$ivbbLastDeliveryCollective}">
     </td>
    </tr>
    <tr>
     <td>
      <label for="publicVisible">
      {t}Public visible{/t}</label>
     </td>
     <td>
	<input type="checkbox" name="use_publicVisible" value="1" {if $use_publicVisible} checked {/if}>
      <select size="1" id="publicVisible" name="publicVisible">
       {html_options values=$ivbbmodes output=$ivbbmodes selected=$publicVisible}
      </select>
     </td>
    </tr>
   </table>
  </td>
  {/if} 
 </tr>
</table>
<input type=hidden name="multiple_user_posted" value="1">
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
<!-- // First input field on page
focus_field('academicTitle');
-->
</script>
