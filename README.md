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
