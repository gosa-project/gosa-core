<h3>{t}Usage statistics{/t}</h3>

{if !$rpcConfigured}

    {t}This feature is disabled. To enable it you can use the GOsa properties plugin, just activate the remote procedure call (RPC) configuration flags.{/t}

{else if !$validRpcHandle}

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

    {if $graphID_1}
        <img src='plugins/statistics/getGraph.php?id={$graphID_1}'>
    {/if}
    {if $graphID_2}
        <img src='plugins/statistics/getGraph.php?id={$graphID_2}'>
    {/if}
    {if $graphID_3}
        <img src='plugins/statistics/getGraph.php?id={$graphID_3}'>
    {/if}
    {if $graphID_4}
        <img src='plugins/statistics/getGraph.php?id={$graphID_4}'>
    {/if}
    {if $graphID_5}
        <img src='plugins/statistics/getGraph.php?id={$graphID_5}'>
    {/if}
    {if $graphID_6}
        <img src='plugins/statistics/getGraph.php?id={$graphID_6}'>
    {/if}

{/if}
