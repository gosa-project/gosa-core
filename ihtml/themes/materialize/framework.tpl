<body>
    {$php_errors}
    <nav>
        <div class="nav-wrapper gonicus-color ">
            <a href="main.php?global_check=1" class="brand-logo" alt="GOsa²">
                <img src="themes/materialize/images/logo.png">
            </a>
            <ul class="right">
                <li class="mg2">
                    <div class='lifetime'>
                        <canvas id="sTimeout" width="22" height="22"
                            title="{$sessionLifetime}|{t}Session expires in %d!{/t}"></canvas>
                    </div>
                </li>
                <li class="mg2">
                    {$loggedin}
                </li>
                <li>
                    <form action='logout.php' name='logoutframe' method='post' enctype='multipart/form-data'>
                        <i class="material-icons md-18 md-dark white-text pointer mg2" title='{t}Log out{/t}'
                            onclick="return question('{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}',
                                    'logout.php?forcedlogout');">logout</i>
                        <input type='hidden' name='forcedlogout' value='1'>
                        <input type="hidden" name="php_c_check" value="1">
                    </form>
                </li>
                <li><a class="dropdown-nav-more" data-target="more-options"><i class="material-icons">more_vert</i></a></li>
                <ul id="more-options" class="dropdown-content">
                    <form action='logout.php' name='logoutframe' method='post' enctype='multipart/form-data'>
                        <li title='{t}Log out{/t}' onclick="return question('{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}', 'logout.php?forcedlogout');">
                            <span class="material-icons">logout</span> Logout
                            <input type='hidden' name='forcedlogout' value='1'>
                            <input type="hidden" name="php_c_check" value="1">
                        </li>
                    </form>
                    <li>{$pathMenu}</li>
                </ul>
            </ul>
        </div>
    </nav>

    <form action='main.php{$plug}' id='mainform' name='mainform' method='post' enctype='multipart/form-data'>
        <div class="row">

            {if $hideMenus}
            <div class='col s12'>
                <div class="card">
                    {$contents}
                    {$msg_dialogs}
                </div>
            </div>
            {else}

            <!-- Breadcrumb -->
            {$breadcrumb}

            <div class="col s12 content-area">
                <!-- Menü -->
                <div class="col s1 menu">
                    <ul class="collapsible">
                        {$menu}
                    </ul>
                </div>

                {$msg_dialogs}

                <!-- Plugin Content-->
                <div class="col s11 content">
                    <div class="breadcrumb-nav" style="display: none">{$breadcrumb}</div>
                    <div class="card {if $noMenuMode}-noMenu{/if}">
                        <div class="card-content">
                            {$contents}
                        </div>
                    </div>
                </div>
            </div>
        </div>

        {/if}

        {if $channel != ""}
        <input type="hidden" name="_channel_" value="{$channel}">
        {/if}

        {$errors}
        {$focus}
        <input type="hidden" name="php_c_check" value="1">
    </form>

    <!-- Automatic logout when session is expired -->
    <script type='text/javascript'>
        setTimeout(function logout() {
            document.location = 'logout.php';
        }, { $sessionLifetime } * 1000);


        // Append change handler to all input fields.
        if ($('pluginModified') != null && $('pluginModified').value == 0) {
            for (i = 0; i < document.forms.length; i++) {
                for (e = 0; e < document.forms[i].elements.length; e++) {
                    var ele = document.forms[i].elements[e];
                    Event.observe(ele, 'change',
                        function () {
                            $('pluginModified').value |= 1;
                        });
                }
            }
        }
    </script>

    <script type="text/javascript" src="themes/materialize/js/materialize.js"></script>

    <script type="text/javascript">
        document.addEventListener('DOMContentLoaded', function () {
            var elems = document.querySelectorAll('.collapsible');
            var instances = M.Collapsible.init(elems, {});
        });
    </script>

</body>
