<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id: class_filter.inc 19251 2010-07-29 13:21:27Z hickert $$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class filter
{
    public $searches = [];
    public $xmlSearches = [];
    public $attributes = [];
    public $search;
    public $defaultSearch;
    public $category = '';
    public $objectStorage = [];
    public $base = '';
    public $scope = '';
    public $query;
    public $value = '';
    public $initial = false;
    public $scopeMode = 'auto';
    public $converter = null;
    public $pid;
    public $customConstraints = [];
    // public $autocompleters = [];

    public $theme;

    public function __construct(string $filename)
    {
        $this->theme = getThemeName();

        // Load eventually passed filename
        if (!$this->load($filename)) {
            exit("Cannot parse $filename!");
        }
        $this->pid = preg_replace('/[^0-9]/', '', microtime(true));
    }

    public function load($filename)
    {
        $contents = file_get_contents($filename);
        $xmlData = xml::xml2array($contents, 1);

        if (!isset($xmlData['filterdef'])) {
            return false;
        }

        $xmlData = $xmlData['filterdef'];

        // Load filter
        if (isset($xmlData['search'])) {
            // Array conversion
            if (!isset($xmlData['search'][0])) {
                $searches = [$xmlData['search']];
            } else {
                $searches = $xmlData['search'];
            }

            /* Store available searches */
            foreach ($searches as $search) {
                /* Do multi conversation */
                if (!isset($search['query'][0])) {
                    $search['query'] = [$search['query']];
                }

                /* Store search */
                $this->xmlSearches[$search['tag']] = $search;
            }
        } else {
            return false;
        }

        // Transfer scope
        $this->scopeMode = $xmlData['definition']['scope'];
        if ($this->scopeMode == 'auto') {
            $this->scope = 'one';
        } else {
            $this->scope = $this->scopeMode;
        }

        // Transfer attributes
        $this->attributes = $xmlData['definition']['attribute'];
        if (!is_array($this->attributes)) {
            $this->attributes = [$this->attributes];
        }

        // Transfer initial value
        if (isset($xmlData['definition']['initial']) && $xmlData['definition']['initial'] == 'true') {
            $this->initial = true;
        }

        // Transfer category
        if (isset($xmlData['definition']['category'])) {
            $this->category = $xmlData['definition']['category'];
        }
        if (!is_array($this->category)) {
            $this->category = [$this->category];
        }

        // Initialize searches and default search mode
        $this->defaultSearch = $xmlData['definition']['default'];
        $this->reloadFilters();
        $this->setSearch($this->defaultSearch);

        return true;
    }

    public function reloadFilters()
    {
        $this->searches = array_merge($this->xmlSearches, userFilter::getFilter($this->category));
        $this->setSearch($this->search);
    }

    public function setSearch($method = null)
    {
        // Maybe our search method has gone?
        if (!isset($this->searches[$method])) {
            $method = $this->defaultSearch;
        }

        // Try to use it, but bail out if there's no help...
        if (isset($this->searches[$method])) {
            $this->query = $this->searches[$method]['query'];
            $this->search = $method;
        } else {
            exit('Invalid search module!');
        }
    }

    public function getTextfield($tag, $value = '', $element = null)
    {
        global $smarty;
        $smarty->assign('tag', $tag);
        $smarty->assign('value', $value);
        $smarty->assign('hasAutocomplete', $element && isset($element['autocomplete']));
        $smarty->assign('frequency', $element['autocomplete']['frequency'] ?? '0.5');
        $smarty->assign('characters', $element['autocomplete']['characters'] ?? '1');

        return $smarty->fetch(get_template_path('filter_textfield.tpl'));
    }

    public function getCurrentBase()
    {
        if (isset($this->search->base) && (string) $this->search->scope != 'auto') {
            return false;
        }

        return $this->base;
    }

    public function getCurrentScope()
    {
        if (isset($this->search->scope) && (string) $this->search->scope != 'auto') {
            return (string) $this->search->scope;
        }

        return $this->scope;
    }

    public function setConverter($hook)
    {
        $this->converter = $hook;
    }

    public function setObjectStorage($storage)
    {
        $this->objectStorage = $storage;
    }

    public function setBase($base)
    {
        $this->base = $base;
    }

    public function setCurrentScope($scope)
    {
        $this->scope = $scope;
    }

    public function render()
    {
        switch ($this->theme) {
            case 'classic':
                $content = "<table class='filter-wrapper' summary='" . _('Search filter') . "'><tr><td>" . $this->renderFilterMenu() . '</td><td>';
                $content .= "<div class='search-filter'>" . $this->getTextfield('search_filter', set_post($this->value), $this->searches[$this->search]) . '</div>' .
                    "&nbsp;<button class='search-filter' type='submit' title='" . _('Search') . "'>" . image('images/find.png') . '</button></td></tr></table>';
                break;

            default:
                $s = $this->searches[$this->search];

                $content = $this->renderFilterMenu();

                $content .= "   <div class='search-field'>
                                    " . $this->getTextfield('search_filter', set_post($this->value), $s) . "
                                    <button class='btn' type='submit' title='" . _('Search') . "'>
                                        <i class='material-icons md-18'>search</i>
                                    </button>
                                </div>";
                break;
        }
        // Return meta data
        return "<input type='hidden' name='FILTER_PID' value='" . $this->pid . "'>" . $content;
    }

    public function query()
    {
        global $class_mapping, $composerClassMap;
        $result = [];

        // Return empty list if initial is not set
        if (!$this->initial) {
            $this->initial = true;

            return $result;
        }
        // Go thru all queries and merge results
        foreach ($this->query as $query) {
            if (!isset($query['backend']) || !isset($query['filter'])) {
                exit('No backend specified in search config.');
            }

            // Is backend available?
            $backend = 'filter' . $query['backend'];
            if (!isset($class_mapping["$backend"])) {
                if (!isset($composerClassMap[$query['backend']])) {
                    exit('Invalid backend specified in search config.');
                } else {
                    $backend = $query['backend'];
                }
            }

            // Load filter and attributes
            $filter = $query['filter'];

            // Handle converters if present
            if ($this->converter) {
                preg_match('/([^:]+)::(.*)$/', $this->converter, $m);
                if ($this->value == '') {
                    $filter = call_user_func([$m[1], $m[2]], preg_replace('/\$/', '*', $filter));
                } else {
                    $filter = call_user_func([$m[1], $m[2]], preg_replace('/\$/', $this->value, $filter));
                }
            }

            // Do not replace escaped \$ - This is required to be able to search for e.g. windows machines.
            if ($this->value == '') {
                $filter = preg_replace('/\$/', '*', $filter);
            } else {
                $filter = preg_replace('/\$/', '*' . LDAP::escapeValue($this->value) . '*', $filter);
            }

            $result = array_unique(array_merge($result, call_user_func([$backend, 'query'], $this->base, $this->scope, $filter, $this->attributes, $this->category, $this->objectStorage)), SORT_REGULAR);
        }

        /* If we had additional Constraints filter the $result array */
        $finalResult = [];
        if ($this->customConstraints) {
            $finalResult = $this->constraintFilter($this->customConstraints, $result);
        } else {
            $finalResult = $result;
        }

        return $finalResult;
    }

    /* iterate the result array with the given constraints */
    public function constraintFilter(array $constraints, array $res)
    {
        $tmp = [];
        /* Check for every occurenc of the contraints and sort them out */
        for ($i = 0; $i < count($res); ++$i) {
            foreach ($constraints as $filter => $value) {
                if (!in_array($res[$i][$filter], $value)) {
                    $tmp[] = $res[$i];
                }
            }
        }

        return $tmp;
    }

    /* set additional Client side filter */
    public function setConstraint($filter)
    {
        $this->customConstraints = $filter;
    }

    public function update()
    {
        if (isset($_POST['FILTER_PID']) && $_POST['FILTER_PID'] == $this->pid) {
            // Save input field
            if (isset($_POST['search_filter'])) {
                $this->value = get_post('search_filter');
            }

            // Save scope if needed
            if ($this->scopeMode == 'auto' && isset($_POST['act']) && $_POST['act'] == 'toggle-subtree') {
                $this->scope = ($this->scope == 'one') ? 'sub' : 'one';
            }

            // Switch filter?
            if (isset($_POST['act'])) {
                foreach ($this->searches as $tag => $cfg) {
                    if ($_POST['act'] == "filter-$tag") {
                        $this->setSearch($tag);
                        break;
                    }
                }
            }
        }
    }

    public function getCompletitionList($config, $value = '*')
    {
        global $class_mapping, $composerClassMap;
        $res = [];

        // Load result attributes
        $attributes = $config['autocomplete']['attribute'];
        if (!is_array($attributes)) {
            $attributes = [$attributes];
        }

        // Do the query
        $result = [];

        // Is backend available?
        // FIXME
        $queries = $config['query'];
        if (!isset($queries[0])) {
            $queries = [$queries];
        }
        foreach ($queries as $query) {
            $backend = 'filter' . $query['backend'];
            if (!isset($class_mapping["$backend"])) {
                if (!isset($composerClassMap[$query['backend']])) {
                    exit('Invalid backend specified in search config.');
                } else {
                    $backend = $query['backend'];
                }
            }
            $filter = preg_replace('/\$/', '*' . LDAP::escapeValue($value) . '*', $query['filter']);

            $result = array_merge($result, call_user_func(
                [$backend, 'query'],
                $this->base,
                $this->scope,
                $filter,
                $attributes,
                $this->category,
                $this->objectStorage
            ));
        }

        foreach ($result as $entry) {
            foreach ($attributes as $attribute) {
                if (is_array($entry[$attribute])) {
                    for ($i = 0; $i < $entry[$attribute]['count']; ++$i) {
                        if (mb_stristr($entry[$attribute][$i], $value)) {
                            $res[] = $entry[$attribute][$i];
                        }
                    }
                } else {
                    $res[] = $entry[$attribute];
                }
            }
        }

        return $res;
    }

    public function processAutocomplete()
    {
        $result = [];

        // Introduce maximum number of entries
        $max = 25;

        if (isset($this->searches[$this->search]['autocomplete'])) {
            $result = $this->getCompletitionList($this->searches[$this->search], get_post('search_filter'));
            $result = array_unique($result);
            asort($result);

            echo '<ul>';
            foreach ($result as $entry) {
                echo '<li>' . mark(get_post('search_filter'), $entry) . '</li>';
                if ($max-- == 0) {
                    break;
                }
            }

            echo '</ul>';
        }
    }

    public function getObjectBase($dn)
    {
        global $config;
        $base = '';

        // Try every object storage
        $storage = $this->objectStorage;
        if (!is_array($storage)) {
            $storage = [$storage];
        }
        foreach ($storage as $location) {
            $pattern = '/^[^,]+,' . preg_quote($location, '/') . '/i';
            $base = preg_replace($pattern, '', $dn);
        }

        /* Set to base, if we're not on a correct subtree */
        if (!isset($config->idepartments[$base])) {
            $base = $config->current['BASE'];
        }

        return $base;
    }

    public function renderFilterMenu()
    {
        global $smarty;
        $smarty->assign('xmlSearches', $this->xmlSearches);
        $smarty->assign('search', $this->search);
        $smarty->assign('userFilters', userFilter::getFilter($this->category));
        $smarty->assign('scopeMode', $this->scopeMode);
        $smarty->assign('scope', $this->scope);
        
        return $smarty->fetch(get_template_path('filter_menu.tpl'));
    }

    public function getFixedFilters()
    {
        return array_keys($this->searches);
    }
}
