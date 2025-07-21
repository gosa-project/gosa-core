<!--////////////////////
	//	ORGANIZATIONAL UNIT (ou)
    //////////////////// -->
<table summary="{t}Organizational Unit{/t}" style="width:100%" cellpadding=4>
    <tr>
        <td style="width:50%">
            <h3>{t}Properties{/t}</h3>

            <table summary="{t}Properties{/t}">
                <tr>
                    <td>
                        <LABEL for="ou">{t}Name of department{/t}</LABEL>{$must}</td>
                    <td>
                        {render acl=$ouACL}
                        <input type='text' id="ou" name="ou" size=25 maxlength=60 value="{$ou}" title="{t}Name of subtree to create{/t}"> {/render}
                    </td>
                </tr>
                <tr>
                    <td>
                        <LABEL for="description">{t}Description{/t}</LABEL>{$must}</td>
                    <td>
                        {render acl=$descriptionACL}
                        <textarea id='description' name='description' style='width:220px' title="{t}Descriptive text for   department{/t}">{$description}</textarea> {/render}
                    </td>
                </tr>
                <tr>
                    <td>
                        <LABEL for="businessCategory">{t}Category{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$businessCategoryACL}
                        <input type='text' id="businessCategory" name="businessCategory" size=25 maxlength=80 value="{$businessCategory}" title="{t}Category for this subtree{/t}"> {/render}
                    </td>
                </tr>
                <tr>
                    <td colspan="2"><br></td>
                </tr>
                {if !$is_root_dse}
                <tr>
                    <td>
                        <LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>

                    <td>
                        {render acl=$baseACL} {$base} {/render}
                    </td>
                </tr>
                {/if} {if $manager_enabled}
                <tr>
                    <td><label for="manager">{t}Manager{/t}</label></td>
                    <td>
                        {render acl=$managerACL}
                        <input type='text' name='manager_name' id='manager_name' value='{$manager_name}' disabled title='{$manager}'> {/render} {render acl=$managerACL} {image path="images/lists/edit.png" action="editManager"} {/render} {if $manager!=""}
                        {render acl=$managerACL} {image path="images/info_small.png" title="{$manager}"} {/render} {render acl=$managerACL} {image path="images/lists/trash.png" action="removeManager"} {/render} {/if}
                    </td>
                </tr>
                {/if}

            </table>

        </td>
        <td class='left-border'>

            &nbsp;
        </td>
        <td>
            <h3>{t}Location{/t}</h3>

            <table summary="{t}Location{/t}" style="width:100%">
                <tr>
                    <td>
                        <LABEL for="st">{t}State{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$stACL}
                        <input type='text' id="st" name="st" size=25 maxlength=60 value="{$st}" title="{t}State where this subtree is located{/t}"> {/render}
                    </td>
                </tr>
                <tr>
                    <td>
                        <LABEL for="l">{t}Location{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$lACL}
                        <input type='text' id="l" name="l" size=25 maxlength=60 value="{$l}" title="{t}Location of this subtree{/t}"> {/render}
                    </td>
                </tr>
                <tr>
                    <td>
                        <LABEL for="postalAddress">{t}Address{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$postalAddressACL}
                        <textarea id="postalAddress" name="postalAddress" style="width:100%" rows=3 cols=22 title="{t}Postal address of this subtree{/t}">{$postalAddress}</textarea> {/render}
                </tr>
                <tr>
                    <td>
                        <LABEL for="telephoneNumber">{t}Phone{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$telephoneNumberACL}
                        <input type='text' id="telephoneNumber" name="telephoneNumber" size=25 maxlength=60 value="{$telephoneNumber}" title="{t}Base telephone number of this subtree{/t}"> {/render}
                    </td>
                </tr>
                <tr>
                    <td>
                        <LABEL for="facsimileTelephoneNumber">{t}Fax{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$facsimileTelephoneNumberACL}
                        <input type='text' id="facsimileTelephoneNumber" name="facsimileTelephoneNumber" size=25 maxlength=60 value="{$facsimileTelephoneNumber}" title="{t}Base facsimile telephone number of this subtree{/t}"> {/render}
                    </td>
                </tr>
            </table>

            </td>
    </tr>
</table>

<hr>
<h3>{t}Administrative settings{/t}</h3>
{render acl=$gosaUnitTagACL}
<input id="is_administrational_unit" type=checkbox name="is_administrational_unit" value="1" {$gosaUnitTag}>
<label for="is_administrational_unit">{t}Tag department as an independent administrative unit{/t}</label> {/render}

<!-- Place cursor -->
<input type='hidden' name='dep_generic_posted' value='1'>
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('ou');
</script>