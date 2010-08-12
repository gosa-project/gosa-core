<h3>{t}Usage statistics{/t}</h3>

{if !$rpcConfigured}

    {t}This feature is disabled. To enable it you can use the GOsa properties plugin, just activate the remote procedure call (RPC) configuration flags.{/t}

{else if !$validRpcHandle}

    {t}Communication with the GOsa-backend failed. Please check the rpc configuration!{/t}

{else}

    <button name='transmitStatistics'>{t}Send{/t}</button>
    <button name='receiveStatistics'>{t}Receive{/t}</button>

    <hr>

    <img src='plugins/statistics/getGraph.php?id={$graphID_1}'>

{/if}
