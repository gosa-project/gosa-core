<div class="webdav-wrapper">
    <div class="row">
        <div class="col s12">
			{render acl=$webdavAccountACL checkbox=$multiple_support checked=$use_webdav}
				<label>
					<input type="checkbox" name="webdav" value="B" {$webdavState} class="center">
					<span style="overflow: hidden;">{t}WebDAV account{/t}</span>
				</label>
			{/render}
		</div>
	</div>
</div>
