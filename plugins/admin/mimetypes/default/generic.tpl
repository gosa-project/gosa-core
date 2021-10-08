<div class="mime-wrapper">
    <div class="row">
        <h2>{t}Generic{/t}</h2>

        <div class="col s12 xl6">
            {render acl=$cnACL}
            <div class="input-field">
                <input type="text" name='cn' id="cn" value="{$cn}" title='{t}Please enter a name for the mime-type here{/t}'>
                <label for="cn">{t}Mime type{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$gotoMimeGroupACL}
            <div class="input-field">
                <select name='gotoMimeGroup' title='{t}Categorize this mime type{/t}' size=1>
                  {html_options output=$gotoMimeGroups values=$gotoMimeGroups selected=$gotoMimeGroup}
                </select>
                <label>{t}Mime-group{/t}</label>
            </div>
            {/render}

            {render acl=$descriptionACL}
            <div class="input-field">
                <input type="text" name='description' id="description" value="{$description}" title='{t}Please specify a description{/t}'>
                <label for="description">{t}Description{/t}</label>
            </div>
            {/render}

            {if !$isReleaseMimeType}
            {render acl=$baseACL}
            <div class="input-field ldap-tree">
                {$base}
            </div>
            {/render}
            {/if}
        </div>

        <div class="col s12 xl6">
            <div class="symbol-wrapper">
                <div class="symbol-text">
                    <p>{t}Icon{/t}:</p>
                </div>

                <div class="symbol-content">
                    <div class="symbol">
                        {if $IconReadable}
                        <img alt="{t}Mime-icon{/t}" src="{$gotoMimeIcon}">
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

                        <button class="btn-small" type='submit' name='update_icon' title="{t}Update mime type icon{/t}">{t}Update{/t}</button>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <h2>{t}Left click{/t}</h2>

        <div class="col s12 xl6">
            <h3>{t}File patterns{/t}{$must}</h3>

            {render acl=$gotoMimeFilePatternACL}
            {$gotoMimeFilePatterns}
            {/render}

            {render acl=$gotoMimeFilePatternACL}
            <div class="input-field add">
                <input type='text' name='NewFilePattern' value='' title='{t}Please specify a new file pattern{/t}'>
                <button class="btn-small" type='submit' name='AddNewFilePattern' title="{t}Add a new file pattern{/t}">{msgPool type=addButton}</button>
            </div>
            {/render}

        </div>

        <div class="col s12 xl6">
            <h3>{t}Applications{/t}</h3>

            {render acl=$gotoMimeApplicationACL}
            {$gotoMimeApplications}
            {/render}

            {render acl=$gotoMimeApplicationACL}
            <div class="input-field">
                <select name="NewApplicationSelect" size=1>
                    <option value="">-</option>
                    {html_options options=$ApplicationList}
                </select>
            </div>
            {/render}

            {render acl=$gotoMimeApplicationACL}
            <div class="input-field add">
                <input type='text' name='NewApplication' value='' title='{t}Enter an application name here{/t}'>
                <button class="btn-small" type='submit' name='AddNewApplication' title="{t}Add application{/t}">{msgPool type=addButton}</button>
            </div>
            {/render}
        </div>
    </div>

    <hr class="divider">

    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Embedding{/t}</h3>

            {render acl=$gotoMimeLeftClickActionACL}
            <label>
                <input type='radio' name='gotoMimeLeftClickAction_IE' value='I' {if $gotoMimeLeftClickAction_I} checked {/if}>
                <span>{t}Show file in embedded viewer{/t}</span>
            </label>

            <label>
                <input type='radio' name='gotoMimeLeftClickAction_IE' value='E' {if $gotoMimeLeftClickAction_E} checked {/if}>
                <span>{t}Show file in external viewer{/t}</span>
            </label>

            <label>
                <input type='checkbox' name='gotoMimeLeftClickAction_Q' value='1' {if $gotoMimeLeftClickAction_Q} checked {/if}>
                <span>{t}Ask whether to save to local disk{/t}</span>
            </label>
            {/render}
        </div>

        <div class="col s12 xl6">
            <h3>{t}Applications{/t}</h3>

            {render acl=$gotoMimeEmbeddedApplicationACL}
            {$gotoMimeEmbeddedApplications}
            {/render}

            {render acl=$gotoMimeEmbeddedApplicationACL}
            <div class="input-field">
                <select name="NewEmbeddedApplicationSelect" size=1>
                    <option value="">-</option>
                    {html_options options=$ApplicationList}
                </select>
            </div>
            {/render}

            {render acl=$gotoMimeEmbeddedApplicationACL}
            <div class="input-field add">
                <input type='text' name='NewEmbeddedApplication' value='' title='{t}Enter an application name here{/t}'>
                <button class="btn-small" type='submit' name='AddNewEmbeddedApplication' title='{t}Add application{/t}'>{msgPool type=addButton}</button>
            </div>
            {/render}

        </div>
    </div>
</div>

<input type="hidden" name="MimeGeneric" value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('cn');
</script>
