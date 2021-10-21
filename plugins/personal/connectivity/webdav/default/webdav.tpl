<div class="webdav-wrapper">
    <div class="row">
        <div class="col s12">
			{render acl=$webdavAccountACL checkbox=$multiple_support checked=$use_webdav}
				<label>
					<p>
						<input type="checkbox" name="webdav" value="B" {$webdavState}>
						<span>{t}WebDAV account{/t}</span>
					</p>
				</label>
			{/render}
		</div>
	</div>
</div>
