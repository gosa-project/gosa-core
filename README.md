# GOsa 2.8 README

## Information

GOsa is an LDAP administration frontend for user administration. It
is NO GENERIC frontend to dictionary servers. Information is stored
in the way the underlying concepts suppose them to be stored (i.e.
people accounts are stored in "ou=people" subtrees, etc.) This can
only be configured marginally.

Complete setups applying Kerberos, AFS, LDAP, Mail, Proxy and Fax setups
are not trivial at all. You should be familiar with these components and
with your UNIX installation, of course. This file is (and will not be)
an introduction to any of these components. See INSTALL for a quick
overview about what to do, to get the things up and running.

If you want to try out GOsa on an easy-to-install server, we recommend
playing with a
[Debian Edu mainserver](https://wiki.debian.org/DebianEdu/Documentation/Stretch/Installation)
installation. The Debian Edu mainserver has GOsa pre-installed and set up
out-of-the box.


## Translations

GOsa is not available in your native language? Just read on...

Translations (or i18n) in GOsa are done by the gettext library. As a
result, every set of translations is stored inside one directory
per language as a text file called ``messages.po``.

For GOsa, you've to differenciate between gosa-core and the various
gosa-plugins. The core has its translation and every plugin has a
seperate translation of its own, too.

GOsa core can be translated by taking a look at the locales/core
directory. Just take the ``messages.po`` file and copy it to some other
location and put your translations into the msgstr fields of this file.
For more comfort, use programs like i.e. poedit to achieve this. You may
look at the ``de/LC_MESSAGES`` for the way how it works.

If you're done with that, create a directory for your language using the
ISO shortcuts (i.e. ``es`` for spain) with a subdirectory
``LC_MESSAGES``. For, e.g., the Spanish translation this would be:

```
 gosa-core/locales/core/es/LC_MESSAGES
```

Put the freshly translated Spanish ``messages.po`` into this directory.

To test this, you've to deploy the ``messages.po`` file in your running
copy of GOsa and run the ``update-gosa`` command, to let GOsa merge the
translations. Then, make sure your Apache web server has locale support
or, in case of Debian, that the specific locale will be generated (via
dpkg-reconfigure locales).

For gosa-plugins, every plugin has a locales directory. Translation works
like described for gosa-core.

Always run ``update-gosa`` after you've added translations in order to
let GOsa compile and re-sync the translations.


## Contribute Translations

You may want your translations to be included in the main [GOsa
repository](https://github.com/gosa-project), then just file and issue
for the correct GOsa component and attach the ``.po`` there.


## Notes

Be sure that ``gosaUserTemplates`` are not able to log into your server,
since they may have no password set. Example configs can be found in the
contrib directory.

Have fun!

---
Cajus Pollmeier <pollmeier@gonicus.de>


## Template System

GOsa comes with a new theme and a simplified template system for plugins and HTML snippets.

The simplified template system makes it easier to transfer your new themes to all plugins. 
For this you need to organize your new plugin templates in a subfolder that has the name of the theme. 

Also code snippets that produce visual output are now easy to customize using switch statements. 
There you need to add an expression with the name of the theme if needed.

Example:

Declare a variable for the theme name in your class or in scope of the function.
Initialize it with the function `getThemeName()` from the include/functions.inc.

Now you can use it as in the example below.

```
switch (${ThemeVariableName}) {
    case '{YourThemeName}':
        code...
        break;

    default:
        code...
        break;
}
```

By default GOsa has the classic design from GOsa 2.7 and a new default design.
The theme is still changed via gosa.conf.

Sebastian Sternfeld <sternfeld@gonicus.de>

## Debug GOsa² core and plugins using Xdebug

**DISCLAIMER**: DO NOT, and I repeat **DO NOT** use this on production webservers!
Enabling a debugger can easily lead to *compromised* data, data *loss*, Remote code execution and more!
So please, use a isolated development server, without any important user data.

1. Install the `php-xdebug` package on your GOsa² server by executing the following command:

Debian (9+): `sudo apt install php-xdebug`

This will automagically configure apache2 too.

More information: https://xdebug.org/docs/install

2. Configure Xdebug:

`sudo editor /etc/php/7.4/mods-available/xdebug.ini` (Debian)

```ini

zend_extension=xdebug.so

xdebug.profiler_enable_trigger = 1
xdebug.profiler_enable = 0
xdebug.profiler_out_dir = "/tmp"

xdebug.remote_enable = 1

xdebug.start_with_request = yes

xdebug.idekey = <REPLACE ME WITH IDE KEY ('VSCODE', 'PHPSTORM'…)>

xdebug.mode = debug

xdebug.client_host = "<REPLACE ME WITH DEV HOST IP>"
xdebug.client_port = 9003

xdebug.log = "/tmp/xdebug.log"
```

**WARNING:** Many Linux distributions now use systemd, which implements private tmp directories.
This means that when PHP is run through a web server or as PHP-FPM, the /tmp directory is
prefixed with something akin to:

`/tmp/systemd-private-ea3cfa882b4e478993e1994033fc5feb-apache.service-FfWZRg`


You can also set up a port forwarding tunnel using SSH, if you're using two different
machines (or a VM) to develop:

`ssh user@remote-machine -R 9003:127.0.0.1:9003`


Also please **notice** that, the Xdebug server connects to the IDE client! (Not the other way around!)
So the IDE client needs to **listen** at port `9003` in this example.


The IDE key depends on the IDE you're using. Visual Studio Code uses `VSCODE` and PhpStorm uses `PHPSTORM`.
But don't worry, most IDE's let you configure this though.


3. Test your installation.

First restart apache2:

`sudo systemctl restart apache2`

Then test if Xdebug was installed successfully:

Execute `php -v`, it should look similar to this:

```
PHP 7.4.25 (cli) (built: Oct 23 2021 21:53:50) ( NTS )
Copyright (c) The PHP Group
Zend Engine v3.4.0, Copyright (c) Zend Technologies
    with Zend OPcache v7.4.25, Copyright (c), by Zend Technologies
    with Xdebug v3.0.2, Copyright (c) 2002-2021, by Derick Rethans
```

4. Install xdebug-helper extension on your development browser.

Your internet browser needs to sent a specific header to the server to indicate a debugging session.
This can be done easily using an extension.
Luckily there are already easy to use extensions for Firefox and Chrome.

https://github.com/BrianGilbert/xdebug-helper-for-firefox)

https://github.com/BrianGilbert/xdebug-helper-for-chrome)


5. Configure your IDE (Visual Studio Code in this example)

* Install `PHP Debug` extension: https://marketplace.visualstudio.com/items?itemName=xdebug.php-debug

* Configure VS Code to properly launch a PHP debug session.

Example `.vscode/launch.json`:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Listen for Xdebug",
            "type": "php",
            "request": "launch",
            "port": 9003,
            // server -> local
            "pathMappings": {
                "/usr/share/gosa/plugins/": "${workspaceRoot}",
            },
            "postDebugTask": "sleep-task",
            "log": true,
        },
    ]
}
```

The `pathMappings` setting is VERY important and helpful!
Imagine you work on `~/dev/test.php` and your Apache2 webserver
is configured to use `/var/www/html/(test.php)`. Your IDE will report any breakpoints
using path `~/dev/test.php`. But the Xdebug server won't find any files under
`~/dev/test.php` since it's a different machine!

`pathMappings` maps `~/dev` to `/var/www/html` and vice-versa.

Also you have to keep your local files and the server files in sync!
This can be archived by using a SSHFS mount to develop directly on the
server's files.

`sshfs <local-mount-dir> root@<remote-server-ip>:/var/www/html`

Also the xdebug client inside of VS Code can sometimes be slow to restart,
so wait a bit after closing this debug session:

`.vscode/tasks.json`:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "sleep-task",
            "command": "sleep 0.5s",
            "type": "shell",
            "presentation": {
                "reveal": "never"
            },
        }
    ]
}
```