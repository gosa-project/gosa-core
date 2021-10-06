<p>
{t}Name{/t}
<input type='text' name='name' value='{$name}' >
</p>

<hr>

<h3>{t}Rule{/t}</h3>
<textarea name='rule' style='height:400px;width:100%;'>{$rule}</textarea>

<hr>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
 <button type='submit' name='SaveRule'>
 {msgPool type=saveButton}</button>&nbsp;
 <button type='submit' name='CancelRule'>
 {msgPool type=cancelButton}</button>
</div>
