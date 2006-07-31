<h2>Edit spam rule</h2>

<table style='width:100%;'>
 <tr>
  <td width='70'>
   {t}Name{/t}
  </td>
  <td>
   <input type='text' name='name' value='{$name}' >
  </td>
 </tr>
 <tr>
  <td colspan=2>
   {t}Rule{/t}
   <textarea name='rule' style='width:100%;'>{$rule}</textarea>
  </td>
 </tr>
</table>
<p class='seperator'>&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveRule' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelRule' value='{t}Cancel{/t}'>
</div>
</p>

