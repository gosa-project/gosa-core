<table class='ObjectListViewport' id='ObjectListViewport_{$OLV_List_Id}' cellspacing=0 cellpadding=0>
  <tr>
    <td>
      <table class='ObjectListViewport_Table' id='ObjectListViewport_Table_{$OLV_List_Id}' cellpadding=0 cellspacing=0 >
        <tr>
          <td class='ObjectListViewport_TD_Header' id='ObjectListViewport_TD_Header_{$OLV_List_Id}'>
            <table class='ObjectListViewport_Header_Table' id='ObjectListViewport_Header_Table_{$OLV_List_Id}'>
              <tr>
                {foreach from=$OLV_Header key=key item=item}
                  <td class='ObjectListViewport_Header_Cell' style='{$item.style}'>{$item.name}</td>
                {/foreach}
              </tr>
            </table>
          </td>
        </tr>
        <tr>
          <td class='ObjectListViewport_TD_Entries' id='ObjectListViewport_TD_Entries_{$OLV_List_Id}'>
            <div class='ObjectListViewport_Entry_Cover' id='ObjectListViewport_Entry_Cover_{$OLV_List_Id}'>
              <table class='ObjectListViewport_Entry_Table' id='ObjectListViewport_Entry_Table_{$OLV_List_Id}'>
                {foreach from=$OLV_Entries key=key item=item}
                  <tr class="{$item.row.class}">
                    {foreach from=$item.cols key=key2 item=item2}
                      <td class='ObjectListViewport_Entry_Cell' style='{$item2.style1}'>
                        <div style='{$item2.style2}'>
                          {$item2.value}
                        </div>
                      </td>
                    {/foreach}
                  </tr>
                {/foreach}
              </table>
            </div>
          </td>
        </tr>
        <tr>
          <td class='ObjectListViewport_TD_Footer' id='ObjectListViewport_TD_Footer_{$OLV_List_Id}'>
            <table class='ObjectListViewport_Footer_Table'>
              <tr>
                <td class='ObjectListViewport_Footer_Cell' colspan='{$OLV_Num_Cols}'>{$OLV_Footer_Message}</td>
              </tr>
            </table>
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<!--
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
-->

