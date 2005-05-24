<table style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>

 <!-- Headline container -->
 <tr>
  <td colspan=4>
   <h2><img alt="" align="middle" src="images/head.png"> {t}Personal information{/t}</h2>
  </td>
 </tr>

 <!-- Base 1 container -->
 <tr>

  <!-- Image container -->
  <td style="vertical-align:top">
   <img align="bottom" border="1" width="145" height="200" src="getpic.php?rand={$rand}" alt="{t}Personal picture{/t}">
   <br>
   <input type=submit name="edit_picture" {$jpegPhotoACL} value="{t}Change picture{/t}...">
  </td>

  <!-- Name, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Name{/t}{$must}</td>
     <td><input name="sn" size=25 maxlength=60 {$snACL} value="{$sn}"></td>
    </tr>

    {if $is_template ne "true"}
    <tr>
     <td>{t}Given name{/t}{$must}</td>
     <td><input name="givenName" size=25 maxlength=60 {$givenNameACL} value="{$givenName}"></td>
    </tr>
    <tr>
     <td>{t}Login{/t}{$must}</td>
     <td><input name="uid" size=25 maxlength=60 {$uidACL} value="{$uid}"></td>
    </tr>
    {/if}
      
    <tr>
     <td>
      <div style="height:10px;"></div>
      {t}Personal title{/t}
     </td>
     <td>
      <div style="height:10px;"></div>
      <input name="personalTitle" size=25 maxlength=60 {$personalTitleACL} value="{$personalTitle}">
     </td>
    </tr>
    <tr>
     <td>{t}Academic title{/t}</td>
     <td>
      <input name="academicTitle" size=25 maxlength=60 {$academicTitleACL} value="{$academicTitle}">
     </td>
    </tr>
    <tr>
     <td>
      <div style="height:10px;"></div>
      {t}Date of birth{/t}
     </td>
     <td>
      <div style="height:10px;"></div>
      {if $use_dob eq 1}
      <select name=day onChange="createResult(this.form,this.form.dob);" {$dobACL}>
       {html_options values=$days output=$days selected=$day}
      </select>
      <select name=month onChange="populate(this.form,this.form.dob);" {$dobACL}>
       {html_options options=$months selected=$month}
      </select>
      <select name=year onChange="populate(this.form,this.form.dob);" {$dobACL}>
       {html_options values=$years output=$years selected=$year}
      </select>
      <input type="hidden" name="dob" value="{$dob}">
      <input type=submit name="set_dob" value="-" {$dobACL}>
      {else}
      <input type=submit name="set_dob" value="{t}Set{/t}" {$dobACL}>
      {/if}
     </td>
    </tr>
    <tr>
     <td>
      {t}Sex{/t}
     </td>
     <td>
      <select size="1" name="gender" {$genderACL}>
       {html_options options=$gender_list selected=$gender}
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <div style="height:10px;"></div>
      {t}Base{/t}
     </td>
     <td>
      <div style="height:10px;"></div>
      <select size="1" name="base" {$selectmode} title="{t}Choose subtree to place user in{/t}">
       {html_options options=$bases selected=$base_select}
      </select>
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">

   <!-- Address, ... -->
   <table> 
    <tr>
     <td style="vertical-align:top;">{t}Address{/t}</td>
     <td><textarea name="homePostalAddress" rows=3 cols=22 {$homePostalAddressACL}>{$homePostalAddress}</textarea></td>
    </tr>
    <tr>
     <td>{t}Private phone{/t}</td>
     <td><input name="homePhone" size=25 maxlength=60 {$homePhoneACL} value="{$homePhone}"></td>
    </tr>
    <tr>
     <td>{t}Homepage{/t}</td>
     <td><input name="labeledURI" size=25 maxlength=60 {$labeledURIACL} value="{$labeledURI}"></td>
    </tr>
    <tr>
     <td colspan=2>
      <div style="height:20px;"></div>
     </td>
    </tr>
    <tr>
     <td>{t}Password storage{/t}</td>
     <td>
      <select size="1" name="pw_storage" {$passwordStorageACL}>
       {html_options values=$pwmode output=$pwmode selected=$pwmode_select}
      </select>
     </td>
    </tr>

    {if $is_template ne "true"}
    <tr>
     <td>{t}Certificates{/t}</td>
     <td>
      <input type=submit name="edit_cert" {$certificatesACL} value="{t}Edit certificates{/t}...">
     </td>
    </tr>
    <tr>
     <td>{t}Kerberos{/t}</td>
     <td><input type=submit name="edit_krb" disabled value="{t}Edit properties{/t}..."></td>
    </tr>
    {/if}

   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table class="line" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4 border=0>

 <!-- Headline container -->
 <tr>
  <td colspan=3>
   <h2><img alt="" align="middle" src="images/house.png"> {t}Organizational information{/t}</h2>
  </td>
 </tr>

 <!-- Base 2 container -->
 <tr>

  {if $governmentmode ne "true"}
  <!-- Company, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Organization{/t}</td>
     <td><input name="o" size=22 maxlength=60 {$oACL} value="{$o}"></td>
    </tr>
    <tr>
     <td>{t}Department{/t}</td>
     <td><input name="ou" size=22 maxlength=60 {$ouACL} value="{$ou}"></td>
    </tr>
    <tr>
     <td>{t}Department No.{/t}</td>
     <td>
      <input name="departmentNumber" size=22 maxlength=60 {$departmentNumberACL} value="{$departmentNumber}">
     </td>
    </tr>
    <tr>
     <td>{t}Employee No.{/t}</td>
     <td>
      <input name="employeeNumber" size=22 maxlength=60 {$employeeNumberACL} value="{$employeeNumber}">
     </td>
    </tr>
    <tr>
     <td>{t}Employee type{/t}</td>
     <td>
      <input name="employeeType" size=22 maxlength=60 {$employeeTypeACL} value="{$employeeType}">
     </td>
    </tr>
   </table>
  </td>
   
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <!-- Phone, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Room No.{/t}</td>
     <td><input name="roomNumber" size=22 maxlength=60 {$roomNumberACL} value="{$roomNumber}"></td>
    </tr>
    {if $has_phoneaccount ne "true"}
    <tr>
     <td>{t}Phone{/t}</td>
     <td>
      <input name="telephoneNumber" size=22 maxlength=60 {$telephoneNumberACL} value="{$telephoneNumber}">
     </td>
    </tr>
    {/if}
    <tr>
     <td>{t}Mobile{/t}</td>
     <td><input name="mobile" size=22 maxlength=60 {$mobileACL} value="{$mobile}"></td>
    </tr>
    <tr>
     <td>{t}Pager{/t}</td>
     <td>
      <input name="pager" size=22 maxlength=60 {$pagerACL} value="{$pager}">
     </td>
    </tr>
    <tr>
     <td>{t}Fax{/t}</td>
     <td><input name="facsimileTelephoneNumber" size=22 maxlength=60 {$facsimileTelephoneNumberACL}
                value="{$facsimileTelephoneNumber}">
     </td>
    </tr>
   </table>
  </td>

  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>

  <!-- Location, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Location{/t}</td>
     <td><input name="l" size=22 maxlength=60 {$lACL} value="{$l}"></td>
    </tr>
    <tr>
     <td>{t}State{/t}</td>
     <td><input name="st" size=22 maxlength=60 {$stACL} value="{$st}"></td>
    </tr>
    <tr>
     <td style="vertical-align:top;">{t}Address{/t}</td>
     <td>
      <textarea name="postalAddress" rows=3 cols=22 {$postalAddressACL}>{$postalAddress}</textarea>
     </td>
    </tr>
   </table>
  </td>

  {else} 

  <!-- Beschreibung, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Vocation{/t}</td>
     <td><input name="vocation" size=22 maxlength=60 {$vocationACL} value="{$vocation}"></td>
    </tr>
    <tr>
     <td>{t}Unit description{/t}</td>
     <td>
      <input name="gouvernmentOrganizationalUnitDescription" size=22 maxlength=60
	{$gouvernmentOrganizationalUnitDescriptionACL}
	value="{$gouvernmentOrganizationalUnitDescription}">
     </td>
    </tr>
    <tr>
     <td>{t}Subject area{/t}</td>
     <td>
      <input name="gouvernmentOrganizationalUnitSubjectArea" size=22 maxlength=60
          {$gouvernmentOrganizationalUnitSubjectAreaACL}
           value="{$gouvernmentOrganizationalUnitSubjectArea}">
     </td>
    </tr>
    <tr>
     <td>{t}Functional title{/t}</td>
     <td>
      <input name="functionalTitle" size=22 maxlength=60 {$functionalTitleACL}
             value="{$functionalTitle}">
     </td>
    </tr>
    <tr>
     <td>{t}Role{/t}</td>
     <td>
      <input name="role" size=22 maxlength=60 {$roleACL} value="{$role}">
     </td>
    </tr>
   </table>
  </td>
  
  <!-- Phone, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Person locality{/t}</td>
     <td>
      <input name="gouvernmentOrganizationalPersonLocality" size=22 maxlength=60
         {$gouvernmentOrganizationalPersonLocalityACL}
                value="{$gouvernmentOrganizationalPersonLocality}">
     </td>
    </tr>
    <tr>
     <td>{t}Unit{/t}</td>
     <td><input name="gouvernmentOrganizationalUnit" size=22 maxlength=60 
     {$gouvernmentOrganizationalUnitACL}
            value="{$gouvernmentOrganizationalUnit}">
     </td>
    </tr>
    <tr>
     <td>{t}Street{/t}</td>
     <td>
      <input name="street" size=22 maxlength=60 {$streetACL} value="{$street}">
     </td>
    </tr>
    <tr>
     <td>{t}Postal code{/t}</td>
     <td> 
      <input name="postalCode" size=22 maxlength=60 {$postalCodeACL} value="{$postalCode}">
     </td>
    </tr>
    <tr>
     <td>{t}House identifier{/t}</td>
     <td>
      <input name="houseIdentifier" size=22 maxlength=60 {$houseIdentifierACL}
             value="{$houseIdentifier}">
     </td>
    </tr>
   </table>
  </td>

  <!-- Location, ... -->
  <td style="vertical-align:top;">
   <table> 
    <tr>
     <td>{t}Room No.{/t}</td>
     <td><input name="roomNumber" size=22 maxlength=60 {$roomNumberACL} value="{$roomNumber}"></td>
    </tr>
    <tr>
     <td>{t}Phone{/t}</td>
     <td>
      {if $has_phoneaccount ne "true"}
      <input name="telephoneNumber" size=22 maxlength=60 {$telephoneNumberACL} value="{$telephoneNumber}">
      {else}
      <i>{t}Please use the phone tab{/t}</i>
      {/if}
     </td>
    </tr>
    <tr>
     <td>{t}Fax{/t}</td>
     <td>
      <input name="facsimileTelephoneNumber" size=22 maxlength=60 
	          {$facsimileTelephoneNumberACL}
                 value="{$facsimileTelephoneNumber}">
     </td>
    </tr>
    <tr>
     <td>{t}Last delivery{/t}</td>
     <td>
      <input name="ivbbLastDeliveryCollective" size=22 maxlength=60 
      {$ivbbLastDeliveryCollectiveACL}
             value="{$ivbbLastDeliveryCollective}">
     </td>
    </tr>
    <tr>
     <td>{t}Public visible{/t}</td>
     <td>
      <select size="1" name="publicVisible" {$publicVisibleACL}>
       {html_options values=$ivbbmodes output=$ivbbmodes selected=$publicVisible}
      </select>
     </td>
    </tr>
   </table>
  </td>
 {/if} 
 </tr>
</table>

<input type=hidden name="generic">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.sn.focus();
  if (yearSel != "") populate(document.mainform,document.mainform.dob);
  -->
</script>

