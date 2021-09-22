{if $multiple_support}
    <input type="hidden" value="1" name="nisnetgroup_mulitple_edit">
{/if}


<table summary="" style="width:100%;">
    <tr>
        <td style="width:50%; vertical-align:top;">
            <table summary="" style="width:100%">
                <tr>
                    <td><LABEL for="cn">{t}NIS Netgroup name{/t}</LABEL>{$must}</td>
                    <td>
                        {if $multiple_support}
                            <input type='text' id="dummy1" name="dummy1" value="{t}Multiple edit{/t}" disabled>
                        {else}
                            {render acl=$cnACL}
                            <input type='text' id="cn" name="cn" 
                                value="{$cn}" title="{t}Name of the NIS Netgroup{/t}">
                            {/render}
                        {/if}
                    </td>
                </tr>
                <tr>
                    <td>
                        <LABEL for="description">{t}Description{/t}</LABEL>
                    </td>
                    <td>
                        {render acl=$descriptionACL checkbox=$multiple_support checked=$use_description}
                            <input type='text' id="description" name="description" 
                                value="{$description}" title="{t}Descriptive text for this NIS Netgroup{/t}">
                        {/render}
                    </td>
                </tr>
                {if !$multiple_support}
                <tr>
                    <td>
                        <LABEL for="base">{t}Base{/t}</LABEL>{$must}
                    </td>
                    <td>
                        {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
                            {$base}
                        {/render}
                    </td>
                </tr>
                {/if}
            </table>

        </td>
        <td class='left-border'>   &nbsp;
        </td>

        <td style="vertical-align:top;">
            <table summary="" style="width:100%">
                <tr>
                    <td style="vertical-align:top; width:50%">
                        <b>
                            <LABEL for="members">{t}NIS Netgroup members{/t}</LABEL></b>
                        <br>
                        {$memberList}
                        <br>
                        {render acl=$memberCnACL}
                            <input type=submit name="edit_membership" value="{msgPool type=addButton}">
                        {/render}
                    </td>
                </tr> 
            </table>
        </td>
    </tr>
</table>

<input type="hidden" name="nisnetgroupedit" value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('cn');
</script>
