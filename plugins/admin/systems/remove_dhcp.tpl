<div style="font-size:18px;">
<img alt="" src="images/button_cancel.png" align=top>&nbsp;{t}Warning{/t}
</div>
<p>
 {$warning}
 {t}This includes 'all' DHCP subsections that are located within this section. Please double check if your really want to do this since there is no way for GOsa to get your data back.{/t}
</p>

<p>
 {t}Best thing to do before performing this action would be to save the current contents of your LDAP tree in a file. So - if you've done so - press 'Delete' to continue or 'Cancel' to abort.{/t}
</p>

<p class="plugbottom">
  <input type=submit name="delete_dhcp_confirm" value="{t}Delete{/t}">
  &nbsp;
  <input type=submit name="cancel_section" value="{t}Cancel{/t}">
</p>
