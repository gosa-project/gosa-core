
{if !$ACL}
	<h3>{msgPool type=permView}</h3>
{else}

<h3>{t}System information{/t}</h3>
<table summary="{t}System information{/t}" style="width:100%;">
 <tr>
  <td style='width:50%'>

   <table summary="{t}Hardware information{/t}">
    <tr>
     <td>
<b>{t}CPU{/t}</b></td><td>{$ghCpuType}</td>
    </tr>
    <tr>
     <td><b>{t}Memory{/t}</b></td><td>{$ghMemSize}</td>
    </tr>
    <tr>
     <td><b>{t}Boot MAC{/t}</b></td><td>{$macAddress}</td>
    </tr>
    <tr>
     <td>
<b>{t}USB support{/t}</b></td><td>{$ghUsbSupport}<div style="height:20px;"></div></td>
    </tr>
    <tr>
     <td><b>{t}System status{/t}</b></td>
     <td>{$status}</td>
    </tr>
    <tr>
     <td><b>{t}Inventory number{/t}</b></td>
     <td>{$ghInventoryNumber}</td>
    </tr>
    <tr>
     <td><b>{t}Last login{/t}</b></td>
     <td>{$gotoLastUser}</td>
    </tr>
   </table>
  </td>
  <td class='left-border'>

   &nbsp;
  </td>
  <td>
   <table summary="{t}Device information{/t}">
    <tr>
     <td>
<b>{t}Network devices{/t}</b></td>
     <td>
      {foreach item=netcard from=$ghNetNic}
        {$netcard}<br>
      {/foreach}
     </td>
    </tr>
    <tr><td colspan=2><div style="height:10px;"></div></td></tr>
    <tr>
     <td>
<b>{t}IDE devices{/t}</b></td>
     <td>
       {foreach item=idedev from=$ghIdeDev}
         {$idedev}<br>
       {/foreach}
     </td>
    </tr>
    <tr><td colspan=2><div style="height:10px;"></div></td></tr>
    <tr>
     <td>
<b>{t}SCSI devices{/t}</b></td>
     <td>
       {foreach item=scsidev from=$ghScsiDev}
         {$scsidev}<br>
       {/foreach}
     </td>
    </tr>
    <tr>
     <td><b>{t}Floppy device{/t}</b></td>
     <td>{$FloppyDevice}</td>
    </tr>
    <tr>
     <td><b>{t}CD-ROM device{/t}</b></td>
     <td>{$CdromDevice}</td>
    </tr>
    <tr><td colspan=2><div style="height:10px;"></div></td></tr>
    <tr>
     <td>
<b>{t}Graphic device{/t}</b></td>
     <td>{$ghGfxAdapter}</td>
    </tr>
    <tr>
     <td>
<b>{t}Audio device{/t}</b></td>
     <td>{$ghSoundAdapter}</td>
    </tr>
   </table>
  </td>
 </tr>
</table>

{if $active eq "true"}
<br>
<h3 style="border-top:1px solid #A0A0A0; padding-top:5px;">{image path="images/lists/on.png"} {t}System status{/t}
</h3>
<table summary="{t}System information{/t}" style="width:100%">
 <tr>
  <td style='width:50%'>

   <table summary="{t}Status{/t}">
    <tr>
     <td><b>{t}Up since{/t}</b></td><td>{$uptime}</td>
    </tr>
    <tr>
     <td><b>{t}CPU load{/t}</b></td><td>{$load}</td>
    </tr>
    <tr>
     <td><b>{t}Memory usage{/t}</b></td><td>{$mem}</td>
    </tr>
    <tr>
     <td><b>{t}Swap usage{/t}</b></td><td>{$swap}</td>
    </tr>
    <tr>
     <td colspan=2>&nbsp;</td>
    </tr>
    {$partitions}
   </table>
  </td>
  <td class='left-border'>

   &nbsp;
  </td>
  <td>

   <table summary="{t}Service information{/t}">
    <tr>
     <td><b>{t}SSH service{/t}</b></td><td>{$sshd}</td>
    </tr>
    <tr>
     <td><b>{t}Print service{/t}</b></td><td>{$cupsd}</td>
    </tr>
    <tr>
     <td><b>{t}Scan service{/t}</b></td><td>{$saned}</td>
    </tr>
    <tr>
     <td><b>{t}Sound service{/t}</b></td><td>{$artsd}</td>
    </tr>
    <tr>
     <td><b>{t}GUI{/t}</b></td><td>{$X}</td>
    </tr>
   </table>
  </td>
 </tr>
</table>
{/if}
{/if}
