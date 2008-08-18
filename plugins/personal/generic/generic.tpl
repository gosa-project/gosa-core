<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">

 <!-- Headline container -->
 <tr>
  <td colspan="4">
   <h2><img alt="" align="middle" src="images/head.png" class="center"> {t}Personal information{/t}</h2>
  </td>
 </tr>

 <!-- Base 1 container -->
 <tr>

  <!-- Image container -->
	<!-- Image container -->
	<td style="vertical-align:top">
	  <table>
	   <tr>
		<td width="147" height="200" bgcolor="gray">
		 <img align="middle" border="0" width="100%" src="getbin.php?rand={$rand}" alt="{t}Personal picture{/t}">
		</td>
	   </tr>
	   <tr>
		<td>
		 <input type="submit" name="edit_picture" {$jpegPhotoACL} value="{t}Change picture{/t}...">
		</td>
	   </tr>
	  </table>
	</td>
	<!--
	Old image container, with incorrect image scaling.
  <td style="vertical-align:top">
   <img align="bottom" border="1" width="145" height="200" src="getbin.php?rand={$rand}" alt="{t}Personal picture{/t}">
   <br>
   <input type="submit" name="edit_picture" {$jpegPhotoACL} value="{t}Change picture{/t}...">
  </td>
	-->

  <!-- Name, ... -->
  <td style="vertical-align:top;">
   <table summary=""> 
    {if $is_template ne "true"}
    <tr>
     <td><label for="sn">{t}Last name{/t}{$must}</label></td>
     <td><input id="sn" name="sn" size=25 maxlength=60 {$snACL} value="{$sn}"></td>
    </tr>
    {else}
    <tr>
     <td><label for="sn">{t}Template name{/t}{$must}</label></td>
     <td><input id="sn" name="sn" size=25 maxlength=60 {$snACL} value="{$sn}"></td>
    </tr>
    {/if}

    {if $is_template ne "true"}
    <tr>
     <td><label for="givenName">{t}First name{/t}{$must}</label></td>
     <td><input id="givenName" name="givenName" size=25 maxlength=60 {$givenNameACL} value="{$givenName}"></td>
    </tr>
    <tr>
     <td><label for="uid">{t}Login{/t}{$must}</label></td>
     <td><input id="uid" name="uid" size=25 maxlength=60 {$uidACL} value="{$uid}"></td>
    </tr>
    {/if}
      
    <tr>
     <td>
      <div style="height:10px;"></div>
      <label for="personalTitle">{t}Personal title{/t}</label>
     </td>
     <td>
      <div style="height:10px;"></div>
      <input id="personalTitle" name="personalTitle" size=25 maxlength=60 {$personalTitleACL} value="{$personalTitle}">
     </td>
    </tr>
    <tr>
     <td><label for="academicTitle">{t}Academic title{/t}</label></td>
     <td>
      <input id="academicTitle" name="academicTitle" size=25 maxlength=60 {$academicTitleACL} value="{$academicTitle}">
     </td>
    </tr>
    <tr>
     <td>
      <div style="height:10px;"></div>
      <label for="day">{t}Date of birth{/t}</label>
     </td>
     <td>
      <div style="height:10px;"></div>
      {if $use_dob eq 1}
      <select id="day" name=day onChange="createResult(this.form,this.form.dateOfBirth);" {$dateOfBirthACL}>
       {html_options values=$days output=$days selected=$day}
      </select>
      <select name=month onChange="populate(this.form,this.form.dateOfBirth);" {$dateOfBirthACL}>
       {html_options options=$months selected=$month}
      </select>
      <select name=year onChange="populate(this.form,this.form.dateOfBirth);" {$dateOfBirthACL}>
       {html_options values=$years output=$years selected=$year}
      </select>
      <input type="hidden" name="dateOfBirth" value="{$dateOfBirth}">
      <input type="submit" name="set_dob" value="-" {$dateOfBirthACL}>
      {else}
      <input type="submit" name="set_dob" value="{t}Set{/t}" {$dateOfBirthACL}>
      {/if}
     </td>
    </tr>
    <tr>
     <td>
      <label for="gender">{t}Sex{/t}</label>
     </td>
     <td>
      <select size="1" id="gender" name="gender" {$genderACL}>
       {html_options options=$gender_list selected=$gender}
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <label for="preferredLanguage">{t}Preferred langage{/t}</label>
     </td>
     <td>
      <select size="1" id="preferredLanguage" name="preferredLanguage" {$preferredLanguageACL}>
       {html_options options=$preferredLanguage_list selected=$preferredLanguage}
      </select>
     </td>
    </tr>
    <tr>
     <td>
      <div style="height:10px;"></div>
      <label for="base">{t}Base{/t}</label>
     </td>
     <td>
      <div style="height:10px;"></div>
      <select id="base" size="1" name="base" {$selectmode} title="{t}Choose subtree to place user in{/t}">
       {html_options options=$bases selected=$base_select}
      </select>
		{if $selectmode == ""}
			<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
		{else}
			<img src="images/folder_gray.png" class="center" title="{t}Select a base{/t}" alt=''>
		{/if}
     </td>
    </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">

   <!-- Address, ... -->
   <table summary="" style="width:100%"> 
    <tr>
     <td style="vertical-align:top;"><label for="homePostalAddress">{t}Address{/t}</label></td>
     <td><textarea id="homePostalAddress" name="homePostalAddress" rows="3" style="width:100%" {$homePostalAddressACL}>{$homePostalAddress}</textarea></td>
    </tr>
    <tr>
     <td><label for="homePhone">{t}Private phone{/t}</label></td>
     <td><input id="homePhone" name="homePhone" size=25 maxlength=60 {$homePhoneACL} value="{$homePhone}"></td>
    </tr>
    <tr>
     <td><label for="labeledURI">{t}Homepage{/t}</label></td>
     <td><input id="labeledURI" name="labeledURI" size=25 maxlength=60 {$labeledURIACL} value="{$labeledURI}"></td>
    </tr>
    <tr>
     <td colspan=2>
      <div style="height:20px;"></div>
     </td>
    </tr>
    <tr>
     <td><label for="pw_storage">{t}Password storage{/t}</label></td>
     <td>
      <select size="1" id="pw_storage" name="pw_storage" {$passwordStorageACL}>
       {html_options values=$pwmode output=$pwmode selected=$pwmode_select}
      </select>
     </td>
    </tr>

    {if $is_template ne "true"}
    <tr>
     <td><label for="edit_cert">{t}Certificates{/t}</label></td>
     <td>
      <input id="edit_cert" type="submit" name="edit_cert" {$certificatesACL} value="{t}Edit certificates{/t}...">
     </td>
    </tr>
    <tr>
     <td><label for="edit_krb">{t}Kerberos{/t}</label></td>
     <td><input id="edit_krb" type="submit" name="edit_krb" disabled value="{t}Edit properties{/t}..."></td>
    </tr>
    {/if}

	{if $gosaLastSystemLogin}
		<tr>
			<td>{t}Last logon{/t}</td>
			<td>{$gosaLastSystemLogin}</td>
		</tr>		
	{/if}
   </table>
  </td>
 </tr>
</table>

<p class="seperator">&nbsp;</p>

<table summary="" class="line" style="width:100%; vertical-align:top; text-align:left;" cellpadding="4" border="0">

 <!-- Headline container -->
 <tr>
  <td colspan="3">
   <h2><img alt="" align="middle" src="images/house.png" class="center">&nbsp;{t}Organizational information{/t}</h2>
  </td>
 </tr>

 <!-- Base 2 container -->
 <tr>

  {if $governmentmode ne "true"}
  <!-- Company, ... -->
  <td style="vertical-align:top;">
   <table summary=""> 
    <tr>
     <td><label for="o">{t}Organization{/t}</label></td>
     <td><input id="o" name="o" size=22 maxlength=60 {$oACL} value="{$o}"></td>
    </tr>
    <tr>
     <td><label for="ou">{t}Department{/t}</label></td>
     <td><input id="ou" name="ou" size=22 maxlength=60 {$ouACL} value="{$ou}"></td>
    </tr>
    <tr>
     <td><label for="departmentNumber">{t}Department No.{/t}</label></td>
     <td>
      <input id="departmentNumber" name="departmentNumber" size=22 maxlength=60 {$departmentNumberACL} value="{$departmentNumber}">
     </td>
    </tr>
    <tr>
     <td><label for="employeeNumber">{t}Employee No.{/t}</label></td>
     <td>
      <input id="employeeNumber" name="employeeNumber" size=22 maxlength=60 {$employeeNumberACL} value="{$employeeNumber}">
     </td>
    </tr>
    <tr>
     <td><label for="employeeType">{t}Employee type{/t}</label></td>
     <td>
      <input id="employeeType" name="employeeType" size=22 maxlength=60 {$employeeTypeACL} value="{$employeeType}">
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
     <td><label for="roomNumber">{t}Room No.{/t}</label></td>
     <td><input id="roomNumber" name="roomNumber" size=22 maxlength=60 {$roomNumberACL} value="{$roomNumber}"></td>
    </tr>
    {if $has_phoneaccount ne "true"}
    <tr>
     <td><label for="telephoneNumber">{t}Phone{/t}</label></td>
     <td>
      <input id="telephoneNumber" name="telephoneNumber" size=22 maxlength=60 {$telephoneNumberACL} value="{$telephoneNumber}">
     </td>
    </tr>
    {/if}
    <tr>
     <td><label for="mobile">{t}Mobile{/t}</label></td>
     <td><input id="mobile" name="mobile" size=22 maxlength=60 {$mobileACL} value="{$mobile}"></td>
    </tr>
    <tr>
     <td><label for="pager">{t}Pager{/t}</label></td>
     <td>
      <input id="pager" name="pager" size=22 maxlength=60 {$pagerACL} value="{$pager}">
     </td>
    </tr>
    <tr>
     <td><label for="facsimileTelephoneNumber">{t}Fax{/t}</label></td>
     <td><input id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=22 maxlength=60 {$facsimileTelephoneNumberACL}
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
   <table summary="" style="width:100%"> 
    <tr>
     <td><label for="l">{t}Location{/t}</label></td>
     <td><input id="l" name="l" size=22 maxlength=60 {$lACL} value="{$l}"></td>
    </tr>
    <tr>
     <td><label for="st">{t}State{/t}</label></td>
     <td><input id="st" name="st" size=22 maxlength=60 {$stACL} value="{$st}"></td>
    </tr>
    <tr>
     <td style="vertical-align:top;"><label for="postalAddress">{t}Address{/t}</label></td>
     <td>
      <textarea id="postalAddress" name="postalAddress" rows=3 style="width:100%" {$postalAddressACL}>{$postalAddress}</textarea>
     </td>
    </tr>
   </table>
  </td>

  {else} 

  <!-- Beschreibung, ... -->
  <td style="vertical-align:top;border-right:1px solid #b0b0b0">
   <table summary=""> 
    <tr>
     <td><label for="vocation">{t}Vocation{/t}</label></td>
     <td><input id="vocation" name="vocation" size=22 maxlength=60 {$vocationACL} value="{$vocation}"></td>
    </tr>
    <tr>
     <td><label for="gouvernmentOrganizationalUnitDescription">{t}Unit description{/t}</label></td>
     <td>
      <input id="gouvernmentOrganizationalUnitDescription" name="gouvernmentOrganizationalUnitDescription" size=22 maxlength=60
	{$gouvernmentOrganizationalUnitDescriptionACL}  
	value="{$gouvernmentOrganizationalUnitDescription}">
     </td>
    </tr>
    <tr>
     <td><label for="gouvernmentOrganizationalUnitSubjectArea">{t}Subject area{/t}</label></td>
     <td>
      <input id="gouvernmentOrganizationalUnitSubjectArea" name="gouvernmentOrganizationalUnitSubjectArea" size=22 maxlength=60
          {$gouvernmentOrganizationalUnitSubjectAreaACL}  
           value="{$gouvernmentOrganizationalUnitSubjectArea}">
     </td>
    </tr>
    <tr>
     <td><label for="functionalTitle">{t}Functional title{/t}</label></td>
     <td>
      <input name="functionalTitle" id="functionalTitle"  size=22 maxlength=60 {$functionalTitleACL}
             value="{$functionalTitle}">
     </td>
    </tr>
    <tr>
     <td><label for="role">{t}Role{/t}</label></td>
     <td>
      <input id="role" name="role" size=22 maxlength=60 {$roleACL} value="{$role}">
     </td>
    </tr>
   </table>
  </td>
  
  <!-- Phone, ... -->
  <td style="vertical-align:top;border-right:1px solid #b0b0b0">
   <table summary=""> 
    <tr>
     <td><label for="gouvernmentOrganizationalPersonLocality">{t}Person locality{/t}</label></td>
     <td>
      <input id="gouvernmentOrganizationalPersonLocality" name="gouvernmentOrganizationalPersonLocality" size=22 maxlength=60
         {$gouvernmentOrganizationalPersonLocalityACL}
                value="{$gouvernmentOrganizationalPersonLocality}">
     </td>
    </tr>
    <tr>
     <td><label for="gouvernmentOrganizationalUnit">{t}Unit{/t}</label></td>
     <td><input id="gouvernmentOrganizationalUnit" name="gouvernmentOrganizationalUnit" size=22 maxlength=60 
     {$gouvernmentOrganizationalUnitACL}
            value="{$gouvernmentOrganizationalUnit}">
     </td>
    </tr>
    <tr>
     <td><label for="street">{t}Street{/t}</label></td>
     <td>
      <input name="street" id="street" size=22 maxlength=60 {$streetACL} value="{$street}">
     </td>
    </tr>
    <tr>
     <td><label for="postalCode">{t}Postal code{/t}</label></td>
     <td> 
      <input name="postalCode" id="postalCode" size=22 maxlength=60 {$postalCodeACL} value="{$postalCode}">
     </td>
    </tr>
    <tr>
     <td><label for="houseIdentifier">{t}House identifier{/t}</label></td>
     <td>
      <input id="houseIdentifier" name="houseIdentifier" size=22 maxlength=60 {$houseIdentifierACL}
             value="{$houseIdentifier}">
     </td>
    </tr>
   </table>
  </td>

  <!-- Location, ... -->
  <td style="vertical-align:top;">
   <table summary=""> 
    <tr>
     <td><label for="roomNumber">{t}Room No.{/t}</label></td>
     <td><input id="roomNumber" name="roomNumber" size=22 maxlength=60 {$roomNumberACL} value="{$roomNumber}"></td>
    </tr>
    <tr>
     <td><label for="telephoneNumber">{t}Phone{/t}</label></td>
     <td>
      {if $has_phoneaccount ne "true"}
      <input id="telephoneNumber" name="telephoneNumber" size=22 maxlength=60 {$telephoneNumberACL} value="{$telephoneNumber}">
      {else}
      <i>{t}Please use the phone tab{/t}</i>
      {/if}
     </td>
    </tr>
    <tr>
     <td><label for="facsimileTelephoneNumber">{t}Fax{/t}</label></td>
     <td>
      <input id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=22 maxlength=60 
	          {$facsimileTelephoneNumberACL}  
                 value="{$facsimileTelephoneNumber}">
     </td>
    </tr>
    <tr>
     <td><label for="ivbbLastDeliveryCollective">{t}Last delivery{/t}</label></td>
     <td>
      <input name="ivbbLastDeliveryCollective" size=22 maxlength=60 {$ivbbLastDeliveryCollectiveACL} id="ivbbLastDeliveryCollective" 
             value="{$ivbbLastDeliveryCollective}">
     </td>
    </tr>
    <tr>
     <td><label for="publicVisible">{t}Public visible{/t}</label></td>
     <td>
      <select size="1" id="publicVisible" name="publicVisible" {$publicVisibleACL}>
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
	focus_field('sn');
  	if (document.mainform.yearSel != "")
				populate(document.mainform,document.mainform.dateOfBirth);
  -->
</script>

