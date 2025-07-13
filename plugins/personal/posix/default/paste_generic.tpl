
<div class="row content">
    <div class="col s6 xl5">
        <h3>{t}POSIX settings{/t}</h3>
        <div class="input-field">
            <input type='text' id="homeDirectory" name="homeDirectory" value="{$homeDirectory}">
            <label for="homeDirectory">{t}Home directory{/t}</label>
        </div>
        <div class="input-field">
            <select id="primaryGroup" size="1" name="primaryGroup">
                {html_options options=$secondaryGroups selected=$primaryGroup}
            </select>
            <label for="primaryGroup">{t}Primary group{/t}</label></td>
        </div>
        <p>
            <label>
                <input type=checkbox id="force_ids" name="force_ids" value="1" {$force_ids}
                    onclick="changeState('uidNumber');changeState('gidNumber');">
                <span>{t}Force UID/GID{/t}</span>
            </label>
        </p>
        <div class="input-field">
            <input type='text' id="uidNumber" name="uidNumber" {$forceMode} value="{$uidNumber}">
            <label for="uidNumber">{t}UID{/t}</label>
        </div>
        <div class="input-field">
            <input type='text' id="gidNumber" name="gidNumber"{$forceMode} value="{$gidNumber}">
            <label for="gidNumber">{t}GID{/t}</label>
        </div>
    </div>
    <div class="col s6 xl5">
        <h3>{t}Group membership{/t}</h3>
        <div class="input-field">
            <select name="group_list[]" size=16 multiple>
                {html_options options=$groupMembership}
            </select>
            <button class="btn-small" type='submit' name='edit_groupmembership'>{msgPool type=addButton}</button>
            <button class="btn-small" type='submit' name='delete_groupmembership'>{msgPool type=delButton}</button>
        </div>
    </div>
</div>