<input type='image' src='images/empty.png' name='no_action_posted' value='do_nothing' alt='' style='width:2px;height:2px;'>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=2>
  <tr>
    <td style="vertical-align:top; width:50%;">
      <div class="contentboxh">
        <p class="contentboxh">&nbsp;{$HEADLINE}</p>
      </div>
      
      <div class="contentboxb">
       <div style='background:white;padding:0px;padding:3px;'>
        {$ROOT}&nbsp; {$BACK}&nbsp; {$HOME}&nbsp; {$RELOAD}&nbsp; {$SEPARATOR}&nbsp; {t}Base{/t} {$BASE}&nbsp;<input class='center' type='image' src='images/lists/submit.png' align='middle' title='{t}Update{/t}' name='submit_department' alt='{t}Submit{/t}'>&nbsp; {$SEPARATOR}&nbsp; <img src='images/rocket.png' alt='' class='center'> {$ACTIONS}
       </div>
      </div>
      
      <div style='height:4px;'>
      </div>
      
      <input type="hidden" id="d_save" value="490">
      {$LIST}
    </td>
    <td style='vertical-align:top'>
      {$FILTER}
    </td>
  </tr>
</table>

<input type="hidden" name="ignore">
