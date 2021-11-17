<h2>{t}Personal information{/t}</h2>

<div class="row content">
    <div class="col s12 xl2 center-align user-image">
        {if !$userPicture_is_readable}
        <img src="plugins/users/images/default.jpg" alt='' class="materialboxed"> {else}
        <img src="getbin.php?rand={$rand}" alt='' class="materialboxed"> {/if} {render acl=$userPictureACL checkbox=$multiple_support checked=$use_edit_picture}
        <button class="btn-small" type="submit" name="edit_picture">{t}Change picture{/t}...</button> {/render}
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

        {render acl=$uidACL}
        <div class="input-field">
            <input type="text" name="sn" id="sn" value="{$sn}">
            <label for="sn">{t}Template name{/t}{$must}</label>
        </div>
        {/render}

        {/if}

        {render acl=$personalTitleACL checkbox=$multiple_support checked=$use_personalTitle}
        <div class="input-field">
            <input type="text" id="personalTitle" name="personalTitle" maxlength=60 value="{$personalTitle}">
            <label for="personalTitle">{t}Personal title{/t}</label>
        </div>
        {/render}

        {render acl=$academicTitleACL checkbox=$multiple_support checked=$use_academicTitle}
        <div class="input-field">
            <input type="text" id="academicTitle" name="academicTitle" maxlength=60 value="{$academicTitle}">
            <label for="academicTitle">{t}Academic title{/t}</label>
        </div>
        {/render}

        {if !$multiple_support}

        {render acl=$dateOfBirthACL}
        <div class="input-field">
            <input type="text" id="lang" value="{$lang}" hidden="true">
            <input class="datepicker" type="text" id="dateOfBirth" name="dateOfBirth" value="{$dateOfBirth}">
            <label for="dateOfBirth">{t}Date of birth{/t}</label>
        </div>
        {/render}

        {render acl=$genderACL}
        <div class="input-field">
            <select size="2" name="gender" id="gender">
                {html_options options=$gender_list selected=$gender}
            </select>
            <label for="gender">{t}Sex{/t}</label>
        </div>
        {/render}

        {/if}

        {render acl=$preferredLanguageACL checkbox=$multiple_support checked=$use_preferredLanguage}
        <div class="input-field">
            <select size="1" id="preferredLanguage" name="preferredLanguage">
                {html_options options=$preferredLanguage_list selected=$preferredLanguage}
            </select>
            <label for="preferredLanguage">{t}Preferred language{/t}</label>
        </div>
        {/render}

        {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
        <div class="input-field ldap-tree">
            {$base}
        </div>
        {/render}

    </div>
    <div class="col s6 xl5">
        {render acl=$homePostalAddressACL checkbox=$multiple_support checked=$use_homePostalAddress}
        <div class="input-field">
            <textarea class="materialize-textarea" id="homePostalAddress" name="homePostalAddress">
                {$homePostalAddress}
            </textarea>
            <label for="homePostalAddress">{t}Address{/t}</label>
        </div>
        {/render}

        {render acl=$homePhoneACL checkbox=$multiple_support checked=$use_homePhone}
        <div class="input-field">
            <input type="text" id="homePhone" name="homePhone" maxlength=60 value="{$homePhone}">
            <label for="homePhone">{t}Private phone{/t}</label>
        </div>
        {/render}

        {render acl=$labeledURIACL checkbox=$multiple_support checked=$use_labeledURI}
        <div class="input-field">
            <input type="text" id="labeledURI" name="labeledURI" maxlength=60 value="{$labeledURI}">
            <label for="labeledURI">{t}Homepage{/t}</label>
        </div>
        {/render}

        {render acl=$passwordStorageACL checkbox=$multiple_support checked=$use_pw_storage}
        <div class="input-field">
            <select size="1" id="pw_storage" name="pw_storage" onchange='document.mainform.submit()'>
                {foreach from=$pwmode key=value item=desc}
                <option value="{$value}" {if $pwmode_select==$value}selected{/if}>{$desc}</option>
                {/foreach}

                {if $pw_configurable eq "true"}
                <button id="edit_pw_method" type="submit" name="edit_pw_method">{t}Configure{/t}...</button>
                {/if}
            </select>
            <label for="pw_storage">{t}Password storage{/t}</label>
        </div>
        {/render}


        {if $is_template ne "true" && !$multiple_support}

        {render acl=$CertificatesACL mode=read_active}
        <div class="input-field certificate">
            <button class="btn-small" id="edit_cert" type="submit" name="edit_cert">{t}Edit certificates{/t}...</button>
        </div>
        {/render}

        {/if} {if !$multiple_support}

        <div class="input-field acl-member-wrapper">
            <label>{t}Restrict login to{/t}</label>

            {render acl=$gosaLoginRestrictionACL}
            {$gosaLoginRestrictionWidget}
            {/render}

        </div>

        {render acl=$gosaLoginRestrictionACL}
        <div class="input-field add">
            <input type="text" id="res" name="res" maxlength=33 onFocus='document.getElementById("res").value=""'>
            <button class="btn btn-small" id="add_res" type="submit" name="add_res">{t}Add{/t}</button>
            <label for="res">{t}IP or network{/t}</label>
        </div>
        {/render}

        {else}

        <div class="input-field">
            <input type='checkbox' name='use_gosaLoginRestriction' {if $use_gosaLoginRestriction} checked {/if} onClick='document.mainform.submit();'>
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

        {render acl=$gosaLoginRestrictionACL}
        <div class="input-field">
            <input type="text" id="res" name="res" maxlength=33 value="{t}IP or network{/t}" onFocus='document.getElementById("res").value=""'>
            <button class="btn-small primary" id="add_res" type="submit" name="add_res">{t}Add{/t}</button>
            <label for="res">{t}Restrict login to{/t}</label>
        </div>
        {/render}
        {/if}

        {/if}
    </div>
</div>

<hr class="divider">

<h3>{t}Organizational information{/t}</h3>

<div class="row">
    {if $governmentmode ne "true"}
    <div class="col s4">
        {render acl=$oACL checkbox=$multiple_support checked=$use_o}
        <div class="input-field">
            <input type="text" id="o" name="o" maxlength=60 value="{$o}">
            <label for="o">{t}Organization{/t}</label>
        </div>
        {/render}

        {render acl=$ouACL checkbox=$multiple_support checked=$use_ou}
        <div class="input-field">
            <input type="text" id="ou" name="ou" maxlength=60 value="{$o}">
            <label for="ou">{t}Department{/t}</label>
        </div>
        {/render}

        {render acl=$departmentNumberACL checkbox=$multiple_support checked=$use_departmentNumber}
        <div class="input-field">
            <input type="text" id="departmentNumber" name="departmentNumber" maxlength=60 value="{$departmentNumber}">
            <label for="departmentNumber">{t}Department No.{/t}</label>
        </div>
        {/render}

        {render acl=$employeeNumberACL checkbox=$multiple_support checked=$use_employeeNumber}
        <div class="input-field">
            <input type="text" id="employeeNumber" name="employeeNumber" maxlength=60 value="{$employeeNumber}">
            <label for="employeeNumber">{t}Employee No.{/t}</label>
        </div>
        {/render}

        {render acl=$employeeTypeACL checkbox=$multiple_support checked=$use_employeeType}
        <div class="input-field">
            <input type="text" id="employeeType" name="employeeType" maxlength=60 value="{$employeeType}">
            <label for="employeeType">{t}Employee type{/t}</label>
        </div>
        {/render}
    </div>

    <div class="col s4">
        {if !$multiple_support}
        {render acl=$managerACL}
        <div class="input-field manager">
            <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'> {/render}
            <label for="manager_name">{t}Manager{/t}</label>
            <div class="icons">
                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons input-icons'>edit</i>" action="editManager"}
                </div>
                {/render}

                {if $manager!=""}
                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons input-icons'>delete</i>" action="removeManager"}
                </div>
                {/render}

                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons input-icons tooltipped' data-postion='bottom' data-tooltip='{$manager}'>info_outline</i>" title="{$manager}"}
                </div>
                {/render}

                {else}

                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons md-disabled'>delete</i>"}
                </div>
                {/render}

                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons md-disabled context'>info_outline</i>"}
                </div>
                {/render}

                {/if}
            </div>
        </div>

        {else}

        <div class="input-field manager">
            <div class="icons prefix">
                <input type='checkbox' value="1" name="use_manager" id="use_manager" {if $use_manager} checked {/if} onClick="document.mainform.submit();">
            </div>

            <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'>
            <label for="manager_name">{t}Manager{/t}</label> {if $use_manager}

            <div class="icons">
                <div class="icon">
                    {image path="<i class='material-icons'>edit</i>" action="editManager" acl=$managerACL}
                </div>

                {if $manager!=""}
                <div class="icon">
                    {image path="<i class='material-icons'>delete</i>" action="removeManager" acl=$managerACL}
                </div>

                <div class="icon">
                    {image path="<i class='material-icons'>info_outline</i>" title="{$manager}" acl=$managerACL}
                </div>

                {else}

                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons md-disabled'>delete</i>"}
                </div>
                {/render}

                {render acl=$managerACL}
                <div class="icon">
                    {image path="<i class='material-icons md-disabled context'>info_outline</i>"}
                </div>
                {/render}

                {/if}
            </div>
            {/if}
        </div>
        {/if}

        {render acl=$roomNumberACL checkbox=$multiple_support checked=$use_roomNumber}
        <div class="input-field">
            <input type="text" id="roomNumber" name="roomNumber" maxlength=60 value="{$roomNumber}">
            <label for="roomNumber">{t}Room No.{/t}</label>
        </div>
        {/render}

        {if $has_phoneaccount ne "true"}

        {render acl=$telephoneNumberACL checkbox=$multiple_support checked=$use_telephoneNumber}
        <div class="input-field">
            <input type="text" id="telephoneNumber" name="telephoneNumber" maxlength=60 value="{$telephoneNumber}">
            <label for="telephoneNumber">{t}Phone{/t}</label>
        </div>
        {/render}

        {/if}

        {render acl=$mobileACL checkbox=$multiple_support checked=$use_mobile}
        <div class="input-field">
            <input type="text" id="mobile" name="mobile" maxlength=60 value="{$mobile}">
            <label for="mobile">{t}Mobile{/t}</label>
        </div>
        {/render}

        {render acl=$pagerACL checkbox=$multiple_support checked=$use_pager}
        <div class="input-field">
            <input type="text" id="pager" name="pager" maxlength=60 value="{$pager}">
            <label for="pager">{t}Pager{/t}</label>
        </div>
        {/render}

    </div>
    <div class="col s4">
        {render acl=$facsimileTelephoneNumberACL checkbox=$multiple_support checked=$use_facsimileTelephoneNumber}
        <div class="input-field">
            <input type="text" id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" maxlength=60 value="{$facsimileTelephoneNumber}">
            <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
        </div>
        {/render}

        {render acl=$lACL checkbox=$multiple_support checked=$use_l}
        <div class="input-field">
            <input type="text" id="l" name="l" maxlength=60 value="{$l}">
            <label for="l">{t}Location{/t}</label>
        </div>
        {/render}

        {render acl=$stACL checkbox=$multiple_support checked=$use_st}
        <div class="input-field">
            <input type="text" id="st" name="st" maxlength=60 value="{$st}">
            <label for="st">{t}State{/t}</label>
        </div>
        {/render}

        {render acl=$postalAddressACL checkbox=$multiple_support checked=$use_postalAddress}
        <div class="input-field">
            <textarea class="materialize-textarea" id="postalAddress" name="postalAddress">
                {$postalAddress}
            </textarea>
            <label for="postalAdress">{t}Address{/t}</label>
        </div>
        {/render}

    </div>

    {else}

    <div class="col s4">
        {render acl=$vocationACL checkbox=$multiple_support checked=$use_vocation}
        <div class="input-field">
            <input type="text" id="vocation" name="vocation" maxlength=60 value="{$vocation}">
            <label for="vocation">{t}Vocation{/t}</label>
        </div>
        {/render}

        {render acl=$gouvernmentOrganizationalUnitDescriptionACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalUnitDescription}
        <div class="input-field">
            <input type="text" id="gouvernmentOrganizationalUnitDescription" name="gouvernmentOrganizationalUnitDescription" maxlength=60 value="{$gouvernmentOrganizationalUnitDescription}">
            <label for="gouvernmentOrganizationalUnitDescription">{t}Unit description{/t}</label>
        </div>
        {/render}

        {render acl=$gouvernmentOrganizationalUnitSubjectAreaACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalUnitSubjectArea}
        <div class="input-field">
            <input type="text" id="gouvernmentOrganizationalUnitSubjectArea" name="gouvernmentOrganizationalUnitSubjectArea" maxlength=60 value="{$gouvernmentOrganizationalUnitSubjectArea}">
            <label for="gouvernmentOrganizationalUnitSubjectArea">{t}Subject area{/t}</label>
        </div>
        {/render}

        {render acl=$functionalTitleACL checkbox=$multiple_support checked=$use_functionalTitle}
        <div class="input-field">
            <input type="text" name="functionalTitle" id="functionalTitle" maxlength=60 value="{$functionalTitle}">
            <label for="functionalTitle">{t}Functional title{/t}</label>
        </div>
        {/render}

        {render acl=$roleACL checkbox=$multiple_support checked=$use_role}
        <div class="input-field">
            <input type="text" id="role" name="role" maxlength=60 value="{$role}">
            <label for="role">{t}Role{/t}</label>
        </div>
        {/render}

    </div>
    <div class="col s4">
        {render acl=$gouvernmentOrganizationalPersonLocalityACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalPersonLocality}
        <div class="input-field">
            <input type="text" id="gouvernmentOrganizationalPersonLocality" name="gouvernmentOrganizationalPersonLocality" maxlength=60 value="{$gouvernmentOrganizationalPersonLocality}">
            <label for="gouvernmentOrganizationalPersonLocality">{t}Person locality{/t}</label>
        </div>
        {/render}

        {render acl=$gouvernmentOrganizationalUnitACL checkbox=$multiple_support checked=$use_gouvernmentOrganizationalUnit}
        <div class="input-field">
            <input type="text" id="gouvernmentOrganizationalUnit" name="gouvernmentOrganizationalUnit" maxlength=60 value="{$gouvernmentOrganizationalUnit}">
            <label for="gouvernmentOrganizationalUnit">{t}Unit{/t}</label>
        </div>
        {/render}

        {render acl=$streetACL checkbox=$multiple_support checked=$use_street}
        <div class="input-field">
            <input type="text" name="street" id="street" maxlength=60 value="{$street}">
            <label for="street">{t}Street{/t}</label>
        </div>
        {/render}

        {render acl=$postalCodeACL checkbox=$multiple_support checked=$use_postalCode}
        <div class="input-field">
            <input type="text" name="postalCode" id="postalCode" maxlength=60 value="{$postalCode}">
            <label for="postalCode">{t}Postal code{/t}</label>
        </div>
        {/render}

        {render acl=$houseIdentifierACL checkbox=$multiple_support checked=$use_houseIdentifier}
        <div class="input-field">
            <input type="text" id="houseIdentifier" name="houseIdentifier" maxlength=60 value="{$houseIdentifier}">
            <label for="houseIdentifier">{t}House identifier{/t}</label>
        </div>
        {/render}

    </div>
    <div class="col s4">
        {render acl=$roomNumberACL checkbox=$multiple_support checked=$use_roomNumber}
        <div class="input-field">
            <input type="text" id="roomNumber" name="roomNumber" maxlength=60 value="{$roomNumber}">
            <label for="roomNumber">{t}Room No.{/t}</label>
        </div>
        {/render}

        {render acl=$telephoneNumberACL checkbox=$multiple_support checked=$use_telephoneNumber}
        <div class="input-field">
            {if $has_phoneaccount ne "true"}
            <input type="text" id="telephoneNumber" name="telephoneNumber" maxlength=60 value="{$telephoneNumber}">
            {else}
            <i>{t}Please use the phone tab{/t}</i>
            {/if}
            <label for="telephoneNumber">{t}Phone{/t}</label>
        </div>
        {/render}

        {render acl=$facsimileTelephoneNumberACL checkbox=$multiple_support checked=$use_facsimileTelephoneNumber}
        <div class="input-field">
            <input type="text" id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" maxlength=60 value="{$facsimileTelephoneNumber}">
            <label for="facsimileTelephoneNumber">{t}Fax{/t}</label>
        </div>
        {/render}

        {render acl=$ivbbLastDeliveryCollectiveACL checkbox=$multiple_support checked=$use_ivbbLastDeliveryCollective}
        <div class="input-field">
            <input type="text" name="ivbbLastDeliveryCollective" maxlength=60 id="ivbbLastDeliveryCollective" value="{$ivbbLastDeliveryCollective}">
            <label for="ivbbLastDeliveryCollective">{t}Last delivery{/t}</label>
        </div>
        {/render}

        {render acl=$publicVisibleACL checkbox=$multiple_support checked=$use_publicVisible}
        <div class="input-field">
            <select size="1" id="publicVisible" name="publicVisible">
                {html_options values=$ivbbmodes output=$ivbbmodes selected=$publicVisible}
            </select>
            <label for="publicVisible">{t}Public visible{/t}</label>
        </div>
        {/render}

    </div>
    {/if}
</div>


{if $multiple_support}
<input type="hidden" name="user_mulitple_edit" value="1">
{/if}
<input type="hidden" name="generic">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('sn');
</script>
