
<h3>{t}Usage statistics{/t}</h3>

{if !$rpcConfigured}

    {t}This feature is disabled. To enable it you can use the GOsa properties plugin, just activate the remote procedure call (RPC) configuration flags.{/t}

{else if !$validRpcHandle || $rpcHandle_Error}

    {t}Communication with the GOsa-backend failed. Please check the rpc configuration!{/t}

{else}

    {if $unsbmittedFiles != 0}
        {$unsbmittedFilesMsg}
        <button name='transmitStatistics'>{t}Send{/t}</button>
        <hr>
    {/if}

    <table>
        <tr>
            <td>{t}Generate report for{/t}</td>
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
            <td> - </td>
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
                <button name='receiveStatistics'>{t}Receive{/t}</button>
            </td>
        </tr>
    </table>
    <hr>

    <table>
        <tr>
            <td>
                {if isset($staticChart1_ID) && $staticChart1_ID}
                    <img src='plugins/statistics/getGraph.php?id={$staticChart1_ID}'>
                {/if}
            </td>
            <td>
                {if isset($staticChart2_ID) && $staticChart2_ID}
                    <img src='plugins/statistics/getGraph.php?id={$staticChart2_ID}'>
                {/if}
            </td>
        </tr>
    </table>

    {if isset($curGraphID) && $curGraphID}
        <select name='selectedGraphType' onChange="document.mainform.submit();" size='1'>
            {html_options options=$availableGraphs selected=$selectedGraphType}
        </select>
        <hr>
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
