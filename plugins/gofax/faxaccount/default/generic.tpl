<div class="faxaccount-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Generic{/t}</h3>

            <div class="input-field">
                {if $multiple_support}
                <input type='text' name="dummy1" value="{t}Multiple edit{/t}" disabled id="facsimileTelephoneNumber"> 
                {else} 
                {render acl=$facsimileTelephoneNumberACL}
                <input name="facsimileTelephoneNumber" id="facsimileTelephoneNumber" size=20 maxlength=65 type='text' value="{$facsimileTelephoneNumber}" title="{t}Fax number for GOfax to trigger on{/t}"> 
                {/render} 
                {/if}
                <label for="facsimileTelephoneNumber">{t}Fax{/t}{$must}</label>
            </div>

            {render acl=$goFaxLanguageACL checkbox=$multiple_support checked=$use_goFaxLanguage}
            <div class="input-field">
                <select size="1" name="goFaxLanguage" id="goFaxLanguage" title="{t}Specify the GOfax communication language for FAX to mail gateway{/t}">
                    {html_options options=$languages selected=$goFaxLanguage}
                </select> 
                <label>{t}Language{/t}</label>
            </div>
            {/render}

            {render acl=$goFaxFormatACL checkbox=$multiple_support checked=$use_goFaxFormat}
            <div class="input-field">
                <select id="goFaxFormat" size="1" name="goFaxFormat" title="{t}Specify delivery format for FAX to mail gateway{/t}">
                    {html_options values=$formats output=$formats selected=$goFaxFormat}
                </select> 
                <label for="goFaxFormat">{t}Delivery format{/t}</label>
            </div>
            {/render}
        </div>

        <div class="col s12 xl6">
            <h3>{t}Delivery methods{/t}</h3>
            
            {render acl=$goFaxIsEnabledACL checkbox=$multiple_support checked=$use_goFaxIsEnabled}
            <label>
                <input type=checkbox name="goFaxIsEnabled" value="1" {$goFaxIsEnabled} > 
                <span>{t}Temporary disable FAX usage{/t}</span>
            </label>
            {/render} 

            {if $has_mailaccount eq "false"} 
            {render acl=$faxtomailACL checkbox=$multiple_support checked=$use_faxtomail}
            <label>
                <input type=checkbox name="faxtomail" value="1" {$faxtomail} > 
                <span>{t}Deliver FAX as mail to{/t}</span>
            </label>
            {/render}

            {render acl=$faxtomailACL checkbox=$multiple_support checked=$use_mail}
            <div class="input-field add">
                <input type='text' name="mail" id="mail" size=25 maxlength=65 value="{$mail}" > 
            </div>
            {/render} 

            {else} 
            {render acl=$faxtomailACL checkbox=$multiple_support checked=$use_faxtomail}
            <label>
                <input type=checkbox name="faxtomail" value="1" {$faxtomail} > 
                <span>{t}Deliver FAX as mail{/t}</span> 
            </label>
            {/render} 
            {/if}

            {render acl=$faxtoprinterACL checkbox=$multiple_support checked=$use_faxtoprinter}
            <label>
                <input type=checkbox name="faxtoprinter" value="1" {$faxtoprinter} > 
                <span>{t}Deliver FAX to printer{/t}</span>
            </label>
            {/render} 

            {render acl=$faxtoprinterACL checkbox=$multiple_support checked=$use_goFaxPrinter}
            <div class="input-field">
                <select size="1" name="goFaxPrinterSelected">
                    {html_options options=$printers selected=$goFaxPrinter}
                </select> 
            </div>
            {/render}
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <div class="col s12 xl6">
            {if !$multiple_support}
            <h3>{t}Alternate FAX numbers{/t}</h3>

            {render acl=$facsimileAlternateTelephoneNumberACL}
            <div class="input-field">
                <select style="width:100%" name="alternate_list[]" size="10" multiple>
                    {html_options values=$facsimileAlternateTelephoneNumber output=$facsimileAlternateTelephoneNumber}
                </select> 
            </div>
            {/render}
            
            {render acl=$facsimileAlternateTelephoneNumberACL}
            <div class="input-field add">
                <input type='text' name="forward_address" size=20 align="middle" maxlength=65 value=""> 
                <button class="btn-small" type='submit' name='add_alternate'>{msgPool type=addButton}</button>
                <button class="btn-small" type='submit' name='add_local_alternate'>{t}Add local{/t}</button>
                <button class="btn-small" type='submit' name='delete_alternate'>{msgPool type=delButton}</button> 
            </div>
            {/render} 
            {/if}
        </div>

        <div class="col s12 xl6">
            <h3>{t}Blacklists{/t}</h3>

            {render acl=$goFaxRBlocklistACL checkbox=$multiple_support checked=$use_edit_incoming}
            <div class="edit-context">
                {t}Blacklists for incoming FAX{/t}
                <button class="btn-small" type='submit' name='edit_incoming'>{t}Edit{/t}</button> 
            </div>
            {/render}

            {render acl=$goFaxSBlocklistACL checkbox=$multiple_support checked=$use_edit_outgoing}
            <div class="edit-context">
                {t}Blacklists for outgoing FAX{/t}
                <button class="btn-small" type='submit' name='edit_outgoing'>{t}Edit{/t}</button> 
            </div>
            {/render}
        </div>
    </div>
</div>

<input type="hidden" name="faxTab" value="faxTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('facsimileTelephoneNumber');
</script>