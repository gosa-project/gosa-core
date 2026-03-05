<div style="display: flex; align-items: center; justify-content: space-between; padding: 10px 30px">
    <div style="display: flex; align-items: center">
        <img src='images/toolbar-warning.png' style="margin-right: 20px">
        <span>Generating this page caused the PHP interpreter to raise some errors!</span>
    </div>
    <button onClick="$('errorbox').toggle();">Toggle details</button>
</div>

    <div id='errorbox' style='position:absolute; z-index:150; display: none;'>
    {foreach $error_collector.items as $index => $error}
        <table width="100%" cellspacing=0 style='color:white; border:2px solid red; background-color:#402005;' class="stacktrace-table">
            <tr>
                <td colspan=3><h1 style='color:white'>PHP error "{$error.errstr|escape}"</h1></td>
            </tr>
            {foreach $error.trace as $trace_entry}
                <tbody class="trace-pair">
                    <tr>
                        <td style='padding-left:20px' width="30%">
                            Traceback {$trace_entry.index|escape}:
                            {if $trace_entry.class}
                                Class: {$trace_entry.class|escape} / Function: {$trace_entry.function|escape}
                            {else}
                                Function: {$trace_entry.function|escape}
                            {/if}
                        </td>
                        <td>
                            File: {$trace_entry.file|escape} (Line {$trace_entry.line|escape})
                        </td>
                        <td width="10%">
                            Type:
                            {if $trace_entry.type == "->"}
                                method
                            {else if $trace_entry.type == "::"}
                                static
                            {else}
                                -
                            {/if}
                        </td>
                    </tr>
                    <tr>
                        <td colspan=3 style='padding-left:20px;'>Arguments: {$trace_entry.args|@implode:", "}</td>
                    </tr>
                </tbody>
            {/foreach}
        </table>
        {/foreach}
    </div>

<style>
    .trace-pair:nth-of-type(even) { background-color: #404040 }
    .stacktrace-table h1 { font-size: 20px; }
</style>