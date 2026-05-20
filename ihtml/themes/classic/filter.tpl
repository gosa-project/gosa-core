<input type='hidden' name='FILTER_PID' value='{$pid}'>
<table class='filter-wrapper' summary='{t}Search filter{/t}'>
    <tr>
        <td>
            <div id='filtermenu'>
                {* Load shortcut *}
                <ul class='level1' id='filter-root'>
                    <li>
                        {strip}
                        <a href='#'>
                            <div class='img' style='background-position:-292px -182px;width:16px;height:16px'></div>
                            <div class='img' style='background-position:-373px -309px;width:7px;height:4px'></div>
                        </a>
                        {/strip}

                        {* Build ul/li list *}
                        <ul class='level2'>

                            {* Build in filters *}
                            {foreach from=$xmlSearches key=tag item=conf}
                                {if $tag == $search}
                                    <li>
                                        {strip}
                                        <a href='#'>
                                            <div class='img' style='background-position:-292px -86px;width:16px;height:16px'></div>&nbsp;{t}{$conf.label}{/t}
                                        </a>
                                        {/strip}
                                    </li>
                                {else}
                                    <li>
                                        {strip}
                                        <a href='#' onClick='$("act").value="filter-{$tag}";$("exec_act").click();'>
                                            <div class='img' style='background-position:-292px -150px;width:16px;height:16px'></div>&nbsp;{t}{$conf.label}{/t}
                                        </a>
                                        {/strip}
                                    </li>
                                {/if}
                            {/foreach}

                            {* User defined filters *}
                            {foreach from=$userFilters key=tag item=conf name=userFilterLoop}
                                {if $tag == $search}
                                    <li {if $smarty.foreach.userFilterLoop.first}style='border-top:1px solid #AAA'{/if}>
                                        {strip}
                                        <a href='#'>
                                            <div class='img' style='background-position:-292px -86px;width:16px;height:16px'></div>&nbsp;{t}{$conf.description}{/t}
                                        </a>
                                        {/strip}
                                    </li>
                                {else}
                                    <li {if $smarty.foreach.userFilterLoop.first}style='border-top:1px solid #AAA'{/if}>
                                        {strip}
                                        <a href='#' onClick='$("act").value="filter-{$tag}";$("exec_act").click();'>
                                            <div class='img' style='background-position:-292px -150px;width:16px;height:16px'></div>&nbsp;{t}{$conf.description}{/t}
                                        </a>
                                        {/strip}
                                    </li>
                                {/if}
                            {/foreach}

                            {* Render scope if set to auto *}
                            {if $scopeMode == 'auto'}
                                <li style='border-top:1px solid #AAA'>
                                    {strip}
                                    <a href='#' onClick='$("act").value="toggle-subtree";$("exec_act").click();'>
                                        {if $scope == 'one'}
                                            <div class='img' style='background-position:-292px -150px;width:16px;height:16px'></div>
                                        {else}
                                            <div class='img' style='background-position:-292px -86px;width:16px;height:16px'></div>
                                        {/if}
                                        &nbsp;{t}Search in subtrees{/t}
                                    </a>
                                    {/strip}
                                </li>
                            {/if}

                            {* Edit filter menu *}
                            <li style='border-top:1px solid #AAA'>
                                {strip}
                                <a href='#' onClick='$("act").value="config-filter";$("exec_act").click();'>
                                    <div class='img' style='background-position:-292px -118px;width:16px;height:16px'></div>&nbsp;{t}Edit filters{/t}...
                                </a>
                                {/strip}
                            </li>
                        </ul>
                    </li>
                </ul>

                <script type="text/javascript" language="JavaScript">
                    var menu2; menu2= new Menu("filter-root", "menu2", configMenu)
                </script>
            </div>
        </td>
        <td>
            <div class='search-filter'>
                <input class='filter_textfield' placeholder='{t}Search{/t}...' id='{$tag}' name='{$tag}' type='text' size='30' maxlength='30' value='{$value}'>

                {if $hasAutocomplete}
                <div id='autocomplete{$tag}' class='autocomplete'></div>
                <script type='text/javascript'>
                    new Ajax.Autocompleter('{$tag}', 'autocomplete{$tag}', 'autocomplete.php', { minChars: {$characters}, frequency: {$frequency} });
                </script>
                {/if}
            </div>
            &nbsp;<button class='search-filter' type='submit' title='{t}Search{/t}'>
                <div class='img' style='background-position:-292px -214px;width:16px;height:16px'></div>
            </button>
        </td>
    </tr>
</table>