<h3>{t}Creating a new user using templates{/t}</h3>
<p>
    {t}Creating a new user can be assisted by using templates. Many database records will be filled automatically.
    Choose 'none' to skip the usage of templates.{/t}
</p>

<div class="row content">
    <div class="col s6 xl5">
        <div class="input-field">
            <select name="template" id="template">
                {html_options options=$templates selected=$template}
            </select>
            <label for="template">{t}Template{/t}</label>
        </div>
        <div class="input-field">
            <input type='text' name="sn" id="sn" value="{$sn}" />
            <label for="sn">{t}Last name{/t}</label>
        </div>
        <div class="input-field">
            <input type='text' name="givenName" id="givenName" value="{$givenName}" />
            <label for="givenName">{t}First name{/t}</label>
        </div>
        {if $got_uid eq "true"}
        {if $edit_uid eq "false"}
        {if $allowUidProposalModification == "true"}
        <div class="input-field">
            <select size="1" name="uidSelect" id='uidSelect' onChange="
            document.getElementById('uid').value = document.getElementById('uidSelect').value;">
                {html_options output=$uids values=$uids selected=$uid}
            </select>
            <label for="uidSelect">{t}Login{/t}</label>
        </div>
        <a class="btn-small primary" onClick="$('uid').toggle();" title="{t}Modify the uid proposal{/t}"><i
                class="material-icons input-icons">edit</i></a>
        <div class="input-field">
            <input type='text' id="uid" name="uid" value="{$uid}" style="display: none;">
        </div>
        {else}
        <div class="input-field">
            <select size="1" name="uid">
                {html_options output=$uids values=$uids selected=$uid}
            </select>
            <label for="uid">{t}Login{/t}</label>
        </div>
        {/if}
        {else}
        <div class="input-field">
            <input type='text' id="uid" value="{$uid}">
            <label for="uid">{t}Login{/t}</label>
        </div>
        {/if}
        {/if}
        </div>
    </div>

    <div class="card-action">
        <button class="btn-small primary" type='submit' name='template_continue'>{t}Continue{/t}</button>
        <button class="btn-small primary" type='submit' name='edit_cancel'>{msgPool type=cancelButton}</button>
    </div>

    <!-- Place cursor -->
    <script language="JavaScript" type="text/javascript">
        focus_field('sn');
    </script>