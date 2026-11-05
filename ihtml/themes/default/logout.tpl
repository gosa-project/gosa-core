<body>

<form action='index.php' method='post' name='mainform'>

<div class='logout-box'>
 <h2>{t}Your GOsa session has expired!{/t}</h2>

 <p>
 {t}It has been a while since your last interaction with GOsa took place. Your session has been closed for security reasons. Please login again to continue with administrative tasks.{/t}
 </p>

 <hr>

 <div class='plugin-actions'>
  <center>
   <button type="submit" name="dummy" id="dummy">{t}Login again{/t}</button>
  </center>
 </div>
</div>

</form>

<script language="JavaScript" type="text/javascript">
  $('dummy').focus();
</script>

</body>
</html>
