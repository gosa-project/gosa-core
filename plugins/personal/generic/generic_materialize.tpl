<h2>{t}Personal information{/t}</h2>

<div class="row content">
  <div class="col s12 xl2 center-align user-image">
    {if !$userPicture_is_readable}
    <img src="plugins/users/images/default.jpg" alt='' class="materialboxed">
    {else}
    <img src="getbin.php?rand={$rand}" alt='' class="materialboxed">
    {/if}
    {render acl=$userPictureACL checkbox=$multiple_support checked=$use_edit_picture}
    <button class="btn-small" type="submit" name="edit_picture">{t}Change picture{/t}...</button>
    {/render}
  </div>
  <div class="col s6 xl5">
    {if $is_template ne "true"}

    <div class="input-field">
      {if $multiple_support}
      <input type="text" name="dummy2" id="sn" value="{t}Multiple edit{/t}" disabled>
      {else}
      {render acl=$snACL}
      <input type="text" name="sn" id="sn" value="{$sn}">
      {/render}
      {/if}
      <label for="sn">{t}Last name{/t}{$must}</label>
    </div>

    <div class="input-field">
      {if $multiple_support}
      <input type="text" name="dummy3" id="givenName" value="{t}Multiple edit{/t}" disabled>
      {else}
      {render acl=$givenNameACL}
      <input type="text" name="givenName" id="givenName" value="{$givenName}">
      {/render}
      {/if}
      <label for="givenName">{t}First name{/t}{$must}</label>
    </div>

    <div class="input-field">
      {if $multiple_support}
      <input type="text" name="dummy1" id="uid" value="{t}Multiple edit{/t}" disabled>
      {else}
      {render acl=$uidACL}
      <input type="text" name="uid" id="uid" value="{$uid}">
      {/render}
      {/if}
      <label for="uid">{t}Login{/t}{$must}</label>
    </div>

    {else}

    <div class="input-field">
      {render acl=$uidACL}
      <input type="text" name="sn" id="sn" value="{$sn}">
      {/render}
      <label for="sn">{t}Template name{/t}{$must}</label>
    </div>

    {/if}

    <div class="input-field">
      {render acl=$personalTitleACL checkbox=$multiple_support checked=$use_personalTitle}
      <input type="text" id="personalTitle" name="personalTitle" maxlength=60 value="{$personalTitle}">
      {/render}
      <label for="personalTitle">{t}Personal title{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$academicTitleACL checkbox=$multiple_support checked=$use_academicTitle}
      <input type="text" id="academicTitle" name="academicTitle" maxlength=60 value="{$academicTitle}">
      {/render}
      <label for="academicTitle">{t}Academic title{/t}</label>
    </div>

    {if !$multiple_support}

    <div class="input-field">
      {render acl=$dateOfBirthACL}
      <input type="text" id="lang" value="{$lang}" hidden="true">
      <input class="datepicker" type="text" id="dateOfBirth" name="dateOfBirth" value="{$dateOfBirth}">
      {/render}
      <label for="dateOfBirth">{t}Date of birth{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$genderACL}
      <select size="2" name="gender" id="gender">
        {html_options options=$gender_list selected=$gender}
      </select>
      {/render}
      <label for="gender">{t}Sex{/t}</label>
    </div>

    {/if}

    <div class="input-field">
      {render acl=$preferredLanguageACL checkbox=$multiple_support checked=$use_preferredLanguage}
      <select size="1" id="preferredLanguage" name="preferredLanguage">
        {html_options options=$preferredLanguage_list selected=$preferredLanguage}
      </select>
      {/render}
      <label for="preferredLanguage">{t}Preferred language{/t}</label>
    </div>

    {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
    <div class="input-field ldap-tree">
      {$base}
    </div>
    {/render}

  </div>
  <div class="col s6 xl5">
    <div class="input-field">
      {render acl=$homePostalAddressACL checkbox=$multiple_support checked=$use_homePostalAddress}
      <textarea class="materialize-textarea" id="homePostalAddress" name="homePostalAddress">
        {$homePostalAddress}
      </textarea>
      {/render}
      <label for="homePostalAddress">{t}Address{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$homePhoneACL checkbox=$multiple_support checked=$use_homePhone}
      <input type="text" id="homePhone" name="homePhone" maxlength=60 value="{$homePhone}">
      {/render}
      <label for="homePhone">{t}Private phone{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$labeledURIACL checkbox=$multiple_support checked=$use_labeledURI}
      <input type="text" id="labeledURI" name="labeledURI" maxlength=60 value="{$labeledURI}">
      {/render}
      <label for="labeledURI">{t}Homepage{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$passwordStorageACL checkbox=$multiple_support checked=$use_pw_storage}
      <select size="1" id="pw_storage" name="pw_storage" onchange='document.mainform.submit()'>
        {foreach from=$pwmode key=value item=desc}
        <option value="{$value}" {if $pwmode_select==$value}selected{/if}>{$desc}</option>
        {/foreach}
        {if $pw_configurable eq "true"}
        <button id="edit_pw_method" type="submit" name="edit_pw_method">{t}Configure{/t}...</button>
        {/if}
      </select>
      {/render}
      <label for="pw_storage">{t}Password storage{/t}</label>
    </div>


    {if $is_template ne "true" && !$multiple_support}

    <div class="input-field certificate">
      {render acl=$CertificatesACL mode=read_active}
      <button class="btn-small" id="edit_cert" type="submit" name="edit_cert">{t}Edit certificates{/t}...</button>
      {/render}
    </div>

    {/if}

    {if !$multiple_support}

    <div class="input-field acl-member-wrapper">
      <label>{t}Restrict login to{/t}</label>
      {render acl=$gosaLoginRestrictionACL}
      {$gosaLoginRestrictionWidget}
      {/render}
    </div>

    <div class="input-field add">
      {render acl=$gosaLoginRestrictionACL}
      <input type="text" id="res" name="res" maxlength=33 onFocus='document.getElementById("res").value=""'>
      {/render}
      {render acl=$gosaLoginRestrictionACL}
      <button class="btn btn-small" id="add_res" type="submit" name="add_res">{t}Add{/t}</button>
      {/render}
      <label for="res">{t}IP or network{/t}</label>
    </div>

    {else}

    <div class="input-field">
      <input type='checkbox' name='use_gosaLoginRestriction' {if $use_gosaLoginRestriction} checked {/if}
        onClick='document.mainform.submit();'>
      <span> </span>
    </div>

    {if !$use_gosaLoginRestriction}

    {render acl=$gosaLoginRestriction_ONLY_R_ACL}
    {$gosaLoginRestrictionWidget}
    {/render}

    {else}

    {render acl=$gosaLoginRestrictionACL}
    {$gosaLoginRestrictionWidget}
    {/render}

    <div class="input-field">
      {render acl=$gosaLoginRestrictionACL}
      <input type="text" id="res" name="res" maxlength=33 value="{t}IP or network{/t}"
        onFocus='document.getElementById("res").value=""'>
      {/render}
      {render acl=$gosaLoginRestrictionACL}
      <button class="btn-small gonicus-color" id="add_res" type="submit" name="add_res">{t}Add{/t}</button>
      {/render}
      {/if}
      <label for="res">{t}Restrict login to{/t}</label>
    </div>

    {/if}
  </div>
</div>

<hr class="divider">

<h3>{t}Organizational information{/t}</h3>

<div class="row">
  {if $governmentmode ne "true"}
  <div class="col s4">
    <div class="input-field">
      {render acl=$oACL checkbox=$multiple_support checked=$use_o}
      <input type="text" id="o" name="o" maxlength=60 value="{$o}">
      {/render}
      <label for="o">{t}Organization{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$ouACL checkbox=$multiple_support checked=$use_ou}
      <input type="text" id="ou" name="ou" maxlength=60 value="{$o}">
      {/render}
      <label for="ou">{t}Department{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$departmentNumberACL checkbox=$multiple_support checked=$use_departmentNumber}
      <input type="text" id="departmentNumber" name="departmentNumber" maxlength=60 value="{$departmentNumber}">
      {/render}
      <label for="departmentNumber">{t}Department No.{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$employeeNumberACL checkbox=$multiple_support checked=$use_employeeNumber}
      <input type="text" id="employeeNumber" name="employeeNumber" maxlength=60 value="{$employeeNumber}">
      {/render}
      <label for="employeeNumber">{t}Employee No.{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$employeeTypeACL checkbox=$multiple_support checked=$use_employeeType}
      <input type="text" id="employeeType" name="employeeType" maxlength=60 value="{$employeeType}">
      {/render}
      <label for="employeeType">{t}Employee type{/t}</label>
    </div>
  </div>

  <div class="col s4">
    {if !$multiple_support}
    <div class="input-field manager">
      {render acl=$managerACL}
      <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'>
      {/render}
      <label for="manager_name">{t}Manager{/t}</label>
      <div class="icons">
        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
          {/render}
        </div>

        {if $manager!=""}
        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
          {/render}
        </div>

        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons input-icons tooltipped' data-postion='bottom'
            data-tooltip='{$manager}'>info</i>" title="{$manager}"}
          {/render}
        </div>
        {else}
        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons md-disabled'>delete</i>"}
          {/render}
        </div>

        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons tooltipped md-disabled context' data-postion='bottom'
            data-tooltip='{t}Manager{/t}'>info</i>"}
          {/render}
        </div>
        {/if}
      </div>
    </div>
    {else}
    <div class="input-field manager">
      <div class="icons prefix">
        <input type='checkbox' value="1" name="use_manager" id="use_manager" {if $use_manager} checked {/if}
          onClick="document.mainform.submit();">
      </div>
      <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'>
      <label for="manager_name">{t}Manager{/t}</label>
      {if $use_manager}
      <div class="icons">
        <div class="icon">
          {image path="<i class='material-icons'>edit</i>" action="editManager" acl=$managerACL}
        </div>
        {if $manager!=""}
        <div class="icon">
          {image path="<i class='material-icons'>info</i>" title="{$manager}" acl=$managerACL}
        </div>
        <div class="icon">
          {image path="<i class='material-icons'>delete</i>" action="removeManager" acl=$managerACL}
        </div>

        {else}
        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons md-disabled'>delete</i>"}
          {/render}
        </div>

        <div class="icon">
          {render acl=$managerACL}
          {image path="<i class='material-icons tooltipped md-disabled context' data-postion='bottom'
            data-tooltip='{t}Manager{/t}'>info</i>"}
          {/render}
        </div>
        {/if}
      </div>
      {/if}
    </div>
    {/if}

    <div class="input-field">
      {render acl=$roomNumberACL checkbox=$multiple_support checked=$use_roomNumber}
      <input type="text" id="roomNumber" name="roomNumber" maxlength=60 value="{$roomNumber}">
      {/render}
      <label for="roomNumber">{t}Room No.{/t}</label>
    </div>

    {if $has_phoneaccount ne "true"}

    <div class="input-field">
      {render acl=$telephoneNumberACL checkbox=$multiple_support checked=$use_telephoneNumber}
      <input type="text" id="telephoneNumber" name="telephoneNumber" maxlength=60 value="{$telephoneNumber}">
      {/render}
      <label for="telephoneNumber">{t}Phone{/t}</label>
    </div>

    {/if}

    <div class="input-field">
      {render acl=$mobileACL checkbox=$multiple_support checked=$use_mobile}
      <input type="text" id="mobile" name="mobile" maxlength=60 value="{$mobile}">
      {/render}
      <label for="mobile">{t}Mobile{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$pagerACL checkbox=$multiple_support checked=$use_pager}
      <input type="text" id="pager" name="pager" maxlength=60 value="{$pager}">
      {/render}
      <label for="pager">{t}Pager{/t}</label>
    </div>
  </div>
  <div class="col s4">
    <div class="input-field">
      {render acl=$facsimileTelephoneNumberACL checkbox=$multiple_support checked=$use_facsimileTelephoneNumber}
      <input type="text" id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" maxlength=60
        value="{$facsimileTelephoneNumber}">
      {/render}
      <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$lACL checkbox=$multiple_support checked=$use_l}
      <input type="text" id="l" name="l" maxlength=60 value="{$l}">
      {/render}
      <label for="l">{t}Location{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$stACL checkbox=$multiple_support checked=$use_st}
      <input type="text" id="st" name="st" maxlength=60 value="{$st}">
      {/render}
      <label for="st">{t}State{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$postalAddressACL checkbox=$multiple_support checked=$use_postalAddress}
      <textarea class="materialize-textarea" id="postalAddress" name="postalAddress">
        {$postalAddress}
      </textarea>
      {/render}
      <label for="postalAdress">{t}Address{/t}</label>
    </div>

  </div>
  {else}
  <div class="col s4">
    <div class="input-field">
      {render acl=$vocationACL checkbox=$multiple_support checked=$use_vocation}
      <input type="text" id="vocation" name="vocation" maxlength=60 value="{$vocation}">
      {/render}
      <label for="vocation">{t}Vocation{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$gouvernmentOrganizationalUnitDescriptionACL checkbox=$multiple_support
      checked=$use_gouvernmentOrganizationalUnitDescription}
      <input type="text" id="gouvernmentOrganizationalUnitDescription" name="gouvernmentOrganizationalUnitDescription"
        maxlength=60 value="{$gouvernmentOrganizationalUnitDescription}">
      {/render}
      <label for="gouvernmentOrganizationalUnitDescription">{t}Unit description{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$gouvernmentOrganizationalUnitSubjectAreaACL checkbox=$multiple_support
      checked=$use_gouvernmentOrganizationalUnitSubjectArea}
      <input type="text" id="gouvernmentOrganizationalUnitSubjectArea" name="gouvernmentOrganizationalUnitSubjectArea"
        maxlength=60 value="{$gouvernmentOrganizationalUnitSubjectArea}">
      {/render}
      <label for="gouvernmentOrganizationalUnitSubjectArea">{t}Subject area{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$functionalTitleACL checkbox=$multiple_support checked=$use_functionalTitle}
      <input type="text" name="functionalTitle" id="functionalTitle" maxlength=60 value="{$functionalTitle}">
      {/render}
      <label for="functionalTitle">{t}Functional title{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$roleACL checkbox=$multiple_support checked=$use_role}
      <input type="text" id="role" name="role" maxlength=60 value="{$role}">
      {/render}
      <label for="role">{t}Role{/t}</label>
    </div>

  </div>
  <div class="col s4">
    <div class="input-field">
      {render acl=$gouvernmentOrganizationalPersonLocalityACL checkbox=$multiple_support
      checked=$use_gouvernmentOrganizationalPersonLocality}
      <input type="text" id="gouvernmentOrganizationalPersonLocality" name="gouvernmentOrganizationalPersonLocality"
        maxlength=60 value="{$gouvernmentOrganizationalPersonLocality}">
      {/render}
      <label for="gouvernmentOrganizationalPersonLocality">{t}Person locality{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$gouvernmentOrganizationalUnitACL checkbox=$multiple_support
      checked=$use_gouvernmentOrganizationalUnit}
      <input type="text" id="gouvernmentOrganizationalUnit" name="gouvernmentOrganizationalUnit" maxlength=60
        value="{$gouvernmentOrganizationalUnit}">
      {/render}
      <label for="gouvernmentOrganizationalUnit">{t}Unit{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$streetACL checkbox=$multiple_support checked=$use_street}
      <input type="text" name="street" id="street" maxlength=60 value="{$street}">
      {/render}
      <label for="street">{t}Street{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$postalCodeACL checkbox=$multiple_support checked=$use_postalCode}
      <input type="text" name="postalCode" id="postalCode" maxlength=60 value="{$postalCode}">
      {/render}
      <label for="postalCode">{t}Postal code{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$houseIdentifierACL checkbox=$multiple_support checked=$use_houseIdentifier}
      <input type="text" id="houseIdentifier" name="houseIdentifier" maxlength=60 value="{$houseIdentifier}">
      {/render}
      <label for="houseIdentifier">{t}House identifier{/t}</label>
    </div>

  </div>
  <div class="col s4">
    <div class="input-field">
      {render acl=$roomNumberACL checkbox=$multiple_support checked=$use_roomNumber}
      <input type="text" id="roomNumber" name="roomNumber" maxlength=60 value="{$roomNumber}">
      {/render}
      <label for="roomNumber">{t}Room No.{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$telephoneNumberACL checkbox=$multiple_support checked=$use_telephoneNumber}
      {if $has_phoneaccount ne "true"}
      <input type="text" id="telephoneNumber" name="telephoneNumber" maxlength=60 value="{$telephoneNumber}">
      {else}
      <i>{t}Please use the phone tab{/t}</i>
      {/if}
      {/render}
      <label for="telephoneNumber">{t}Phone{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$facsimileTelephoneNumberACL checkbox=$multiple_support checked=$use_facsimileTelephoneNumber}
      <input type="text" id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" maxlength=60
        value="{$facsimileTelephoneNumber}">
      {/render}
      <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$ivbbLastDeliveryCollectiveACL checkbox=$multiple_support checked=$use_ivbbLastDeliveryCollective}
      <input type="text" name="ivbbLastDeliveryCollective" maxlength=60 id="ivbbLastDeliveryCollective"
        value="{$ivbbLastDeliveryCollective}">
      {/render}
      <label for="ivbbLastDeliveryCollective">{t}Last delivery{/t}</label>
    </div>

    <div class="input-field">
      {render acl=$publicVisibleACL checkbox=$multiple_support checked=$use_publicVisible}
      <select size="1" id="publicVisible" name="publicVisible">
        {html_options values=$ivbbmodes output=$ivbbmodes selected=$publicVisible}
      </select>
      {/render}
      <label for="publicVisible">{t}Public visible{/t}</label>
    </div>

  </div>
  {/if}
</div>


{if $multiple_support}
<input type="hidden" name="user_mulitple_edit" value="1">
{/if}
<input type=hidden name="generic">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
  focus_field('sn');
</script>
