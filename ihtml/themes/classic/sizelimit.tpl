<h3>{$warning}</h3>
<p>
{t}The size limit option makes LDAP operations faster and saves the LDAP server from getting too much load. The easiest way to handle big databases without long timeouts would be to limit your search to smaller values and use filters to get the entries you are looking for.{/t}
</p>

<hr>

<b>{t}Please choose the way to react for this session{/t}:</b>
<p>
<input type="radio" name="action" value="ignore">{t}ignore this error and show all entries the LDAP server returns{/t}<br>
<input type="radio" name="action" value="limited" checked>{t}ignore this error and show all entries that fit into the defined size limit{/t}<br>
<input type="radio" name="action" value="newlimit">{$limit_message}
</p>
<hr>
<div class="plugin-actions">
 <button type='submit' name='set_size_action'>{t}Set{/t}</button>
</div>

<input type="hidden" name="ignore">
