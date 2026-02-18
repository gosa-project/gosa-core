<div id="modal1" class="modal">
    <div class="modal-content">
        <h4>There was a <b>problem</b> while generating this GOsa page!</h4>

        <h5 class="red-text text-darken-3 error-count-container">
            <i class="material-icons left">error</i>
            PHP errors detected! ({$error_collector.items|@count})
        </h5>

        <p class="grey-text">
            GOsa Version: {$error_collector.version|escape}
        </p>

        <ul class="collapsible">
            {foreach $error_collector.items as $index => $error}
                <li>
                    <div class="collapsible-header error-count-container">
                        <i class="material-icons">zoom_out_map</i>
                        <span class="truncate" style="max-width: 75%;">
                            #{$index+1}: {$error.errstr|escape}
                        </span>
                        <span class="grey-text" style="margin-left: 10px;">
                            {$error.errfile|escape}:{$error.errline|escape}
                        </span>
                    </div>

                    <div class="collapsible-body">
                        {if $error.trace|@count > 0}
                            <h6>Stacktrace</h6>

                            <table class="highlight responsive-table stacktrace-table">
                                <thead>
                                    <tr>
                                        <th>#</th>
                                        <th>File</th>
                                        <th>Class::Function</th>
                                        <th>Args</th>
                                        <th>Type</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    {foreach $error.trace as $trace_entry}
                                        <tr>
                                            <td>{$trace_entry.index|escape}</td>
                                            <td>{$trace_entry.file|escape}:{$trace_entry.line|escape}</td>
                                            {if $trace_entry.class}
                                                <td>{$trace_entry.class|escape}::{$trace_entry.function|escape}</td>
                                            {else}
                                                <td>{$trace_entry.function|escape}</td>
                                            {/if}
                                            <td>{$trace_entry.args|@implode:", "}</td>
                                            <td>
                                                {if $trace_entry.type == "->"}
                                                    method
                                                {else if $trace_entry.type == "::"}
                                                    static
                                                {else}
                                                    -
                                                {/if}
                                            </td>
                                        </tr>
                                    {/foreach}
                                </tbody>
                            </table>
                        {/if}
                    </div>
                </li>
            {/foreach}
        </ul>
        <p class="grey-text">
            Click on the elements to open them.
        </p>
    </div>
    <div class="modal-footer">
        <a href="#!" class="modal-close waves-effect waves-green btn-flat">OK</a>
    </div>
</div>

<script>
    document.addEventListener('DOMContentLoaded', function() {
        var elems = document.querySelectorAll('.modal');
        var instances = M.Modal.init(elems);

        var modalElem = document.getElementById('modal1');
        var modalInstance = M.Modal.getInstance(modalElem);
        modalInstance.open();

        M.Collapsible.init(document.querySelectorAll('.collapsible'));
    });
</script>
<style>
    .error-count-container {
        display: flex;
        align-items: center;
    }
    .stacktrace-table th,
    .stacktrace-table td {
        padding: 8px 12px;
        vertical-align: top;
    }

    .stacktrace-table {
        table-layout: fixed;
        width: 100%;
    }
    .stacktrace-table th:nth-child(1),
    .stacktrace-table td:nth-child(1) { width: 50px; }

    .stacktrace-table th:nth-child(3),
    .stacktrace-table td:nth-child(3) { width: 170px; }

    .stacktrace-table th:nth-child(5),
    .stacktrace-table td:nth-child(5) { width: 80px; }
</style>