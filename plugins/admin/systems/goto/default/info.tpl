
<div class="system-wrapper">
     <div class="system-info">
          {if !$ACL}
               <h2>{msgPool type=permView}</h2>
          {else}
               <div class="row">
                    <div class="col s12 xl6">
                         <h2>{t}System information{/t}</h2>

                         <table class="sys-info-table striped">
                              <tbody>
                                   <tr>
                                        <td>
                                             {t}CPU{/t}
                                        </td>
                                        <td>
                                             {$ghCpuType}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Memory{/t}
                                        </td>
                                        <td>
                                             {$ghMemSize}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Boot MAC{/t}
                                        </td>
                                        <td>
                                             {$macAddress}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}USB support{/t}
                                        </td>
                                        <td>
                                             {$ghUsbSupport}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}System status{/t}
                                        </td>
                                        <td>
                                             {$status}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Inventory number{/t}
                                        </td>
                                        <td>
                                             {$ghInventoryNumber}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Last login{/t}
                                        </td>
                                        <td>
                                             {$gotoLastUser}
                                        </td>
                                   </tr>
                              </tbody>
                         </table>
                    </div>

                    <div class="col s12 xl6">
                         <h2>{t}Hardware information{/t}</h2>

                         <table class="sys-info-table striped">
                              <tbody>
                                   <tr>
                                        <td>
                                             {t}Network devices{/t}
                                        </td>
                                        <td>
                                             {foreach item=netcard from=$ghNetNic}
                                                  {$netcard}
                                             {/foreach}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}IDE devices{/t}
                                        </td>
                                        <td>
                                             {foreach item=idedev from=$ghIdeDev}
                                                  {$idedev}
                                             {/foreach}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}SCSI devices{/t}
                                        </td>
                                        <td>
                                             {foreach item=scsidev from=$ghScsiDev}
                                                  {$scsidev}
                                             {/foreach}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Floppy device{/t}
                                        </td>
                                        <td>
                                             {$FloppyDevice}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}CD-ROM device{/t}
                                        </td>
                                        <td>
                                             {$CdromDevice}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Graphic device{/t}
                                        </td>
                                        <td>
                                             {$ghGfxAdapter}
                                        </td>
                                   </tr>
                                   <tr>
                                        <td>
                                             {t}Audio device{/t}
                                        </td>
                                        <td>
                                             {$ghSoundAdapter}
                                        </td>
                                   </tr>
                              </tbody>
                         </table>
                    </div>
               </div>

               {if $active eq "true"}
                    <div class="head-content">
                         {image path="<i class='material-icons input-icons'>lightbulb_outline</i>"}
                         <h2>{t}System status{/t}</h2>
                    </div>

                    <div class="row">
                         <div class="col s12 xl6">
                              <h3>{t}System information{/t}</h3>

                              <table class="sys-info-table striped">
                                   <tbody>
                                        <tr>
                                        <td>
                                             {t}Up since{/t}
                                        </td>
                                        <td>
                                             {$uptime}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}CPU load{/t}
                                        </td>
                                        <td>
                                             {$load}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}Memory usage{/t}
                                        </td>
                                        <td>
                                             {$mem}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}Swap usage{/t}
                                        </td>
                                        <td>
                                             {$swap}
                                        </td>
                                        </tr>
                                   </tbody>
                              </table>
                         </div>

                         <div class="col s12 xl6">
                              <h3>Service information</h3>

                              <table class="sys-info-table striped">
                                   <tbody>
                                        <tr>
                                        <td>
                                             {t}SSH service{/t}
                                        </td>
                                        <td>
                                             {$sshd}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}Print service{/t}
                                        </td>
                                        <td>
                                             {$cupsd}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}Scan service{/t}
                                        </td>
                                        <td>
                                             {$saned}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}Sound service{/t}
                                        </td>
                                        <td>
                                             {$artsd}
                                        </td>
                                        </tr>
                                        <tr>
                                        <td>
                                             {t}GUI{/t}
                                        </td>
                                        <td>
                                             {$X}
                                        </td>
                                        </tr>
                                   </tbody>
                              </table>
                         </div>
                    </div>
               {/if}
          {/if}
     </div>
</div>
