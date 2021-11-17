{if $multiple_support}
<input type="hidden" value="1" name="group_mulitple_edit"> {/if}

<div class="group-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <div class="input-field">
                {if $multiple_support}
                <input type='text' id="dummy1" name="dummy1" size=25 maxlength=60 value="{t}Multiple edit{/t}" disabled> {else} {render acl=$cnACL}
                <input type='text' id="cn" name="cn" size=25 maxlength=60 value="{$cn}" title="{t}POSIX name of the group{/t}"> {/render} {/if}
                <label for="cn">{t}Group name{/t}{$must}</label>
            </div>

            {render acl=$descriptionACL checkbox=$multiple_support checked=$use_description}
            <div class="input-field">
                <input type='text' id="description" name="description" size=40 maxlength=80 value="{$description}" title="{t}Descriptive text for this group{/t}">
                <label for="description">{t}Description{/t}</label>
            </div>
            {/render}

            {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
            <div class="input-field ldap-tree">
                {$base}
            </div>
            {/render}

            {if !$multiple_support}
            {render acl=$gidNumberACL}
            <div class="input-gid">
                <label>
                    <p>
                        <input type=checkbox name="force_gid" value="1" title="{t}Normally IDs are auto-generated, select to specify manually{/t}" {$force_gid} onclick="changeState('gidNumber')">
                        <span>
                            {t}Force GID{/t}
                            <input type='text' name="gidNumber" size=9 maxlength=9 id="gidNumber" {$forceMode} value="{$gidNumber}" title="{t}Forced ID number{/t}">
                        </span>
                    </p>
                </label>
            </div>
            {/render}

            <div class="row">
                {render acl=$sambaGroupTypeACL}
                <div class="col s6">
                    <label>
                        <p>
                            <input type=checkbox name="smbgroup" value="1" {$smbgroup}
                            title="{t}Select to create a samba conform group{/t}">
                            <span>
                                <select id="groupType" size="1" name="groupType">
                                    {html_options options=$groupTypes selected=$groupType}
                                </select>
                            </span>
                        </p>
                    </label>
                </div>
                {/render}

                <div class="col s6">

                    {render acl=$sambaDomainNameACL}
                    <div class="input-field">
                        <select id="sambaDomainName" size="1" name="sambaDomainName">
                                    {html_options values=$sambaDomains output=$sambaDomains selected=$sambaDomainName}
                                </select>
                        <label>{t}in domain{/t}</label>
                    </div>
                    {/render}

                </div>
            </div>
            {else}

            {render acl=$sambaGroupTypeACL checkbox=$multiple_support checked=$use_smbgroup}
            <label>
                <p>
                    <input class="center" type=checkbox name="smbgroup" value="1" {$smbgroup}>
                    <span>{t}Select to create a samba conform group{/t}</span>
                </p>
            </label>
            {/render}

            {render acl=$sambaGroupTypeACL checkbox=$multiple_support checked=$use_groupType}
            <div class="input-field">
                <select size="1" name="groupType">
                        {html_options options=$groupTypes selected=$groupType}
                    </select>
                <label>{t}in domain{/t}</label>
            </div>
            {/render}

            {render acl=$sambaDomainNameACL checkbox=$multiple_support checked=$use_sambaDomainName}
            <div class="input-field">
                <select id="sambaDomainName" size="1" name="sambaDomainName">
                        {html_options values=$sambaDomains output=$sambaDomains selected=$sambaDomainName}
                    </select>
                <label>{t}Domain name{/t}</label>
            </div>
            {/render}

            {/if}
            {if $pickupGroup == "true"}

            {render acl=$fonGroupACL checkbox=$multiple_support checked=$use_fon_group}
            <label>
                <p>
                    <input class="center" type=checkbox name="fon_group" value="1" {$fon_group}>
                    <span>{t}Members are in a phone pickup group{/t}</span>
                </p>
            </label>
            {/render}

            {/if}
            {if $nagios == "true"}

            {render acl=$nagiosGroupACL checkbox=$multiple_support checked=$use_nagios_group}
            <div class="input-field">
                <input class="center" type=checkbox name="nagios_group" value="1" {$nagios_group}>
                <span>{t}Members are in a Nagios group{/t}</span>
            </div>
            {/render}

            {/if}

            <hr class="divider">

        {$trustModeDialog}
        </div>

        <div class="col s12 xl6">
            {if $restrictedByDynGroup}
            <h3 class="card-title">{t}The group members are part of a dyn-group and cannot be managed!{/t}</h3>

            {if $multiple_support}

            {render acl=$memberACL}
            {$commonList}
            {/render}

            {else}

            {render acl=$memberACL}
            {$memberList}
            {/render}

            {/if}
            {else}
            {if $multiple_support}
            <h3 class="card-title">{t}Common group members{/t}</h3>

            {render acl=$memberACL}
            {$commonList}
            <button class="btn-small" type='submit' name='edit_membership'>{msgPool type=addButton}</button>
            {/render}

            <h3 class="card-title">{t}Partial group members{/t}</h3>

            {render acl=$memberACL}
            {$partialList}
            {/render}

            {else}
            <h3 class="card-title">{t}Group members{/t}</h3>

            {render acl=$memberACL}
            {$memberList}
            <button class="btn-small" type='submit' name='edit_membership'>{msgPool type=addButton}</button>
            {/render}

            {/if}
            {/if}
        </div>
    </div>

    <input type="hidden" name="groupedit" value="1">

    <!-- Place cursor -->
    <script language="JavaScript" type="text/javascript">
        // First input field on page
        focus_field('cn');
    </script>
</div>
