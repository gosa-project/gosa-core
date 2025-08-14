<input type="hidden" name="MAX_FILE_SIZE" value="2000000">


<div class="content-wrapper certificate">
    <h2>{t}Certificates{/t}</h2>

    <div class="card-content-scroll">
        <div class="row">
            <div class="col s4">
                {t}Standard certificate{/t}
            </div>
            <div class="col s4">
                <span>{if $Certificate_readable}{$userCertificateinfo}{/if}</span>
            </div>
            <div class="col s4 certificate-upload">
                {if $userCertificate_state ne "true"}

                {render acl=$CertificateACL}
                <div class="file-field input-field">
                    <div class="btn-small">
                        <span>Datei auswählen</span>
                        <input id="userCertificate_file" name="userCertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
                    </div>
                    <div class="file-path-wrapper">
                        <input placeholder='Keine Datei ausgewählt' type="text" class="file-path validate">
                    </div>
                </div>
                {/render}

                {else}

                {render acl=$CertificateACL}
                <button class="btn-small" type='submit' name='remove_userCertificate'>{t}Remove{/t}</button>
                {/render}

                {/if}
            </div>
        </div>
        <hr class="divider">
        <div class="row">
            <div class="col s4">
                {t}S/MIME certificate{/t}
            </div>
            <div class="col s4">
                <span>{if $Certificate_readable}{$userSMIMECertificateinfo}{/if}</span>
            </div>
            <div class="col s4 certificate-upload">
                {if $userSMIMECertificate_state ne "true"}

                {render acl=$CertificateACL}
                <div class="file-field input-field">
                    <div class="btn-small">
                        <span>Datei auswählen</span>
                        <input id="userSMIMECertificate_file" name="userSMIMECertificate_file" type="file" size="20" maxlength="255" accept="text/*.*">
                    </div>
                    <div class="file-path-wrapper">
                        <input placeholder='Keine Datei ausgewählt' type="text" class="file-path validate">
                    </div>
                </div>
                {/render}

                {else}

                {render acl=$CertificateACL}
                <button type='submit' name='remove_userSMIMECertificate'>{t}Remove{/t}</button>
                {/render}

                {/if}
            </div>
        </div>
        <hr class="divider">
        <div class="row">
            <div class="col s4">
                {t}PKCS12 certificate{/t}
            </div>
            <div class="col s4">
                <span>{if $Certificate_readable}{$userPKCS12info}{/if}</span>
            </div>
            <div class="col s4 certificate-upload">
                {if $userPKCS12_state ne "true"}

                {render acl=$CertificateACL}
                <div class="file-field input-field">
                    <div class="btn-small">
                        <span>Datei auswählen</span>
                        <input id="userPKCS12_file" name="userPKCS12_file" type="file" size="20" maxlength="255" accept="text/*.*">
                    </div>
                    <div class="file-path-wrapper">
                        <input placeholder='Keine Datei ausgewählt' type="text" class="file-path validate">
                    </div>
                </div>
                {/render}

                {else}

                {render acl=$CertificateACL}
                <button type='submit' name='remove_userPKCS12'>{t}Remove{/t}</button>
                {/render}

                {/if}
            </div>
        </div>
    </div>
</div>

{if $governmentmode eq "true"}
<hr class="divider">
<div class="row">
    <div class="col s4">
        {t}Certificate serial number{/t}
    </div>
    <div class="col s8">
        <label>
            {render acl=$CertificateACL}
                <input type='text' id="certificateSerialNumber" name="certificateSerialNumber" size=10 maxlength=20 value="{$certificateSerialNumber}">
            {/render}
        </label>
    </div>
</div>
{/if}

<div class='card-action'>

    {render acl=$CertificateACL}
    <button class="btn-small primary" type='submit' name='cert_edit_finish'>{msgPool type=saveButton}</button>
    {/render}

    <button class="btn-small primary" type='submit' name='cert_edit_cancel'>{msgPool type=cancelButton}</button>
</div>
