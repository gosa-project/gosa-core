<div class="filter-wrapper">
  <h2>{t}List of defined filters{/t}</h2>

  <div class="user-filter card-content-scroll">
    {$list}
  </div>

  <button class="btn-small" type='submit' name='addFilter'>{msgPool type='addButton'}</button>

  <div class="plugin-actions card-action">
    <button class="btn-small primary" type='submit' name='saveFilter'>{msgPool type='saveButton'}</button>
    <button class="btn-small primary" type='submit' name='cancelFilter'>{msgPool type='cancelButton'}</button>
  </div>
</div>
