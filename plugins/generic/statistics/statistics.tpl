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
                {if isset($graphIDs.1) && $graphIDs.1}
                    <img src='plugins/statistics/getGraph.php?id={$graphIDs.1}'>
                {/if}
            </td>
            <td>
                {if isset($graphIDs.7) && $graphIDs.7}
                    <img src='plugins/statistics/getGraph.php?id={$graphIDs.7}'>
                {/if}
            </td>
        </tr>
    </table>

    <table>
        <tr>
            <td>
                {if isset($graphIDs.2) && $graphIDs.2}
                    <img src='plugins/statistics/getGraph.php?id={$graphIDs.2}'>
                    <input type='hidden' name='graphPosted_2' value='1'>
                {/if}
            </td>
            <td>
                {if isset($graphLegends.2) && $graphIDs.2}
                    {$graphLegends.2}
                {/if}
            </td>
        </tr>
    </table>
    {if isset($graphIDs.3) && $graphIDs.3}
        <img src='plugins/statistics/getGraph.php?id={$graphIDs.3}'>
    {/if}
    {if isset($graphIDs.4) && $graphIDs.4}
        <img src='plugins/statistics/getGraph.php?id={$graphIDs.4}'>
    {/if}
    {if isset($graphIDs.5) && $graphIDs.5}
        <img src='plugins/statistics/getGraph.php?id={$graphIDs.5}'>
    {/if}
    {if isset($graphIDs.6) && $graphIDs.6}
        <img src='plugins/statistics/getGraph.php?id={$graphIDs.6}'>
    {/if}

{/if}
