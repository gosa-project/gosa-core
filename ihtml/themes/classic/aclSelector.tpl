<input type='hidden' name='acl_dummy_0_0_0' value='1'>
{foreach $aclObjects as $obj}
    <table summary='{$obj.name}' style='width:100%;border:1px solid #A0A0A0' cellspacing=0 cellpadding=2>
        <tr>
            <td style='background-color:{if $obj.expand}#C8C8FF{else}#C8C8C8{/if};height:1.8em;' colspan=2>
                <b>{t}Object{/t}: {$obj.name}</b>
            </td>
            <td align='right' style='background-color:{if $obj.expand}#C8C8FF{else}#C8C8C8{/if};height:1.8em;'>
                {if $obj.splist}
                    <button type='button' onclick="$('{$obj.tname}').toggle();">{t}Show/hide advanced settings{/t}</button>
                {/if}
            </td>
        </tr>
        <tr>
            <td style='background-color:#E0E0E0' colspan=2>
                {if $aclWriteable}
                    <input id='acl_{$obj.tname}_0_c' type='checkbox' name='acl_{$obj.key}_0_c' {if $obj.overall.c}checked{/if}>
                    <label for='acl_{$obj.tname}_0_c'>{t}Create objects{/t}</label>
                    <input id='acl_{$obj.tname}_0_m' type='checkbox' name='acl_{$obj.key}_0_m' {if $obj.overall.m}checked{/if}>
                    <label for='acl_{$obj.tname}_0_m'>{t}Move objects{/t}</label>
                    <input id='acl_{$obj.tname}_0_d' type='checkbox' name='acl_{$obj.key}_0_d' {if $obj.overall.d}checked{/if}>
                    <label for='acl_{$obj.tname}_0_d'>{t}Remove objects{/t}</label>
                    {if $obj.selfModify}
                        <input id='acl_{$obj.tname}_0_s' type='checkbox' name='acl_{$obj.key}_0_s' {if $obj.overall.s}checked{/if}>
                        <label for='acl_{$obj.tname}_0_s'>{t}Restrict changes to user's own object{/t}</label>
                    {/if}
                {else}
                    <input type='checkbox' disabled name='dummy_c' {if $obj.overall.c}checked{/if}>{t}Create objects{/t}
                    <input type='checkbox' disabled name='dummy_m' {if $obj.overall.m}checked{/if}>{t}Move objects{/t}
                    <input type='checkbox' disabled name='dummy_d' {if $obj.overall.d}checked{/if}>{t}Remove objects{/t}
                    {if $obj.selfModify}
                        <input type='checkbox' disabled name='dummy_s' {if $obj.overall.s}checked{/if}>{t}Restrict changes to user's own object{/t}
                    {/if}
                {/if}
            </td>
            <td style='background-color:#D4D4D4'>
                &nbsp;{t}Complete object{/t}:
                {if $aclWriteable}
                    <input id='acl_{$obj.tname}_0_r' type='checkbox' name='acl_{$obj.key}_0_r' {if $obj.overall.r}checked{/if}>
                    <label for='acl_{$obj.tname}_0_r'>{t}read{/t}</label>
                    <input id='acl_{$obj.tname}_0_w' type='checkbox' name='acl_{$obj.key}_0_w' {if $obj.overall.w}checked{/if}>
                    <label for='acl_{$obj.tname}_0_w'>{t}write{/t}</label>
                {else}
                    <input disabled type='checkbox' name='dummy_r' {if $obj.overall.r}checked{/if}>{t}read{/t}
                    <input disabled type='checkbox' name='dummy_w' {if $obj.overall.w}checked{/if}>{t}write{/t}
                {/if}
            </td>
        </tr>
        <tr id='tr_{$obj.tname}' style='vertical-align:top;height:0px;'>
            <td colspan=3>
                <div id='{$obj.tname}' style='overflow:hidden; display:none;vertical-align:top;width:100%;'>
                    <table style='width:100%;' summary='{$obj.name}'>
                        {foreach $obj.attributes as $attr name=attr}
                            {if $smarty.foreach.attr.iteration % 3 == 1}
                                <tr>
                            {/if}
                            <td style='border-top:1px solid #A0A0A0;{if $smarty.foreach.attr.iteration % 3 != 0}border-right:1px solid #A0A0A0;{/if}width:33%'>
                                <b>{$attr.dsc}</b> ({$attr.key})<br>
                                {if $aclWriteable}
                                    <input id='acl_{$obj.tname}_{$attr.key}_r' type='checkbox' name='acl_{$obj.key}_{$attr.key}_r' {if $attr.r}checked{/if}>
                                    <label for='acl_{$obj.tname}_{$attr.key}_r'>{t}read{/t}</label>
                                    <input id='acl_{$obj.tname}_{$attr.key}_w' type='checkbox' name='acl_{$obj.key}_{$attr.key}_w' {if $attr.w}checked{/if}>
                                    <label for='acl_{$obj.tname}_{$attr.key}_w'>{t}write{/t}</label>
                                {else}
                                    <input disabled type='checkbox' name='dummy_r_{$attr.key}' {if $attr.r}checked{/if}>{t}read{/t}
                                    <input disabled type='checkbox' name='dummy_w_{$attr.key}' {if $attr.w}checked{/if}>{t}write{/t}
                                {/if}
                            </td>
                            {if $smarty.foreach.attr.iteration % 3 == 0 || $smarty.foreach.attr.last}
                                {if $smarty.foreach.attr.last && $smarty.foreach.attr.iteration % 3 != 0}
                                    {assign var=filled value=$smarty.foreach.attr.iteration % 3}
                                    {assign var=missing value=3-$filled}
                                    {section name=fill loop=$missing}
                                        <td style='border-top:1px solid #A0A0A0;width:33%'>&nbsp;</td>
                                    {/section}
                                {/if}
                                </tr>
                            {/if}
                        {/foreach}
                    </table>
                </div>
            </td>
        </tr>
    </table>
    <br/>
{/foreach}