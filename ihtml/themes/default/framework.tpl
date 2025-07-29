<body>
    {$php_errors}
    {$msg_dialogs}
    <nav class="accent">
        <div class="nav-wrapper">
            <a href="main.php?global_check=1" class="brand-logo" alt="GOsa²">
                <img src="themes/default/images/logo.png">
            </a>
            <ul class="nav-menu">
                <li>
                    <div class='lifetime'>
                        <canvas id="sTimeout" width="22" height="22"
                            title="{$sessionLifetime}|{t}Session expires in %d!{/t}"></canvas>
                    </div>
                </li>

                <li><a class="dropdown-nav-more" data-target="more-options"><i class="material-icons">more_vert</i></a>
                </li>
                <ul id="more-options" class="dropdown-content">
                    {$pathMenu}
                    <form action='logout.php' name='logoutframe' method='post' enctype='multipart/form-data'>
                        <li title='{t}Log out{/t}'
                            onclick="return question('{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}', 'logout.php?forcedlogout');">
                            <span class="material-icons">logout</span> Logout {$userName}
                            <input type='hidden' name='forcedlogout' value='1'>
                            <input type="hidden" name="php_c_check" value="1">
                        </li>
                    </form>
                </ul>
            </ul>
        </div>
    </nav>

    <form action='main.php{$plug}' id='mainform' name='mainform' method='post' enctype='multipart/form-data'>
        <div class="row">

            {if $hideMenus}
            <div class='col s12'>
                <div class="card first-level">
                    <div class="card-content first-level">
                        {$contents}
                    </div>
                </div>
            </div>
            {else}
            <!-- Breadcrumb -->
            {$breadcrumb}
            <div class="content-area {if $noMenuMode}noMenu{/if}">
                <!-- Menü -->
                {if !$noMenuMode}
                <div class="menu">
                    <ul class="collapsible expandable">
                        {$menu}
                    </ul>
                </div>
                {/if}

                <!-- Plugin Content-->
                <div class="content{if $noMenuMode}-noMenu{/if}">
                    <div class="card first-level">
                        <div class="card-content first-level">
                            {$contents}
                        </div>
                    </div>
                </div>
            </div>
            {/if}
        </div>


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
        }, {$sessionLifetime} * 1000);


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

    <script type="text/javascript" src="themes/default/js/materialize.js"></script>
    <script type="text/javascript" src="themes/default/js/dropdown_material_fix.js"></script>
    <script type="text/javascript" defer src="themes/default/js/eventListener.js"></script>
    <script type="text/javascript" defer src="themes/default/js/main.js"></script>
    <script type="text/javascript" defer src="themes/default/js/functions.js"></script>
</body>
