  <body>
   {$php_errors}
<<<<<<< HEAD
   <div class='title-bar'>
    <ul>
     <li>{$logo}</li>
     <li class='right table-wrapper'>
       <div class='logout-label'>
         <form action='logout.php' name='logoutframe' method='post' enctype='multipart/form-data'>
          <div style='cursor:pointer' title='{t}Log out{/t}' onClick="
                    return question('{t}You are currently editing a database entry. Do you want to dismiss the changes?{/t}',
            'logout.php?forcedlogout');">{image path="{$logoutimage}"}</div>
          <input type='hidden' name='forcedlogout' value='1'>
          <input type="hidden" name="php_c_check" value="1">
         </form>
       </div>
     </li>
     <li class='right table-wrapper'>
      <div class='logged-in-label'>{$loggedin}</div>
     </li>
     <li class='right table-wrapper'>
       <div class='logout-label'>
        <canvas id="sTimeout" width="22" height="22" title="{$sessionLifetime}|{t}Session expires in %d!{/t}"></canvas>
       </div>
     </li>
    </ul>
   </div>
   <form action='main.php{$plug}' id='mainform' name='mainform' method='post' enctype='multipart/form-data'>

   {if $hideMenus}

    {$contents}
    {$msg_dialogs}

   {else}

    {$menu}
    {$msg_dialogs}
    <div class='plugin-area{if $noMenuMode}-noMenu{/if}'>
      {$pathMenu}
      {$contents}
    </div>
   {/if}
=======
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

>>>>>>> gosa-core_v2.8

   {if $channel != ""}
   <input type="hidden" name="_channel_" value="{$channel}">
   {/if}

   {$errors}
   {$focus}
   <input type="hidden" name="php_c_check" value="1">
  </form>
  
  <!-- Automatic logout when session is expired -->
  <script type='text/javascript'>
<<<<<<< HEAD
   function logout()
   {
    document.location = 'logout.php';
   }
   logout.delay({$sessionLifetime});
=======
        setTimeout(function logout() {
            document.location = 'logout.php';
        }, {$sessionLifetime} * 1000);
>>>>>>> gosa-core_v2.8


   // Append change handler to all input fields. 
   if($('pluginModified') != null && $('pluginModified').value == 0){
       for(i=0;i<document.forms.length;i++){
           for(e=0;e<document.forms[i].elements.length;e++){
               var ele = document.forms[i].elements[e];
               Event.observe(ele, 'change', 
                    function () {
                        $('pluginModified').value |= 1;
                    });
           }
       }
   }
<<<<<<< HEAD

  </script>    
 </body>
</html>
=======
    </script>

    <script type="text/javascript" src="themes/default/js/materialize.js"></script>
    <script type="text/javascript" src="themes/default/js/dropdown_material_fix.js"></script>
    <script type="text/javascript" defer src="themes/default/js/eventListener.js"></script>
    <script type="text/javascript" defer src="themes/default/js/main.js"></script>
    <script type="text/javascript" defer src="themes/default/js/functions.js"></script>
</body>
>>>>>>> gosa-core_v2.8
