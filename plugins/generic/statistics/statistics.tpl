<h3>{t}Usage statistics{/t}</h3>

{if !$rpcConfigured}

    {t}This feature is disabled. To enable it you can use the GOsa properties plugin, just activate the remote procedure call (RPC) configuration flags.{/t}

{else if !$validRpcHandle}

    {t}Communication with the GOsa-backend failed. Please check the rpc configuration!{/t}

{else}

    <button name='transmitStatistics'>{t}Send{/t}</button>
    <button name='receiveStatistics'>{t}Receive{/t}</button>

    <hr>
    <table width="100%">
        <tr>
            <td>{t}Start{/t}</td>
            <td>
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
        </tr>
        <tr>
            <td>{t}Stop{/t}</td>
            <td>
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
        </tr>
    </table>

    <img src='plugins/statistics/getGraph.php?id={$graphID_1}'>
    <img src='plugins/statistics/getGraph.php?id={$graphID_2}'>

{/if}
