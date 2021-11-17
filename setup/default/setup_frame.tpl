 <body>
  {$php_errors}

    <nav class="accent">
        <div class="nav-wrapper primary ">
            <a href="#" class="brand-logo" alt="GOsa²">
                <img src="themes/default/images/logo.png">
            </a>
            <ul class="nav-menu">
                <li>
                    {$version}
                </li>
            </ul>
        </div>
    </nav>

    <form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
        {$msg_dialogs}

        {if isset($errors)}{$errors}{/if}
        <div class="container content-area setup">
            <div class="card first-level">
                <div class="card-content first-level">
                    <div class="header-wrapper">{$header}</div>
                    {$contents}

                    <div class="card-action">
                        {$bottom}
                    </div>
                </div>
            </div>
        </div>
        <input type='hidden' name='setup_goto_step' value=''>
    </form>

    <script type="text/javascript" src="themes/default/js/materialize.js"></script>
    <script type="text/javascript" defer src="themes/default/js/eventListener.js"></script>
    <script type="text/javascript" defer src="themes/default/js/main.js"></script>
    <script type="text/javascript" defer src="themes/default/js/functions.js"></script>
 </body>
</html>
