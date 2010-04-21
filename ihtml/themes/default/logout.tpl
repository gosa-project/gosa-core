<body>

<form action='index.php' method='post' name='mainform'>

<div class='logout-box'>
 <h2>{t}Your GOsa session has expired!{/t}</h2>

 <p>
 {t}The last interaction with the GOsa web interface has been some time ago in the past. For security reasons, the session has been closed. To continue with administrative tasks, please sign in again.{/t}
 </p>

 <hr>

 <div class='plugin-actions'>
  <center>
   <button type="submit" name="dummy">{t}Sign in again{/t}</button>
  </center>
 </div>
</div>

</form>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  focus_field('dummy');
  -->
</script>

</body>
</html>
