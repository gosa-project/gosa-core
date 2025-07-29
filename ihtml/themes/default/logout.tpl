<body>

  <div class="logout-box">
    <form action='index.php' method='post' name='mainform'>

      <div class="container">
        <div class="card">
          <div class="card-content">
            <div class="content-wrapper">
              <h2 class="card-title">{t}Your GOsa session has expired!{/t}</h2>
              <p>
                {t}It has been a while since your last interaction with GOsa took place. Your session has been closed for security reasons. Please login again to continue with administrative tasks.{/t}
              </p>

              <div class="card-action">
                <button class="btn-small primary" name="dummy" id="dummy">
                  {t}Login again{/t}
                </button>
              </div>
            </div>
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
