<div class="content-wrapper card-content-scroll">
  <h2>{t}PHP module and extension checks{/t}</h2>

  <p>
    {t}This step checks if your PHP server has all required modules and configuration settings.{/t}
  </p>

  <div class="setup-table card-content-scroll">
    <div class="row">
      <div class="col s6">
        <table class="striped">
          <caption>{t}Inspection{/t}</caption>
          <thead>
            <tr>
              <th>
                {t}Basic checks{/t}
              </th>
              <th></th>
            </tr>
          </thead>

          <tbody>
            {foreach from=$basic item=val key=key}
              {if $basic[$key].SOLUTION != "" && !$basic[$key].RESULT}
                <tr>
              {else}
                <tr>
              {/if}

              <td>{$basic[$key].NAME}</td>

              {if $basic[$key].RESULT}
                  <td class="green-text">{t}OK{/t}</td>
              {else}
                {if $basic[$key].MUST}
                  <td class="red-text">{t}Error{/t}</td>
                {else}
                  <td class="orange-text">{t}Warning{/t}</td>
                {/if}

                </tr>

                {if $basic[$key].SOLUTION != ""}
                  <tr>
                    <td>{$basic[$key].SOLUTION}</td>
                  </tr>
                  <tr>
                    <td>
                      {if $basic[$key].MUST}
                        <b>{t}GOsa will NOT run without fixing this.{/t}</b>
                      {else}
                        <b>{t}GOsa will run without fixing this.{/t}</b>
                      {/if}
                    </td>
                  </tr>
                {/if}
              {/if}
            {/foreach}
          </tbody>
        </table>
      </div>
      <div class="col s6">
        <table class="striped">
          <caption>{t}Extended checks{/t}</caption>
          <thead>
            <tr>
              <th>
                {t}PHP setup configuration{/t} (<a href='?info' target='_blank'>{t}show information{/t}</a>)
              </th>
            </tr>
          </thead>

          <tbody>
            {foreach from=$config item=val key=key}
              {if $config[$key].SOLUTION != "" && !$config[$key].RESULT}
                <tr>
              {else}
                <tr>
              {/if}

              <td>{$config[$key].NAME}</td>

              {if $config[$key].RESULT}
                <td class="green-text">{t}OK{/t}</td>
                </tr>
              {else}
                {if $config[$key].MUST}
                  <td class="red-text">{t}Error{/t}</td>
                {else}
                  <td class="orange-text">{t}Warning{/t}</td>
                {/if}

                </tr>

                {if $config[$key].SOLUTION != ""}
                  <tr>
                    <td>{$config[$key].SOLUTION}</td>
                  </tr>

                  <tr>
                    <td>
                    {if $config[$key].MUST}
                      <b>{t}GOsa will NOT run without fixing this.{/t}</b>
                    {else}
                      <b>{t}GOsa will run without fixing this.{/t}</b>
                    {/if}

                    </td>
                  </tr>
                {/if}
              {/if}
            {/foreach}
            </tr>
          </tbody>
        </table>
      </div>
    </div>
  </div>
</div>
