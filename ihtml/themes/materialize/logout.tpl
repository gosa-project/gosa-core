<body>

  <div class="center-screen">
    <form action='index.php' method='post' name='mainform'>

      <div class="container">
        <div class="card">
          <div class="card-content">
            <span class="card-title">{t}Your GOsa session has expired!{/t}</span>
            <p>
              {t}It has been a while since your last interaction with GOsa took place. Your session has been closed for security reasons. Please login again to continue with administrative tasks.{/t}
            </p>
          </div>
          <div class="card-action">
            <button class="btn-small gonicus-color" name="dummy" id="dummy">
              {t}Login again{/t}
            </button>
          </div>
        </div>
      </div>

    </form>
  </div>


  <script language="JavaScript" type="text/javascript">
    $('dummy').focus();
  </script>

</body>

</html>