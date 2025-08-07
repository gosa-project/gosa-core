
<h3>{t}Usage statistics{/t}</h3>
{if $registrationNotWanted}
    
    {t}This feature is disabled. To enable it you have to register GOsa, you can initiate a registration using the dash-board plugin.{/t}
    <button type='button' onClick="openPlugin({$dashBoardId});">{t}Dash board{/t}</button>

{else if !$instanceRegistered}

    {t}This feature is disabled. To enable it you have to register GOsa, you can initiate a registration using the dash-board plugin.{/t}

    <button type='button' onClick="openPlugin({$dashBoardId});">{t}Dash board{/t}</button>

{else if !$serverAccessible || !$validRpcHandle || $rpcHandle_Error}

    {t}Communication with the GOsa-backend failed. Please check the RPC configuration!{/t}

{else}

    {if $unsbmittedFiles != 0}
        {$unsbmittedFilesMsg}
        <button name='transmitStatistics'>{t}Send{/t}</button>
        <hr>
    {/if}

    <table>
        <tr>
            <td><b>{t}Generate report for{/t}:</b></td>
            <td style='width:220px;'>
                 <input type="text" id="graph1DatePicker1" name="graph1DatePicker1" class="date" value="{$graph1DatePicker1}">
                 <script type="text/javascript">
                  {literal}
                   var datepicker  = new DatePicker(
                         { relative : 'graph1DatePicker1',
                           language : '{/literal}{$lang}{literal}',
                           keepFieldEmpty : true,
                           enableCloseEffect : false,
                           enableShowEffect : false });
                  {/literal}
                 </script>
            </td>
            <td style='width:220px;'>
                <input type="text" id="graph1DatePicker2" name="graph1DatePicker2" class="date" value="{$graph1DatePicker2}">
                <script type="text/javascript">
                 {literal}
                  var datepicker  = new DatePicker(
                        { relative : 'graph1DatePicker2',
                          language : '{/literal}{$lang}{literal}',
                          keepFieldEmpty : true,
                          enableCloseEffect : false,
                          enableShowEffect : false });
                 {/literal}
                </script>
            </td>
            <td>
                <button name='receiveStatistics'>{t}Update{/t}</button>
            </td>
        </tr>
    </table>
    <hr>

    <table>
        <tr>
            <td>
                {if isset($staticChart1_ID) && $staticChart1_ID}
                    <img src='plugins/statistics/getGraph.php?id={$staticChart1_ID}'>
                {else}
                    <div style='height:200px; width: 400px;'>
                        <i>{t}No statistic data for given period{/t}</i>
                    </div>
                {/if}
            </td>
            <td>
                {if isset($staticChart2_ID) && $staticChart2_ID}
                    <img src='plugins/statistics/getGraph.php?id={$staticChart2_ID}'>
                {else}
                    <div style='height:200px; width: 400px;'>
                        <i>{t}No statistic data for given period{/t}</i>
                    </div>
                {/if}
            </td>
        </tr>
    </table>

    {if isset($curGraphID) && $curGraphID}
        <hr>
        <b>{t}Select report type{/t}:</b>&nbsp;
        <select name='selectedGraphType' onChange="document.mainform.submit();" size='1'>
            {html_options options=$availableGraphs selected=$selectedGraphType}
        </select>
        {$curGraphOptions}
        <table>
            <tr>
                <td>
                    <input type='hidden' name='currentGraphPosted' value='1'>
                    <img src='plugins/statistics/getGraph.php?id={$curGraphID}'>
                </td>
            </tr>
            <tr>
                <td>
                    {$curSeriesSelector}
                </td>
            </tr>
        </table>
    {/if}
{/if}
