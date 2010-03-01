<h3>{t}Personal information{/t}</h3>

<div class="container">

  <div class="container-element">
    <div class="fieldset-left">
      <fieldset>
{if !$userPicture_is_readable}
			  <img class='center' border="0" src="plugins/users/images/default.jpg" alt="{t}Personal picture{/t}">
{else}
			  <img class='center' border="0" src="getbin.php?rand={$rand}" alt="{t}Personal picture{/t}">
{/if}
        <br>
        {render acl=$userPictureACL checkbox=$multiple_support checked=$use_edit_picture}
          <button type="submit" name="edit_picture">{t}Change picture{/t}...</button>
        {/render}
      </fieldset>
    </div>
  </div>

  <div class="container-element" style='width:30%;'>
    <div class="fieldset-middle">
      <legend>{t}Generic{/t}</legend>
      <fieldset>
{if $is_template eq "true"}

        <label for="sn">{t}Template name{/t}{$must}</label>
        {render acl=$snACL}
          <input type="text" id="sn" name="sn" value="{$sn}">
        {/render}
{else}

 {if $multiple_support}

        <label for="sn">{t}Last name{/t}{$must}</label>
	      <input type="text" id="sn" name="dummy2" value="{t}Multiple edit{/t}" disabled>

        <label for="givenName">{t}Given name{/t}{$must}</label>
	      <input type="text" id="givenName" name="dummy3" value="{t}Multiple edit{/t}" disabled>

        <label for="uid">{t}Uid{/t}{$must}</label>
			  <input type="text" id="uid" name="dummy1" value="{t}Multiple edit{/t}" disabled>

 {else}

        <label for="sn">{t}Last name{/t}{$must}</label>
        {render acl=$snACL}
  	      <input type="text" id="sn" name="sn" value="{$sn}">
        {/render}

        <label for="givenName">{t}Given name{/t}{$must}</label>
        {render acl=$givenNameACL}
  	      <input type="text" id="givenName" name="givenName" value="{$givenName}">
        {/render}

        <label for="uid">{t}Uid{/t}{$must}</label>
        {render acl=$uidACL}
  	      <input type="text" id="uid" name="uid" value="{$uid}">
        {/render}

 {/if}
{/if}

      <div style="height:10px;"></div>

      <label for="personalTitle">{t}Personal title{/t}</label>
      {render acl=$personalTitleACL checkbox=$multiple_support checked=$use_personalTitle}
		    <input type="text" id="personalTitle" name="personalTitle" value="{$personalTitle}">
      {/render}
      <label for="academicTitle">{t}Academic title{/t}</label>
      {render acl=$academicTitleACL checkbox=$multiple_support checked=$use_academicTitle}
	      <input type="text" id="academicTitle" name="academicTitle" value="{$academicTitle}">
      {/render}

      <div style="height:10px;"></div>

{if !$multiple_support}

      <label for="day">{t}Date of birth{/t}</label>
      {render acl=$dateOfBirthACL}
      <input type="text" id="dateOfBirth" name="dateOfBirth" class="date"  value="{$dateOfBirth}">
      {if $dateOfBirthACL|regex_replace:"/[cdmr]/":"" == "w"}
        <script type="text/javascript">
          {literal}
          var datepicker  = new DatePicker({ relative : 'dateOfBirth', language : '{/literal}{$lang}{literal}', keepFieldEmpty : true,
                                             enableCloseEffect : false, enableShowEffect : false });
          {/literal}
        </script>
        {/if}
      {/render}

      <label for="gender">{t}Sex{/t}</label>
      {render acl=$genderACL}
	      <select size="1" id="gender" name="gender">
	       {html_options options=$gender_list selected=$gender}
	      </select>
      {/render}

{/if}

      <label for="preferredLanguage">{t}Preferred langage{/t}</label>
      {render acl=$preferredLanguageACL checkbox=$multiple_support checked=$use_preferredLanguage}
	      <select size="1" id="preferredLanguage" name="preferredLanguage">
	       {html_options options=$preferredLanguage_list selected=$preferredLanguage}
	      </select>
      {/render}

      <label for="base">{t}Base{/t}</label>
      <div style="height:10px;"></div>
      {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
        {$base}
      {/render}

      </fieldset>
    </div>
  </div>

  <div class="container-element" style='width:50%;'>
    <div class="fieldset-right">

      <fieldset>

      <label for="homePostalAddress">{t}Address{/t}</label>
      {render acl=$homePostalAddressACL checkbox=$multiple_support checked=$use_homePostalAddress}
	      <textarea id="homePostalAddress" name="homePostalAddress" rows="3" style="width:100%">{$homePostalAddress}</textarea>
      {/render}
     
      <label for="homePhone">{t}Private phone{/t}</label>
      {render acl=$homePhoneACL checkbox=$multiple_support checked=$use_homePhone}
	      <input type="text" id="homePhone" name="homePhone" size=25 maxlength=60 value="{$homePhone}">
      {/render}
     
      <label for="labeledURI">{t}Homepage{/t}</label>
      {render acl=$labeledURIACL checkbox=$multiple_support checked=$use_labeledURI}
        <input type="text" id="labeledURI" name="labeledURI" size=25 maxlength=60 value="{$labeledURI}">
      {/render}
     
      <div style="height:10px;"></div>

      <label for="pw_storage">{t}Password storage{/t}</label>
      {render acl=$passwordStorageACL checkbox=$multiple_support checked=$use_pw_storage}
	      <select size="1" id="pw_storage" name="pw_storage" onchange='document.mainform.submit()'>
          {foreach from=$pwmode key=value item=desc}
            <option value="{$value}" {if $pwmode_select == $value}selected{/if} >{$desc}</option>
          {/foreach}
	      </select>
      {/render}
      {render acl=$passwordStorageACL checkbox=$multiple_support checked=$use_pw_storage}
      	{if $pw_configurable eq "true"}
	        &nbsp; <button id="edit_pw_method" type="submit" name="edit_pw_method">{t}Configure{/t}...</button>
	      {/if}
      {/render}
     
      {if $is_template ne "true" && !$multiple_support}
        <label for="edit_cert">{t}Certificates{/t}</label>
        {render acl=$CertificatesACL mode=read_active}
          <button id="edit_cert" type="submit" name="edit_cert">{t}Edit certificates{/t}...</button>
        {/render}
      {/if}

      <div style="height:10px;"></div>
     
      {if !$multiple_support}
        {render acl=$gosaLoginRestrictionACL}
          {$gosaLoginRestrictionWidget}
        {/render}
        {render acl=$gosaLoginRestrictionACL}
        <input type="text" id="res" name="res" size=22 maxlength=33 
          value="{t}IP or network{/t}" onFocus='document.getElementById("res").value=""'>
        {/render}
        {render acl=$gosaLoginRestrictionACL}
          <button id="add_res" type="submit" name="add_res">{t}Add{/t}<button>
        {/render}
      
      {else}

        <input type='checkbox' name='use_gosaLoginRestriction' {if $use_gosaLoginRestriction} checked {/if}
          onClick='document.mainform.submit();'>
        {if !$use_gosaLoginRestriction}
          {render acl=$gosaLoginRestriction_ONLY_R_ACL}
            {$gosaLoginRestrictionWidget}
          {/render}
        {else}
          {render acl=$gosaLoginRestrictionACL}
            {$gosaLoginRestrictionWidget}
          {/render}
          {render acl=$gosaLoginRestrictionACL}
            <input type="text" id="res" name="res" size=22 maxlength=33 
              value="{t}IP or network{/t}" onFocus='document.getElementById("res").value=""'>
          {/render}
          {render acl=$gosaLoginRestrictionACL}
            <input id="add_res" type="submit" name="add_res" value="{t}Add{/t}">
          {/render}
        {/if}
      {/if}

      </fieldset>
    </div>
  </div>
</div>

<div class='clear'></div>

<!--

<p class="seperator">&nbsp;</p>



 <!-- Headline container -->
 
  
   <h2>{t}Organizational information{/t}</h2>
  
 

 <!-- Base 2 container -->
 

  {if $governmentmode ne "true"}
  <!-- Company, ... -->
  
    
    
     <label for="o">{t}Organization{/t}</label>
     	
{render acl=$oACL checkbox=$multiple_support checked=$use_o}
		<input type="text" id="o" name="o" size=22 maxlength=60 value="{$o}">
{/render}
     
    
    
     <label for="ou">{t}Department{/t}</label>
     
{render acl=$ouACL checkbox=$multiple_support checked=$use_ou}
		<input type="text" id="ou" name="ou" size=22 maxlength=60 value="{$ou}">
{/render}
     
    
    
     <label for="departmentNumber">{t}Department No.{/t}</label>
     
{render acl=$departmentNumberACL checkbox=$multiple_support checked=$use_departmentNumber}
	      <input type="text" id="departmentNumber" name="departmentNumber" size=22 maxlength=60 value="{$departmentNumber}">
{/render}
     
    
    
     <label for="employeeNumber">{t}Employee No.{/t}</label>
     
{render acl=$employeeNumberACL checkbox=$multiple_support checked=$use_employeeNumber}
      		<input type="text" id="employeeNumber" name="employeeNumber" size=22 maxlength=60 value="{$employeeNumber}">
{/render}
     
    
    
     <label for="employeeType">{t}Employee type{/t}</label>
     
{render acl=$employeeTypeACL checkbox=$multiple_support checked=$use_employeeType}
      		<input type="text" id="employeeType" name="employeeType" size=22 maxlength=60 value="{$employeeType}">
{/render}
     
    
    {if !$multiple_support}
    
     <label for="manager">{t}Manager{/t}</label>
     
{render acl=$managerACL}
        <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled
          title='{$manager}'>
{/render}
{render acl=$managerACL}
        <input type='image' src='images/lists/edit.png' name='editManager' class='center'>
{/render}
        {if $manager!=""}
{render acl=$managerACL}
        <img src='images/info_small.png' title='{$manager}' class='center'>
{/render}
{render acl=$managerACL}
        <input type='image' src='images/lists/trash.png' name='removeManager' class='center'>
{/render}
        {/if}
     
    
    {else}
    
     <label for="manager">{t}Manager{/t}</label>
     
      <input type='checkbox' value="1" name="use_manager" id="use_manager" {if $use_manager} checked {/if}
        onClick="document.mainform.submit();">
        <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled
          title='{$manager}'>
        {if $use_manager}
        <input type='image' src='images/lists/edit.png' name='editManager' class='center'>
        {if $manager!=""}
        <img src='images/info_small.png' title='{$manager}' class='center'>
        <input type='image' src='images/lists/trash.png' name='removeManager' class='center'>
        {/if}
        {/if}
     
    
  
    {/if}
   
  
   
  
   &nbsp;
  

  <!-- Phone, ... -->
  
    
    
     <label for="roomNumber">{t}Room No.{/t}</label>
     
{render acl=$roomNumberACL checkbox=$multiple_support checked=$use_roomNumber}
		<input type="text" id="roomNumber" name="roomNumber" size=22 maxlength=60 value="{$roomNumber}">
{/render}
     
    
    {if $has_phoneaccount ne "true"}
    
     <label for="telephoneNumber">{t}Phone{/t}</label>
     
{render acl=$telephoneNumberACL checkbox=$multiple_support checked=$use_telephoneNumber}
      		<input type="text" id="telephoneNumber" name="telephoneNumber" size=22 maxlength=60 value="{$telephoneNumber}">
{/render}
     
    
    {/if}
    
     <label for="mobile">{t}Mobile{/t}</label>
     
{render acl=$mobileACL checkbox=$multiple_support checked=$use_mobile}
		<input type="text" id="mobile" name="mobile" size=22 maxlength=60 value="{$mobile}">
{/render}
     
    
    
     <label for="pager">{t}Pager{/t}</label>
     
{render acl=$pagerACL checkbox=$multiple_support checked=$use_pager}
      		<input type="text" id="pager" name="pager" size=22 maxlength=60 value="{$pager}">
{/render}
     
    
    
     <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
     
{render acl=$facsimileTelephoneNumberACL checkbox=$multiple_support checked=$use_facsimileTelephoneNumber}
		<input type="text" id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=22 maxlength=60 value="{$facsimileTelephoneNumber}">
{/render}
     
    
   
  

  
   &nbsp;
  

  <!-- Location, ... -->
  
    
    
     <label for="l">{t}Location{/t}</label>
     
{render acl=$lACL checkbox=$multiple_support checked=$use_l}
		<input type="text" id="l" name="l" size=22 maxlength=60 value="{$l}">
{/render}
     
    
    
     <label for="st">{t}State{/t}</label>
     
{render acl=$stACL checkbox=$multiple_support checked=$use_st}
		<input type="text" id="st" name="st" size=22 maxlength=60 value="{$st}">
{/render}
     
    
    
     
     
{render acl=$postalAddressACL checkbox=$multiple_support checked=$use_postalAddress}
      		<textarea id="postalAddress" name="postalAddress" rows=3 style="width:100%">{$postalAddress}</textarea>
{/render}
     
    
   
  

  {else} 

  <!-- Beschreibung, ... -->
  
    
    
     <label for="vocation">{t}Vocation{/t}</label>
     
{render acl=$vocationACL checkbox=$multiple_support checked=$use_vocation}
		<input type="text" id="vocation" name="vocation" size=22 maxlength=60 value="{$vocation}">
{/render}
     
    
    
     <label for="gouvernmentOrganizationalUnitDescription">{t}Unit description{/t}</label>
     
{render acl=$gouvernmentOrganizationalUnitDescriptionACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalUnitDescription}
      		<input type="text" id="gouvernmentOrganizationalUnitDescription" name="gouvernmentOrganizationalUnitDescription" size=22 maxlength=60
		value="{$gouvernmentOrganizationalUnitDescription}">
{/render}
     
    
    
     <label for="gouvernmentOrganizationalUnitSubjectArea">{t}Subject area{/t}</label>
     
{render acl=$gouvernmentOrganizationalUnitSubjectAreaACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalUnitSubjectArea}
	      	<input type="text" id="gouvernmentOrganizationalUnitSubjectArea" name="gouvernmentOrganizationalUnitSubjectArea" size=22 maxlength=60
           	value="{$gouvernmentOrganizationalUnitSubjectArea}">
{/render}
     
    
    
     <label for="functionalTitle">{t}Functional title{/t}</label>
     
{render acl=$functionalTitleACL checkbox=$multiple_support checked=$use_functionalTitle}
      		<input type="text" name="functionalTitle" id="functionalTitle"  size=22 maxlength=60
             	value="{$functionalTitle}">
{/render}
     
    
    
     <label for="role">{t}Role{/t}</label>
     
{render acl=$roleACL checkbox=$multiple_support checked=$use_role}
      		<input type="text" id="role" name="role" size=22 maxlength=60 value="{$role}">
{/render}
     
    
   
  
  
  <!-- Phone, ... -->
  
    
    
     <label for="gouvernmentOrganizationalPersonLocality">{t}Person locality{/t}</label>
     
{render acl=$gouvernmentOrganizationalPersonLocalityACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalPersonLocality}
      		<input type="text" id="gouvernmentOrganizationalPersonLocality" name="gouvernmentOrganizationalPersonLocality" size=22 maxlength=60
                value="{$gouvernmentOrganizationalPersonLocality}">
{/render}
     
    
    
     <label for="gouvernmentOrganizationalUnit">{t}Unit{/t}</label>
     
{render acl=$gouvernmentOrganizationalUnitACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalUnit}
		<input type="text" id="gouvernmentOrganizationalUnit" name="gouvernmentOrganizationalUnit" size=22 maxlength=60 
            	value="{$gouvernmentOrganizationalUnit}">
{/render}
     
    
    
     <label for="street">{t}Street{/t}</label>
     
{render acl=$streetACL checkbox=$multiple_support checked=$use_street}
      		<input type="text" name="street" id="street" size=22 maxlength=60 value="{$street}">
{/render}
     
    
    
     <label for="postalCode">{t}Postal code{/t}</label>
      
{render acl=$postalCodeACL checkbox=$multiple_support checked=$use_postalCode}
      		<input type="text" name="postalCode" id="postalCode" size=22 maxlength=60 value="{$postalCode}">
{/render}
     
    
    
     <label for="houseIdentifier">{t}House identifier{/t}</label>
     
{render acl=$houseIdentifierACL checkbox=$multiple_support checked=$use_houseIdentifier}
      		<input type="text" id="houseIdentifier" name="houseIdentifier" size=22 maxlength=60
             	value="{$houseIdentifier}">
{/render}
     
    
   
  

  <!-- Location, ... -->
  
    
    
     <label for="roomNumber">{t}Room No.{/t}</label>
     
{render acl=$roomNumberACL checkbox=$multiple_support checked=$use_roomNumber}
		<input type="text" id="roomNumber" name="roomNumber" size=22 maxlength=60 value="{$roomNumber}">
{/render}
     
    
    
     <label for="telephoneNumber">{t}Phone{/t}</label>
     
{render acl=$telephoneNumberACL checkbox=$multiple_support checked=$use_telephoneNumber}
	      {if $has_phoneaccount ne "true"}
			<input type="text" id="telephoneNumber" name="telephoneNumber" size=22 maxlength=60 value="{$telephoneNumber}">
	      {else}
			<i>{t}Please use the phone tab{/t}</i>
	      {/if}
{/render}
     
    
    
     <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
     
{render acl=$facsimileTelephoneNumberACL checkbox=$multiple_support checked=$use_facsimileTelephoneNumber}
      		<input type="text" id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=22 maxlength=60 
                value="{$facsimileTelephoneNumber}">
{/render}
     
    
    
     <label for="ivbbLastDeliveryCollective">{t}Last delivery{/t}</label>
     
{render acl=$ivbbLastDeliveryCollectiveACL checkbox=$multiple_support checked=$use_ivbbLastDeliveryCollective}
      		<input type="text" name="ivbbLastDeliveryCollective" size=22 maxlength=60 id="ivbbLastDeliveryCollective" 
             	value="{$ivbbLastDeliveryCollective}">
{/render}
     
    
    
     <label for="publicVisible">{t}Public visible{/t}</label>
     
{render acl=$publicVisibleACL checkbox=$multiple_support checked=$use_publicVisible}
	      <select size="1" id="publicVisible" name="publicVisible">
	       {html_options values=$ivbbmodes output=$ivbbmodes selected=$publicVisible}
	      </select>
{/render}
     
    
   
  
 {/if} 
 


{if $multiple_support}
	<input type="hidden" name="user_mulitple_edit" value="1">
{/if}
<input type=hidden name="generic">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('sn');
  -->
</script>
-->
