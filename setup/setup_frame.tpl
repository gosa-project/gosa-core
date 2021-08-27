 <body>
  {$php_errors}
  <div class='title-bar'>
   <ul>
    <li><img src='themes/default/images/logo.png' alt='GOsa'></li>
    <li class='right' style='padding-top:10px;padding-right:5px'>{$version}</li>
   </ul>
  </div>

    <nav>
        <div class="nav-wrapper gonicus-color ">
            <a href="#" class="brand-logo" alt="GOsa²">
                <img src="themes/materialize/images/logo.png">
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

    <script type="text/javascript" defer src="themes/materialize/js/pickerLanguage.js"></script>
    <script type="text/javascript" src="themes/materialize/js/materialize.js"></script>
    <script type="text/javascript" defer src="themes/materialize/js/eventListener.js"></script>
    <script type="text/javascript" defer src="themes/materialize/js/main.js"></script>
    <script type="text/javascript" defer src="themes/materialize/js/functions.js"></script>
 </body>
</html>
