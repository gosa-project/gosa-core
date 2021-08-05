 <body>
  {$php_errors}
  <div class='title-bar'>
   <ul>
    <li><img src='themes/default/images/logo.png' alt='GOsa'></li>
    <li class='right' style='padding-top:10px;padding-right:5px'>{$version}</li>
   </ul>
  </div>

  <form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
   {$msg_dialogs}

    
    <div class="row">
        <form action='setup.php' name='mainform' method='post' enctype='multipart/form-data'>
            {$msg_dialogs}
    
            {if isset($errors)}{$errors}{/if}
            <div class="container">
                <div class="row">
                    <div class="col s12">
                        <div class="card">
                            <div class="card-content">
                                <span class="card-title">{$header}</span>
                                {$contents}
                            </div>
                            <div class="card-action">
                                {$bottom}
                            </div>
                        </div>
                    </div>
                </div>
            </div>  
            <input type='hidden' name='setup_goto_step' value=''>
        </form>
    </div>
    
    <script type="text/javascript" src="themes/materialize/js/materialize.js"></script>
    <script type="text/javascript" src="themes/materialize/js/eventListener.js"></script>

 </body>
</html>
