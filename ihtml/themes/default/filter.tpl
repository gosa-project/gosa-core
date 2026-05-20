<input type='hidden' name='FILTER_PID' value='{$pid}'>

<div class='filtermenu' id='filtermenu'>
    <a class='dropdown-filter' data-target='filter-dropdown'>
        <i class='material-icons'>filter_alt</i>
        <i class='material-icons'>arrow_drop_down</i>
    </a>

    <ul class='dropdown-content' id='filter-dropdown'>
        <li>
            <ul class='collection'>
                {* Built-in filters *}
                {foreach from=$xmlSearches key=tag item=conf}
                    {if $tag == $search}
                        <li class='collection-item'>
                            <a href='#' class='active'>
                                <i class='material-icons'>checked</i>
                                {t}{$conf.label}{/t}
                            </a>
                        </li>
                    {else}
                        <li class='collection-item'>
                            <a href='#' onClick='$("act").value="filter-{$tag}";$("exec_act").click();'>
                                {t}{$conf.label}{/t}
                            </a>
                        </li>
                    {/if}
                {/foreach}

                {* User defined filters *}
                {foreach from=$userFilters key=tag item=conf name=userFilterLoop}
                    <li {if $smarty.foreach.userFilterLoop.first}class='divider' tabindex='-1'{/if}></li>
                    <li class='collection-item'>
                        <div>
                            {t}{$conf.description}{/t}
                            {if $tag == $search}
                                <a class='secondary-content' href='#'>
                                    <i class='material-icons md-18'>checked</i>
                                </a>
                            {else}
                                <a class='secondary-content' href='#' onClick='$("act").value="filter-{$tag}";$("exec_act").click();'></a>
                            {/if}
                        </div>
                    </li>
                {/foreach}

                {* Render scope if set to auto *}
                {if $scopeMode == 'auto'}
                    <li class='divider' tabindex='-1'></li>
                    <li class='collection-item'>
                        <a href='#' class='search-subtree{if $scope != 'one'} active{/if}'
                            onClick='$("act").value="toggle-subtree";$("exec_act").click();'>
                            {if $scope != 'one'}
                                <i class='material-icons left'>checked</i>
                            {/if}
                            {t}Search in subtrees{/t}
                        </a>
                    </li>
                {/if}

                {* Edit filter menu *}
                <li class='divider' tabindex='-1'></li>
                <li class='collection-item'>
                    <a href='#' onClick='$("act").value="config-filter";$("exec_act").click();'>
                        <i class='material-icons'>settings</i>
                        {t}Edit filters{/t}...
                    </a>
                </li>
            </ul>
        </li>
    </ul>
</div>

<div class='search-field'>
    {$textfield nofilter}
    <button class='btn' type='submit' title='{t}Search{/t}'>
        <i class='material-icons md-18'>search</i>
    </button>
</div>