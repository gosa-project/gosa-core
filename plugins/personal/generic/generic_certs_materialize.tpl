<input type="hidden" name="MAX_FILE_SIZE" value="2000000">

<h5>{t}Certificates{/t}</h5>

<div class="row">
    <div class="col s4">
        {t}Standard certificate{/t}
    </div>
    <div class="col s4">
        <span>{if $Certificate_readable}{$userCertificateinfo}{/if}</span>
    </div>
    <div class="col s4">
        <label>
            {if $userCertificate_state ne "true"}
            {render acl=$CertificateACL}
            <input id="userCertificate_file" name="userCertificate_file" type="file" size="20" maxlength="255"
                accept="text/*.*">
            {/render}
            {else}
            {render acl=$CertificateACL}
            <button type='submit' name='remove_userCertificate'>
                {t}Remove{/t}</button>
            {/render}
            {/if}
        </label>
    </div>
</div>
<hr>
<div class="row">
    <div class="col s4">
        {t}S/MIME certificate{/t}
    </div>
    <div class="col s4">
        <span>{if $Certificate_readable}{$userSMIMECertificateinfo}{/if}</span>
    </div>
    <div class="col s4">
        <label>
            {if $userSMIMECertificate_state ne "true"}
            {render acl=$CertificateACL}
            <input id="userSMIMECertificate_file" name="userSMIMECertificate_file" type="file" size="20" maxlength="255"
                accept="text/*.*">
            {/render}
            {else}
            {render acl=$CertificateACL}
            <button type='submit' name='remove_userSMIMECertificate'>
                {t}Remove{/t}</button>
            {/render}
            {/if}
        </label>
    </div>
</div>
<hr>
<div class="row">
    <div class="col s4">
        {t}PKCS12 certificate{/t}
    </div>
    <div class="col s4">
        <span>{if $Certificate_readable}{$userPKCS12info}{/if}</span>
    </div>
    <div class="col s4">
        <label>
            {if $userPKCS12_state ne "true"}
            {render acl=$CertificateACL}
            <input id="userPKCS12_file" name="userPKCS12_file" type="file" size="20" maxlength="255" accept="text/*.*">
            {/render}
            {else}
            {render acl=$CertificateACL}
            <button type='submit' name='remove_userPKCS12'>
                {t}Remove{/t}</button>
            {/render}
            {/if}
        </label>
    </div>
</div>

{if $governmentmode eq "true"}
<hr>
<div class="row">
    <div class="col s6">
        {t}Certificate serial number{/t}
    </div>
    <div class="col s6">
        <label>
            {render acl=$CertificateACL}
            <input type='text' id="certificateSerialNumber" name="certificateSerialNumber" size=10 maxlength=20
                value="{$certificateSerialNumber}">
            {/render}
        </label>
    </div>
</div>
{/if}



<hr>
<div class='plugin-actions'>
    {render acl=$CertificateACL}
    <button class="btn-small gonicus-color" type='submit' name='cert_edit_finish'>
        {msgPool type=saveButton}</button>
    {/render}
    <button class="btn-small gonicus-color" type='submit' name='cert_edit_cancel'>
        {msgPool type=cancelButton}</button>
</div>