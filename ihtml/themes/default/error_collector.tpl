<div id="error-collector-modal" class="modal">
    <div class="modal-content">
        <h4>
            {t escape=no
            1='<b>' 2='</b>'
            }There was a %1problem%2 while generating this GOsa page!{/t}
        </h4>

        <h5 class="red-text text-darken-3 error-count-container">
            <i class="material-icons left">error</i>
            {t}PHP errors detected!{/t}
            ({$error_collector.items|@count})
        </h5>

        <p class="grey-text">
            {t}GOsa version:{/t}
            {$error_collector.version|escape}
        </p>

        <ul class="collapsible">
            {foreach $error_collector.items as $index => $error}
                <li>
                    <div class="collapsible-header error-count-container">
                        <i class="material-icons">zoom_out_map</i>
                        <span class="truncate">
                            #{$index+1}: {$error.errstr|escape}
                        </span>
                        <span class="grey-text collapsible-file-path">
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
                                            <td>{$trace_entry.args|@implode:", "|escape}</td>
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
            {t}Click on the elements to open them.{/t}
        </p>
    </div>
    <div class="modal-footer">
        <a href="#!" class="modal-close waves-effect waves-green btn-flat">
            {t}OK{/t}
        </a>
    </div>
</div>

<a class="waves-effect waves-light btn-large red" id="floating-error-btn">
    <i class="material-icons left">error</i>
    {t
    1={$error_collector.items|@count}
    }%1 error(s) detected!{/t}
</a>

<script>
    document.addEventListener('DOMContentLoaded', function() {
        // Initialize the collapsibles
        M.Collapsible.init(document.querySelectorAll('.collapsible'));

        // Initialize the modal
        var elems = document.querySelectorAll('.modal');
        var instances = M.Modal.init(elems);

        // Open the modal
        var modalElem = document.getElementById('error-collector-modal');
        var modalInstance = M.Modal.getInstance(modalElem);

        document.getElementById('floating-error-btn').addEventListener('click', function() {
            modalInstance.open();
        });
    });
</script>

<style>
    .collapsible-file-path { margin-left: 10px; }

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

    #floating-error-btn {
        position: fixed;
        bottom: 24px;
        left: 24px;
        z-index: 1000;
    }

    #error-collector-modal { width: 60%; }
</style>