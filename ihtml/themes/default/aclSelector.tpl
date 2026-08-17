<input type='hidden' name='acl_dummy_0_0_0' value='1'>
{foreach $aclObjects as $obj}
    <div class='acl-object-wrapper'>
        <div class='object-head-wrapper'>
            <div class='object-name'>
                {t}Object{/t}: {$obj.name}
            </div>
            {if $obj.splist}
            <div class='advanced-settings'>
                <button class='btn-extra-small' type='button' onclick="$('{$obj.tname}').toggle();">
                    {t}Show/hide advanced settings{/t}
                </button>
            </div>
            {/if}
        </div>
    <div class='object-body-wrapper'>
        <div class='row'>
            <div class='col s9 options'>
                {if $aclWriteable}
                    <label>
                        <p>
                            <input type='checkbox' id='acl_{$obj.tname}_0_c' name='acl_{$obj.key}_0_c' {if $obj.overall.c}checked{/if}>
                            <span>{t}Create objects{/t}</span>
                        </p>
                    </label>
                    <label>
                        <p>
                            <input type='checkbox' id='acl_{$obj.tname}_0_m' name='acl_{$obj.key}_0_m' {if $obj.overall.m}checked{/if}>
                            <span>{t}Move objects{/t}</span>
                        </p>
                    </label>
                    <label>
                        <p>
                            <input type='checkbox' id='acl_{$obj.tname}_0_d' name='acl_{$obj.key}_0_d' {if $obj.overall.d}checked{/if}>
                            <span>{t}Remove objects{/t}</span>
                        </p>
                    </label>
                    {if $obj.selfModify}
                    <label>
                        <p>
                            <input type='checkbox' id='acl_{$obj.tname}_0_s' name='acl_{$obj.key}_0_s' {if $obj.overall.s}checked{/if}>
                            <span>{t}Restrict changes to user's own object{/t}</span>
                        </p>
                    </label>
                    {/if}
                {else}
                    <label>
                        <p>
                            <input type='checkbox' name='dummy_c' disabled {if $obj.overall.c}checked{/if}>
                            <span>{t}Create objects{/t}</span>
                        </p>
                    </label>
                    <label>
                        <p>
                            <input type='checkbox' name='dummy_m' disabled {if $obj.overall.m}checked{/if}>
                            <span>{t}Move objects{/t}</span>
                        </p>
                    </label>
                    <label>
                        <p>
                            <input type='checkbox' name='dummy_d' disabled {if $obj.overall.d}checked{/if}>
                            <span>{t}Remove objects{/t}</span>
                        </p>
                    </label>
                    {if $obj.selfModify}
                    <label>
                        <p>
                            <input type='checkbox' name='dummy_s' disabled {if $obj.overall.s}checked{/if}>
                            <span>{t}Restrict changes to user's own object{/t}</span>
                        </p>
                    </label>
                    {/if}
                {/if}
            </div>
            <div class='col s3 options'>
                <span class='selection-label'>{t}Complete object{/t}:</span>
                {if $aclWriteable}
                    <label>
                        <p>
                            <input type='checkbox' id='acl_{$obj.tname}_0_r' name='acl_{$obj.key}_0_r' {if $obj.overall.r}checked{/if}>
                            <span>{t}read{/t}</span>
                        </p>
                    </label>
                    <label>
                        <p>
                            <input type='checkbox' id='acl_{$obj.tname}_0_w' name='acl_{$obj.key}_0_w' {if $obj.overall.w}checked{/if}>
                            <span>{t}write{/t}</span>
                        </p>
                    </label>
                {else}
                    <label>
                        <p>
                            <input type='checkbox' name='dummy_r' disabled {if $obj.overall.r}checked{/if}>
                            <span>{t}read{/t}</span>
                        </p>
                    </label>
                    <label>
                        <p>
                            <input type='checkbox' name='dummy_w' disabled {if $obj.overall.w}checked{/if}>
                            <span>{t}write{/t}</span>
                        </p>
                    </label>
                {/if}
            </div>
        </div>
    </div>
    </div>
    <div class='acl_container' id='{$obj.tname}' style='overflow:hidden; display:none;'>
        <div class='row'>
            {foreach $obj.attributes as $attr}
            <div class='col s6 xl4'>
                <div class='option-grp'>
                    <div class='option-name'>
                        {$attr.dsc} ({$attr.key})
                    </div>
                <div class='options'>
                    {if $aclWriteable}
                        <label>
                            <p>
                                <input type='checkbox' id='acl_{$obj.tname}_{$attr.key}_r' name='acl_{$obj.key}_{$attr.key}_r' {if $attr.r}checked{/if}>
                                <span>{t}read{/t}</span>
                            </p>
                        </label>
                        <label>
                            <p>
                                <input type='checkbox' id='acl_{$obj.tname}_{$attr.key}_w' name='acl_{$obj.key}_{$attr.key}_w' {if $attr.w}checked{/if}>
                                <span>{t}write{/t}</span>
                            </p>
                        </label>
                    {else}
                        <label>
                            <p>
                                <input type='checkbox' name='dummy_r_{$attr.key}' disabled {if $attr.r}checked{/if}>
                                <span>{t}read{/t}</span>
                            </p>
                        </label>
                        <label>
                            <p>
                                <input type='checkbox' name='dummy_w_{$attr.key}' disabled {if $attr.w}checked{/if}>
                                <span>{t}write{/t}</span>
                            </p>
                        </label>
                    {/if}
                </div>
            </div>
        </div>
            {/foreach}
        </div>
    </div>
{/foreach}