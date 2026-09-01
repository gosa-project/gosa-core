#!/bin/bash
cat > include/version.php <<EOF
<?php
define('GOSA_VERSION', '${1}');
EOF