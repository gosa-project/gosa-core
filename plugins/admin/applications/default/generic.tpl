<div class="application-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            {render acl=$cnACL}
            <div class="input-field">
                <input type='text' id="cn" name="cn" value="{$cn}" title="{t}Application name{/t}">
                <label for="cn">{t}Application name{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$gosaApplicationNameACL}
            <div class="input-field">
                <input id="gosaApplicationName" name="gosaApplicationName" type="text" value="{$gosaApplicationName}" title="{t}Application name to be displayed (i.e. below icons){/t}">
                <label for="gosaApplicationName">{t}Display name{/t}</label>
            </div>
            {/render}

            {render acl=$gosaApplicationExecuteACL}
            <div class="input-field">
                <input id="gosaApplicationExecute" name="gosaApplicationExecute" type="text" value="{$gosaApplicationExecute}" title="{t}Path and/or binary name of application{/t}">
                <label for="gosaApplicationExecute">{t}Execute{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$descriptionACL}
            <div class="input-field">
                <input type='text' id="description" name="description" value="{$description}">
                <label for="description">{t}Description{/t}</label>
            </div>
            {/render}

            {if !$isReleaseApplikation}
            {render acl=$baseACL}
            <div class="input-field ldap-tree">
                {$base}
            </div>
            {/render}
            {/if}
        </div>

        <div class="col s12 xl6">
            {render acl=$gosaApplicationIconACL}
            <div class="symbol-wrapper">
                <div class="symbol-text">
                    <p>{t}Icon{/t}:</p>
                </div>

                <div class="symbol-content">
                    <div class="symbol">
                        {if $IconReadable}
                        <img alt="" src="getbin.php?rand={$rand}">
                        {else}
                        {image path=""}
                        {/if}
                    </div>

                    <div class="input-field add">
                        <div class="file-field">
                            <input type="hidden" name="MAX_FILE_SIZE" value="100000">
                            <div class="btn-small">
                            <span>{t}Browse{/t}</span>
                            <input name="picture_file" type="file" size="20" maxlength="255" accept="image/*.png" id="picture_file">
                            </div>
                            <div class="file-path-wrapper">
                                <input class="file-path validate" type="text">
                            </div>
                        </div>

                        <button class="btn-small" type='submit' name='update' title="{t}Reload picture from LDAP{/t}">{t}Update{/t}</button>
                        <button class="btn-small" type='submit' name='remove_picture' title="{t}Remove picture from LDAP{/t}">{t}Remove picture{/t}</button>
                    </div>
                </div>
            </div>
            {/render}
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Options{/t}</h3>

            {render acl=$execForGroupmembersACL}
            <label>
                <input type=checkbox name="execForGroupmembers" value="1" {$execForGroupmembers}>
                <span>{t}Only executable for members{/t}</span>
            </label>
            {/render}

            {render acl=$overwriteConfigACL}
            <label>
                <input type=checkbox name="overwriteConfig" value="1" {$overwriteConfig}>
                <span>{t}Replace user configuration on startup{/t}</span>
            </label>
            {/render}
        </div>

        <div class="col s12 xl6">
            {render acl=$placeOnDesktopACL}
            <label>
                <input type=checkbox name="placeOnDesktop" value="1" {$placeOnDesktop}>
                <span>{t}Place icon on members desktop{/t}</span>
            </label>
            {/render}

            {render acl=$placeInStartmenuACL}
            <label>
                <input type=checkbox name="placeInStartmenu" value="1" {$placeInStartmenu}>
                <span>{t}Place entry in members start menu{/t}</span>
            </label>
            {/render}

            {render acl=$placeOnKickerACL}
            <label>
                <input type=checkbox name="placeOnKicker" value="1" {$placeOnKicker}>
                <span>{t}Place entry in members launch bar{/t}</span>
            </label>
            {/render}
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <div class="col s12 xl12">
            <h3>{t}Script{/t}</h3>

            {render acl=$gotoLogonScriptACL}
            <div class="input-field textarea-multiple">
                <textarea name="gotoLogonScript" class="materialize-textarea multiple">{$gotoLogonScript}</textarea>
            </div>
            {/render}

            {render acl=$gotoLogonScriptACL}
            <div class="script-action">
                <div class="file-field input-field">
                    <div class="btn-small">
                        <span>{t}Browse{/t}</span>
                        <input type="file" name="ScriptFile" value="{t}Import{/t}">
                    </div>
                    <div class="file-path-wrapper">
                        <input class="file-path validate" type="text">
                    </div>
                </div>
                <button class="btn-small" type='submit' name='upLoad'>{t}Upload{/t}</button>
                <button class="btn-small" name='save'>
                {image path="<i class='material-icons input-icons'>save</i>" action="downloadScript" title="{t}Download{/t}"}</button>
            </div>
            {/render}
        </div>
    </div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('cn');
</script>
