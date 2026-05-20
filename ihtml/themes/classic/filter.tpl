<input type='hidden' name='FILTER_PID' value='{$pid}'>
<table class='filter-wrapper' summary='{t}Search filter{/t}'>
    <tr>
        <td>{$filterMenu nofilter}</td>
        <td>
            <div class='search-filter'>
                {$textfield nofilter}
            </div>
            &nbsp;<button class='search-filter' type='submit' title='{t}Search{/t}'>
                <div class='img' style='background-position:-292px -214px;width:16px;height:16px'></div>
            </button>
        </td>
    </tr>
</table>