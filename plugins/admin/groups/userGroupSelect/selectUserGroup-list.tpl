<input type='image' src='images/empty.png' name='no_action_posted' value='do_nothing' alt='' style='width:2px;height:2px;'>

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=2>
  <tr>
    <td style="vertical-align:top; width:50%;">
      <div class="contentboxh">
        <p class="contentboxh">&nbsp;{$HEADLINE}&nbsp;{$SIZELIMIT}</p>
      </div>
      
      <div class="contentboxb">
       <div style='background:white;padding:0px;padding:3px;'>
        <table><tr>
          <td>{$ROOT}&nbsp;</td><td>{$BACK}&nbsp;</td><td>{$HOME}&nbsp;</td><td>{$RELOAD}&nbsp;</td><td>{$SEPARATOR}&nbsp;</td><td>{t}Base{/t} {$BASE}&nbsp;<input class='center' type='image' src='images/lists/submit.png' align='middle' title='{t}Update{/t}' name='submit_department' alt='{t}Submit{/t}'>&nbsp;</td><td>{$SEPARATOR}&nbsp;</td><td><img src='images/rocket.png' alt='' class='center'></td><td> {$ACTIONS}</td>
        </tr></table>
       </div>
      </div>
      
      <div style='height:4px;'>
      </div>
      
      <input type="hidden" id="d_save" value="450">
      <input type="hidden" id="d_space" value="760">
      {$LIST}
    </td>
    <td style='vertical-align:top'>
      {$FILTER}
    </td>
  </tr>
</table>


<p class='separator'>&nbsp;</p>
  <p style="text-align:right">
  <input type=submit name="SavetargetSelectDialog" value="{msgPool type=okButton}">
  &nbsp;
  <input type=submit name="ClosetargetSelectDialog" value="{msgPool type=cancelButton}">
</p>

<input type="hidden" name="ignore">
