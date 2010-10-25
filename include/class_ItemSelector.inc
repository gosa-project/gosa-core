<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2010 GONICUS GmbH
 *
 * ID: $$Id: class_listing.inc 15296 2010-01-26 08:27:39Z cajus $$
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

class ItemSelector {

    public $base;
    public $pid;
    public $action;
    public $height= 500;
    public $submitButton= true;
    public $tree;
    public $pathMapping;
    public $lastState;
    public $releaseInfo;
    public $releaseBase;

    function __construct($bases, $base, $releaseBase)
    {
        // Initialize pid
        $this->pid= preg_replace("/[^0-9]/", "", microtime(TRUE));

        // Get list of used IDs
        if(!session::is_set('releaseSelector_USED_IDS')){
            session::set('releaseSelector_USED_IDS',array());
        }
        $usedIds = session::get('releaseSelector_USED_IDS');

        // Generate instance wide unique ID
        $pid = "";
        while($pid == "" || in_array($pid, $usedIds)){

            // Wait 1 msec to ensure that we definately get a new id
            if($pid != "") usleep(1);
            $tmp= gettimeofday();
            $pid =  'l'.md5(microtime().$tmp['sec']);
        }

        // Only keep the last 10 list IDsi
        $usedIds = array_slice($usedIds, count($usedIds) -10, 10);
        $usedIds[] = $pid;
        session::set('releaseSelector_USED_IDS',$usedIds);
        $this->pid = $pid;

        // Transfer data
        $this->releaseBase = $releaseBase; 
        $this->setBases($bases);
        $this->setBase($base);
    }


    function setSubmitButton($flag)
    {
        $this->submitButton= $flag;
    }


    function setHeight($value)
    {
        $this->height= $value;
    }


    function setBase($base)
    {
        $this->base= $base;
        if (isset($this->pathMapping[$base])) {
            $this->update(true);
        }
    }

    function setRootBase($base)
    {
        $this->releaseBase = $base;
    }


    function checkBase($base)
    {
        return isset($this->pathMapping[$base]);
    }


    function checkLastBaseUpdate()
    {
        return $this->lastState;
    }


    function setBases($bases)
    {
        global $config;
        $this->releaseInfo = array();
        $this->pathMapping= array();
        $selected= $this->base == $this->releaseBase?"Selected":"";
        $first= true;
        $last_indent= 2;

        foreach ($bases as $base => $dummy) {

            // Build path style display
            $this->pathMapping[$base]= $base == $this->releaseBase? '/' : $dummy;
            $this->releaseInfo[$base]['name'] = preg_replace("/^.*\//","",$base);
            $this->releaseInfo[$base]['description'] = $dummy;
        }

        // Save bases to session for autocompletion
        session::global_set('pathMapping', $this->pathMapping);
        session::global_set('department_info', $this->releaseInfo);
    }


    function update($force= false)
    {
        global $config;

        // Analyze for base changes if needed
        $this->action= null;
        $last_base= $this->base;
        if(isset($_REQUEST["BPID_{$this->pid}"]) && $_REQUEST["BPID_{$this->pid}"] == $this->pid) {
            if (isset($_POST['bs_rebase_'.$this->pid]) && !empty($_POST['bs_rebase_'.$this->pid])) {
                $new_base= base64_decode(get_post('bs_rebase_'.$this->pid));
                if (isset($this->pathMapping[$new_base])) {
                    $this->base= $new_base;
                    $this->action= 'rebase';
                } else {
                    $this->lastState= false;
                    return false;
                }
            }else{
                // Input field set?
                if (isset($_POST['bs_input_'.$this->pid])) {

                    // Take over input field base
                    if ($this->submitButton && isset($_POST['submit_base_'.$this->pid]) || !$this->submitButton) {

                        // Check if base is available
                        $this->lastState= false;
                        foreach ($this->pathMapping as $key => $path) {
                            if (mb_strtolower($path) == mb_strtolower(get_post('bs_input_'.$this->pid))) {
                                $this->base= $key;
                                $this->lastState= true;
                                break;
                            }
                        }
                    }
                }
            } 

        }

        /* Skip if there's no change */
        if (($this->tree && $this->base == $last_base) && !$force) {
            return true;
        }

        $link= "onclick=\"\$('bs_rebase_".$this->pid."').value='".base64_encode($this->releaseBase)."';  
            $('submit_tree_base_".$this->pid."').click();\"";

        $initialValue = "";
        if(isset($this->pathMapping[$this->base])){
            $initialValue = $this->pathMapping[$this->base];
        }

        $this->tree= "<input style='width:160px' type='text' size='35' 
            name='bs_input_{$this->pid}' id='bs_input_{$this->pid}'

            onkeydown=\"    \$('bs_{$this->pid}').hide(); \"
            onfocus=\"      \$('bs_{$this->pid}').hide(); \"
            onmouseover=\"  mouseIsStillOver = true; 
            function showIt() 
            {
                if(mouseIsStillOver){
                    \$('bs_".$this->pid."').show();  
                }
            };
        Element.clonePosition(\$('bs_".$this->pid."'),
                'bs_input_".$this->pid."', 
                {setHeight: false, setWidth: false, offsetTop:(Element.getHeight('bs_input_".$this->pid."'))});
        rtimer=showIt.delay(0.25); \" 

            onmouseout=\"   mouseIsStillOver=false; 
            rtimer=Element.hide.delay(0.25,'bs_".$this->pid."')\"

            value=\"".preg_replace('/"/','&quot;',$initialValue)."\">";

        // Autocompleter
        $this->tree.= "<div id='autocomplete_".$this->pid."' class='autocomplete'></div>".
            "<script type='text/javascript'>".
            "var mouseIsStillOver = false;".
            "new Ajax.Autocompleter('bs_input_".$this->pid."', 'autocomplete_".$this->pid."', 'autocomplete.php?type=base', { minChars: 3, frequency: 0.5 });";
        if ($this->submitButton) {
            $this->tree.= "$('bs_input_".$this->pid."').observe('keypress', function(event) { if(event.keyCode == Event.KEY_RETURN) { $('submit_base_".$this->pid."').click(); } });";
        }
        $this->tree.= "</script>";

        $this->tree.= "<div class='treeList' style='display:none;max-height:".$this->height."px' id='bs_".$this->pid."' onmouseover=\"window.clearTimeout(rtimer);\" onmouseout=\"rtimer= Element.hide.delay(0.25, 'bs_".$this->pid."')\">

    <ul class='treeList'>\n";
        $first= true;
        $last_indent= 1;

        foreach ($this->pathMapping as $base => $dummy) {

            // Skip root for tree
            if ($base == $this->releaseBase) {
#                continue;
            }

            // Build path style display
            $elements= explode('/', substr($base, strlen($this->releaseBase), strlen($base)));
            $indent= count($elements) ;
       
            if (!$first && ($indent == $last_indent)) {
                $this->tree.= "</li>\n";
            }
            if ($indent > $last_indent) {
                $this->tree.= "<ul>\n";
            }
            if ($indent < $last_indent) {
                for ($i= 0; $i < ($last_indent-$indent); $i++) {
                    $this->tree.= "</li></ul>\n";
                }
                $this->tree.= "</li>\n";
            }
            $selected= $this->base == $base?" class='treeListSelected'":"";
            $link= "onclick=\"\$('bs_rebase_".$this->pid."').value='".base64_encode($base)."';$('submit_tree_base_".$this->pid."').click();\"";

            $this->tree.= "<li>".
                "<a$selected $link>".
                $this->gennonbreaks($this->releaseInfo[$base]['name']).
                ($this->releaseInfo[$base]['description']==''?'':'&nbsp;<span class="informal">['.$this->gennonbreaks($this->releaseInfo[$base]['description']).']</span>').
                "</a>";

            $last_indent= $indent;
            $first= false;
        }

        // Close tree
        for ($i= 1; $i<$last_indent; $i++) {
            $this->tree.= "</li></ul>\n";
        }
        $this->tree.= "</div>\n";

        // Draw submitter if required
        if ($this->submitButton) {
            $this->tree.= image('images/lists/submit.png', "submit_base_".$this->pid, _("Submit"));
        }
        $this->tree.= "<input type='submit' style='display:none' name='submit_tree_base_".$this->pid."' id='submit_tree_base_".$this->pid."'>";
        $this->tree.= "<input type='hidden' name='bs_rebase_".$this->pid."' id='bs_rebase_".$this->pid."'>";
        $this->tree.= "<input type='hidden' name='BPID_{$this->pid}' id='BPID_{$this->pid}' value='".$this->pid."'>";

        $this->lastState= true;
        return true;
    }


    function gennonbreaks($string)
    {
        return str_replace('-', '&#8209;', str_replace(' ', '&nbsp;', $string));
    }


    function render()
    {
        return $this->tree;
    }


    function getBase()
    {
        return $this->base;
    }


    function getAction()
    {
        // Do not do anything if this is not our BPID, or there's even no BPID available...
        if(!isset($_REQUEST["BPID_{$this->pid}"]) || $_REQUEST["BPID_{$this->pid}"] != $this->pid) {
            return;
        }

        if ($this->action) {
            return array("targets" => array($this->base), "action" => $this->action);
        }

        return null;
    }

}

?>
