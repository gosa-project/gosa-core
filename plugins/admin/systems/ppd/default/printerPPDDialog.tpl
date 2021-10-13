<div class="system-wrapper">
    <div class="system-printer-driver">
        <div class="row">
            <div class="col s12 xl6">
                <h3>{t}Printer driver{/t}</h3>

                {if !$path_valid}
                    <p><b>{msgPool type=invalidConfigurationAttribute param=ppdPath}</b></p>
                {else}
                    {render acl=$acl}
                        <div class="input-field add">
                            <p id="testi"><i>{$ppdString}</i></p>
                            <label for="testi">{t}Model{/t}:</label>

                            <button class="btn-small" type="submit" name="SelectPPD">{t}Select{/t}</button>
                        </div>

                        <div class="input-field">
                            <div class="new-driver-action" id="testi2">
                                <div class="file-field input-field">
                                    <div class="btn-small">
                                        <span>{t}Browse{/t}</span>
                                        <input id="NewPPDFile" name="NewPPDFile" type="file">
                                    </div>
                                    <div class="file-path-wrapper">
                                        <input type="text" class="file-path validate">
                                    </div>
                                </div>

                                <button class="btn-small" type="submit" name="SubmitNewPPDFile">{t}Upload{/t}</button>
                            </div>
                            <label for="testi2">{t}New driver{/t}:</label>
                        </div>
                    {/render}
                {/if}
            </div>

            <div class="col s12 xl6">
                {if $path_valid}
                    {if $showOptions eq 1}
                        <h3>{t}Options{/t}</h3>
                        {$properties}
                    {/if}
                {/if}
            </div>
        </div>
    </div>
</div>

<div class="plugin-actions card-action">
    {if $path_valid}
        {render acl=$acl}
            <button class="btn-small primary" type="submit" name="SavePPD">{msgPool type=applyButton}</button>
        {/render}
    {/if}

    <button class="btn-small primary" type="submit" name="ClosePPD">{msgPool type=cancelButton}</button>
</div>

<input type="hidden" name="PPDDisSubmitted" value="1">
