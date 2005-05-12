<?
/*
 * Smarty plugin
 * -------------------------------------------------------------
 * File:     block.t.php
 * Type:     block function
 * Name:     t (short for translate through gettext)
 * Purpose:  return a translated string and, if
 *      development = true, check for translations and store in
 *      appropriate file.  Use babelfish to generate a translation if
 *      use_babelfish = true
 * Usage:  {t}Some text which could be translated if you wish{/t}
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------
 * 
 * @link http://www.gnu.org/copyleft/lesser.html
 * @copyright 2003 Tom Anderson
 * @author Tom Anderson <toma@etree.org>
 * @param string $language override global t_language variable
 * @version: 1.3
 */
function smarty_block_t($params, $content, &$this)
{
        if (!$content) return $content;
        // Setup local development variables.
        // If $development = false then use_babelfish is ignored.
        $development = false;
        // If use_babelfish is false, placeholders will be put in .po files
        // but no translation will be attempted using babelfish.
        $use_babelfish = false;

        global $t_language, $t_language_from, $t_msgfmt_location, $t_gettext_message_dir;

        // Check for new language
        if (isset($params['language']) && $params['language']) {
                $t_language = $params['language'];
                putenv ("LC_ALL={$GLOBALS[t_language]}");
        }

        // If under development, check for necessary files
        if ($development) {
                // Translate string once for every language supported
                foreach ($GLOBALS['t_language_all'] as $t_language) {
                        // Check for language directory; create if necessary
                        if (!is_dir($t_gettext_message_dir . $t_language)) {
                                mkdir ($t_gettext_message_dir . $t_language);
                                mkdir ("$t_gettext_message_dir$t_language/LC_MESSAGES");
                        }
                        // Check for .po file
                        $po = "$t_gettext_message_dir$t_language/LC_MESSAGES/messages.po";
                        if (!file_exists($po)) _create_po_file($po);

                        // Load the .po file and check for the current translation string
                        $text = file($po);
                        $cst_content = addcslashes(trim($content), '\\"');
                        if (!_in_array_trim('msgid "' . $cst_content . '"', $text)) {
                                // String is not in translation file; add it
                                // Get translated string from babelfish if set to true
                                if ($use_babelfish) {
                                        $translated = '';
                                        $url ="http://babelfish.altavista.com/babelfish/tr?lp={$t_language_from}_{$t_language}&intl=1&tt=urltext&doit=done&urltext=" . urlencode(htmlspecialchars($content, ENT_NOQUOTES, 'UTF-8'));
                                        $fp = fopen($url, 'r');
                                        // Find the translated string
                                        //foreach ($babel as $line) {
                                        while ($line = fgets($fp)) {
                                                if (strstr($line, "lang=$t_language")) {
                                                        // multibytes do not work yet with
                                                        // html_entity_decode but that is ok,
                                                        // since altavista does not seem to send entities.
                                                        $translated = html_entity_decode(trim(addcslashes(strip_tags($line), '\\"')), ENT_NOQUOTES);

                                                        break;
                                                }
                                        }
                                        fclose($fp);
                                        unset($url);
                                }

                                // Add string to .po
                                // mark translation as babelfish, so we know 
                                // that we need to look after it.
                                if ($use_babelfish) {
                                        error_log("#: babelfish\n", 3, $po);
                                }
                                error_log("#: " . $this->_plugins['block']['t'][1] . "\nmsgid \"" . $cst_content . "\"\nmsgstr \"$translated\"\n\n", 3, $po);

                                // Recompile translation file
                                `$t_msgfmt_location -o $t_gettext_message_dir$t_language/LC_MESSAGES/messages.mo $po`;
                        }
                }
        }
        // Do the actual translation through gettext.
        bindtextdomain ('messages', $t_gettext_message_dir);
        return gettext(trim($content));
}

// Helper functions
function _in_array_trim($needle, $haystack) {
        $needle = trim($needle);
        foreach ($haystack as $ck) {
                if ($needle == trim($ck)) return true;
        }
        return false;
}

function _create_po_file($po) {
        touch ($po);
        // put header info for .po file
        $year = date('Y');
        $date = date('r');
        $body = <<<EOD
# Smarty gettext generated translation file
# Author toma@etree.org
# Version 1.0 - 4/29/2003
#, fuzzy
msgid ""
msgstr ""
"Project-Id-Version: Smarty Gettext 1.0\\n"
"POT-Creation-Date: $date\\n"
"PO-Revision-Date: $date\\n"
"Last-Translator: Smarty Generated\\n"
"Language-Team: http://babelfish.altavista.com/babelfish/\\n"
"MIME-Version: 1.0n\\n"
"Content-Type: text/plain; charset=utf-8\\n"
"Content-Transfer-Encoding: 8bit\\n"
\n
EOD;
        error_log($body, 3, $po);

}

/* vim: set expandtab: */
?>
