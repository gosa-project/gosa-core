<div class="device-wrapper">
    <div class="row">
        <h3>{t}Devices{/t}</h3>
        <div class="col s12 xl6">
            {render acl=$cnACL}
            <div class="input-field">
                <input type="text" size=40 value="{$cn}" name="cn" id="cn">
                <label for="cn">{t}Device name{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$descriptionACL}
            <div class="input-field">
                <input type="text" size=40 value="{$description}" name="description" id="description">
                <label for="description">{t}Description{/t}</label>
            </div>
            {/render}

            {render acl=$typeACL}
            <div class="input-field">
                <select id="type" size="1" name="type" title="{t}Choose the device type{/t}">
                    {html_options options=$types selected=$type}
                </select>
                <label>{t}Device type{/t}</label>
            </div>
            {/render}

            {render acl=$baseACL}
            <div class="input-field ldap-tree">
                {$base}
            </div>
            {/render}
        </div>

        <div class="col s12 xl6">
            {render acl=$devIDACL}
            <div class="input-field">
                <input type="text" value="{$devID}" name="devID" id="devID">
                <label for="devID">{t}Serial number{/t}&nbsp;{t}(iSerial){/t}{$must}</label>
            </div>
            {/render}

            {render acl=$vendorACL}
            <div class="input-field">
                <input type="text" value="{$vendor}" name="vendor" id="vendor">
                <label for="vendor">{t}Vendor-ID{/t}&nbsp;{t}(idVendor){/t}{$must}</label>
            </div>
            {/render}

            {render acl=$serialACL}
            <div class="input-field">
                <input type="text" value="{$serial}" name="serial" id="serial">
                <label for="produkt">{t}Product-ID{/t}&nbsp;{t}(idProduct){/t}{$must}</label>
            </div>
            {/render}
        </div>
    </div>
</div>
<input type='hidden' value="1" name="deviceGeneric_posted">

<script language="JavaScript" type="text/javascript">
    // First input field on page    
    focus_field('name');  
</script>
