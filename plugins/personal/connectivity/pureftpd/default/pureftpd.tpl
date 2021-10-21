<div class="ftpd-wrapper">
  <div class="row">
    <div class="col s12">
      {render acl=$pureftpdACL checkbox=$multiple_support checked=$use_pureftpd}
        <label>
          <p>
            <input type="checkbox" name="pureftpd" value="B" {$pureftpdState}
              onclick="{$changeState}" class="center">
            <span>{t}FTP account{/t}</span>
          </p>
        </label>
      {/render}
    </div>
  </div>

  <div class="row ftp-account">
    <div class="col s12 xl6">
      <h3>{t}Bandwidth{/t}</h3>

      {render acl=$FTPUploadBandwidthACL checkbox=$multiple_support checked=$use_FTPUploadBandwidth}
        <div class="input-field">
          <input type='text' name="FTPUploadBandwidth" id="FTPUploadBandwidth" maxlength=7 value="{$FTPUploadBandwidth}" {$fstate} >
          <label for="FTPUploadBandwidth">{t}Upload bandwidth{/t} ({t}KB/s{/t})</label>
        </div>
      {/render}

      {render acl=$FTPDownloadBandwidthACL checkbox=$multiple_support checked=$use_FTPDownloadBandwidth}
        <div class="input-field">
          <input type='text' name="FTPDownloadBandwidth" id="FTPDownloadBandwidth" maxlength=7 value="{$FTPDownloadBandwidth}" {$fstate} >
          <label for="FTPDownloadBandwidth">{t}Download bandwidth{/t} ({t}KB/s{/t})</label>
        </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <h3>{t}Quota{/t}</h3>

      {render acl=$FTPQuotaFilesACL checkbox=$multiple_support checked=$use_FTPQuotaFiles}
        <div class="input-field">
          <input type='text' name="FTPQuotaFiles" id="FTPQuotaFiles" maxlength=10 value="{$FTPQuotaFiles}" {$fstate} >
          <label for="FTPQuotaFiles">{t}Files{/t}</label>
        </div>
      {/render}

      {render acl=$FTPQuotaMBytesACL checkbox=$multiple_support checked=$use_FTPQuotaMBytes}
        <div class="input-field">
          <input type='text' name="FTPQuotaMBytes" id="FTPQuotaMBytes" maxlength=10 value="{$FTPQuotaMBytes}" {$fstate} >
          <label for="FTPQuotaMBytes">{t}Size{/t} ({t}MB{/t})</label>
        </div>
      {/render}
    </div>

    <hr class="divider">

    <div class="col s12 xl6">
      <h3>{t}Ratio{/t}</h3>

      <div class="input-field add">
        {render acl=$FTPUploadRatioACL checkbox=$multiple_support checked=$use_FTPUploadRatio}
          <input type='text' name="FTPUploadRatio" id="FTPUploadRatio" size=5
            maxlength=20 value="{$FTPUploadRatio}" {$fstate} >
          <label for="FTPUploadRatio">{t}Uploaded / downloaded files{/t}</label>
        {/render}

        <span class="intermediate-char">/</span>

        {render acl=$FTPDownloadRatioACL checkbox=$multiple_support checked=$use_FTPDownloadRatio}
          <input type='text' name="FTPDownloadRatio" id="FTPDownloadRatio" size=5
            maxlength=20 value="{$FTPDownloadRatio}" {$fstate} >
        {/render}
      </div>
    </div>

    <div class="col s12 xl6">
      <h3>{t}Miscellaneous{/t}</h3>

      {render acl=$FTPStatusACL checkbox=$multiple_support checked=$use_FTPStatus}
        <label>
          <input type=checkbox name="FTPStatus" id="FTPStatus" value="disabled" {$FTPStatus}
            title="{t}Check to disable FTP Access{/t}" {$fstate} class="center">
          <span>{t}Temporary disable FTP access{/t}</span>
        </label>
      {/render}
    </div>
  </div>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
	focus_field('FTPUploadBandwidth');
</script>
