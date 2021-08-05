<input type="hidden" name="mailedit" value="1">
<table style='width:100%; ' summary="{t}Mail settings{/t}">


 <!-- Headline container -->
 <tr>
  <td style='width:50%; '>

   <h3>{t}Mail distribution list{/t}</h3>
   <table summary="{t}Mail distribution list{/t}">
    <tr>
     <td><LABEL for="mail">{t}Primary address{/t}</LABEL>{$must}</td>
     <td>
{render acl=$mailACL}
	<input type='text' id="mail" name="mail" size=50 maxlength=65 value="{$mail}" title="{t}Primary mail address for this distribution list{/t}">
{/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('mail');
  -->
</script>
