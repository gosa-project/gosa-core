/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2010 GONICUS GmbH
 *
 * ID: $$Id: index.php 15301 2010-01-26 09:40:08Z cajus $$
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

/* Install event handlers */
Event.observe(window, 'resize', resizeHandler);
Event.observe(window, 'load', resizeHandler);
Event.observe(window, 'load', initProgressPie);
Event.observe(window, 'keypress', keyHandler);


/* Ask before switching a plugin with this function */
function question(text, url) {
    if (document.mainform.ignore || $('pluginModified') == null || $('pluginModified').value == 0) {
        location.href = url;
        return true;
    }
    if (confirm(text)) {
        location.href = url;
        return true;
    }
    return false;
}


/* Toggle checkbox that matches regex */
function chk_set_all(regex, value) {
    for (var i = 0; i < document.mainform.elements.length; i++) {
        var _id = document.mainform.elements[i].id;
        if (_id.match(regex)) {
            document.getElementById(_id).checked = value;
        }
    }
}


function toggle_all_(regex, state_object) {
    state = document.getElementById(state_object).checked;
    chk_set_all(regex, state);
}


/* Scroll down the body frame */
function scrollDown2() {
    document.body.scrollTop = document.body.scrollHeight - document.body.clientHeight;
}


/* Toggle checkbox that matches regex */
function acl_set_all(regex, value) {
    for (var i = 0; i < document.mainform.elements.length; i++) {
        var _id = document.mainform.elements[i].id;
        if (_id.match(regex)) {
            document.getElementById(_id).checked = value;
        }
    }
}

/* Toggle checkbox that matches regex */
function acl_toggle_all(regex) {
    for (var i = 0; i < document.mainform.elements.length; i++) {
        var _id = document.mainform.elements[i].id;
        if (_id.match(regex)) {
            if (document.getElementById(_id).checked == true) {
                document.getElementById(_id).checked = false;
            } else {
                document.getElementById(_id).checked = true;
            }
        }
    }
}


/* Global key handler to estimate which element gets the next focus if enter is pressed */
function keyHandler(DnEvents) {

    var element = Event.element(DnEvents);

    // determines whether Netscape or Internet Explorer
    k = (Prototype.Browser.Gecko) ? DnEvents.keyCode : window.event.keyCode;
    if (k == 13 && element.type != 'textarea') { // enter key pressed

        // Stop 'Enter' key-press from beeing processed internally
        Event.stop(DnEvents);

        // No nextfield explicitly specified
        var next_element = null;
        if (typeof(nextfield) != 'undefined') {
            next_element = $(nextfield);
        }

        // nextfield not given or invalid
        if (!next_element || typeof(nextfield) == 'undefined') {
            next_element = getNextInputElement(element);
        }

        if (element != null && element.type == 'submit') {

            // If the current element is of type submit, then submit the button else set focus
            element.click();
            return (false);
        } else if (next_element != null && next_element.type == 'submit') {

            // If next element is of type submit, then submit the button else set focus
            next_element.click();
            return (false);
        } else if (next_element) {
            next_element.focus();
            return;
        }

    } else if (k == 9 && element.type == 'textarea') {
        Event.stop(DnEvents);
        element.value += "\t";
        return false;
    }
}

function getNextInputElement(element) {
    var found = false;
    for (var e = 0; e < document.forms.length; e++) {
        for (var i = 0; i < document.forms[e].elements.length; i++) {
            var el = document.forms[e].elements[i]
            if (found && !el.disabled && el.type != 'hidden' && !el.name.match(/^submit_tree_base/) && !el.name.match(/^bs_rebase/)) {
                return (el);
            }
            if ((el.id != "" && el.id == element.id) || (el.name != "" && el.name == element.name)) {
                found = true;
            }
        }
    }
}

function changeState() {
    for (var i = 0; i < arguments.length; i++) {
        var element = $(arguments[i]);
        if (element.hasAttribute('disabled')) {
            element.removeAttribute('disabled');
        } else {
            element.setAttribute('disabled', 'disabled');
        }
    }
}

function changeSelectState(triggerField, myField) {
    if (document.getElementById(triggerField).value != 2) {
        document.getElementById(myField).disabled = true;
    } else {
        document.getElementById(myField).disabled = false;
    }
}

function changeSubselectState(triggerField, myField) {
    if (document.getElementById(triggerField).checked == true) {
        document.getElementById(myField).disabled = false;
    } else {
        document.getElementById(myField).disabled = true;
    }
}

function changeTripleSelectState(firstTriggerField, secondTriggerField, myField) {
    if (
        document.getElementById(firstTriggerField).checked == true &&
        document.getElementById(secondTriggerField).checked == true) {
        document.getElementById(myField).disabled = false;
    } else {
        document.getElementById(myField).disabled = true;
    }
}

<!-- Second field must be non-checked -->
function changeTripleSelectState_2nd_neg(firstTriggerField, secondTriggerField, myField) {
    if (
        document.getElementById(firstTriggerField).checked == true &&
        document.getElementById(secondTriggerField).checked == false) {
        document.getElementById(myField).disabled = false;
    } else {
        document.getElementById(myField).disabled = true;
    }
}


function popup(target, name) {
    var mypopup =
        window.open(
            target,
            name,
            "width=600,height=700,location=no,toolbar=no,directories=no,menubar=no,status=no,scrollbars=yes"
        );
    mypopup.focus();
    return false;
}

function js_check(form) {
    form.javascript.value = 'true';
}

function divGOsa_toggle(element) {
    var cell;
    var cellname = "tr_" + (element);

    if (Prototype.Browser.Gecko) {
        document.poppedLayer = document.getElementById(element);
        cell = document.getElementById(cellname);

        if (document.poppedLayer.style.visibility == "visible") {
            $(element).hide();
            cell.style.height = "0px";
            document.poppedLayer.style.height = "0px";
        } else {
            $(element).show();
            document.poppedLayer.style.height = "";
            if (document.defaultView) {
                cell.style.height = document.defaultView.getComputedStyle(document.poppedLayer, "").getPropertyValue('height');
            }
        }
    } else if (Prototype.Browser.IE) {
        document.poppedLayer = document.getElementById(element);
        cell = document.getElementById(cellname);
        if (document.poppedLayer.style.visibility == "visible") {
            $(element).hide();
            cell.style.height = "0px";
            document.poppedLayer.style.height = "0px";
            document.poppedLayer.style.position = "absolute";
        } else {
            $(element).show();
            cell.style.height = "";
            document.poppedLayer.style.height = "";
            document.poppedLayer.style.position = "relative";
        }
    }
}

function resizeHandler(e) {
    if (!e) e = window.event;

    // This works with FF / IE9. If Apples resolves a bug in webkit,
    // it works with Safari/Chrome, too.
    if ($("d_scrollbody") && $("t_nscrollbody")) {
        var contentHeight = document.viewport.getHeight() - 216;
        if ($$('div.plugin-actions').length != 0) {
            var height = 0;
            $$('div.plugin-actions').each(function(s) {
                height += s.getHeight();
            });
            contentHeight -= height + 25;
        }

        if (Prototype.Browser.IE) {
            document.getElementById('d_scrollbody').style.height = contentHeight + 23 + 'px';
            document.getElementById('t_nscrollbody').style.height = contentHeight + 'px';
        } else {
            document.getElementById('d_scrollbody').style.minHeight = contentHeight + 23 + 'px';
            document.getElementById('t_nscrollbody').style.minHeight = contentHeight + 'px';
        }
    }

    return true;
}


function absTop(e) {
    return (e.offsetParent) ? e.offsetTop + absTop(e.offsetParent) : e.offsetTop;
}

/* Set focus to first valid input field
   avoid IExplorer warning about hidding or disabled fields
 */
function focus_field() {
    var i = 0;
    var e = 0;
    var found = false;
    var element_name = "";
    var element = null;

    while (focus_field.arguments[i] && !found) {

        var tmp = document.getElementsByName(focus_field.arguments[i]);
        for (e = 0; e < tmp.length; e++) {

            if (isVisible(tmp[e])) {
                found = true;
                element = tmp[e];
                break;
            }
        }
        i++;
    }

    if (element && found) {
        element.blur();
        element.focus();
    }
}


/*  This function pops up messages from message queue
    All messages are hidden in html output (style='display:none;').
    This function makes single messages visible till there are no more dialogs queued.

    hidden inputs:
    current_msg_dialogs		- Currently visible dialog
    closed_msg_dialogs		- IDs of already closed dialogs
    pending_msg_dialogs		- Queued dialog IDs.
 */
function next_msg_dialog() {
    var s_pending = "";
    var a_pending = new Array();
    var i_id = 0;
    var i = 0;
    var tmp = "";
    var ele = null;
    var ele2 = null;
    var cur_id = "";

    if (document.getElementById('current_msg_dialogs')) {
        cur_id = document.getElementById('current_msg_dialogs').value;
        if (cur_id != "") {
            ele = document.getElementById('e_layer' + cur_id);
            ele.onmousemove = "";
            $('e_layer' + cur_id).hide();
            document.getElementById('closed_msg_dialogs').value += "," + cur_id;
            document.getElementById('current_msg_dialogs').value = "";
        }
    }

    if (document.getElementById('pending_msg_dialogs')) {
        s_pending = document.getElementById('pending_msg_dialogs').value;
        a_pending = s_pending.split(",");
        if (a_pending.length) {
            i_id = a_pending.pop();
            for (i = 0; i < a_pending.length; ++i) {
                tmp = tmp + a_pending[i] + ',';
            }
            tmp = tmp.replace(/,$/g, "");
            if (i_id != "") {
                ele = document.getElementById('e_layer' + i_id);
                ele3 = document.getElementById('e_layerTitle' + i_id);
                ele.style.display = 'block';
                document.getElementById('pending_msg_dialogs').value = tmp;
                document.getElementById('current_msg_dialogs').value = i_id;
                ele2 = document.getElementById('e_layer2');
                ele3.onmousedown = start_move_div_by_cursor;
                ele2.onmouseup = stop_move_div_by_cursor;
                ele2.onmousemove = move_div_by_cursor;
            } else {
                ele2 = document.getElementById('e_layer2');
                ele2.style.display = "none";
            }
        }
    }
}


/* Drag & drop for message dialogs */
var enable_move_div_by_cursor = false; // Indicates wheter the div movement is enabled or not
var mouse_x_on_div = 0; //
var mouse_y_on_div = 0;
var div_offset_x = 0;
var div_offset_y = 0;

/* Activates msg_dialog drag & drop
 * This function is called when clicking on a displayed msg_dialog
 */
function start_move_div_by_cursor(e) {
    var x = 0;
    var y = 0;
    var cur_id = 0;
    var dialog = null;
    var event = null;

    /* Get current msg_dialog position
     */
    cur_id = document.getElementById('current_msg_dialogs').value;
    if (cur_id != "") {
        dialog = document.getElementById('e_layer' + cur_id);
        x = dialog.style.left;
        y = dialog.style.top;
        x = x.replace(/[^0-9]/g, "");
        y = y.replace(/[^0-9]/g, "");
        if (!y) y = 1;
        if (!x) x = 1;
    }

    /* Get mouse position within msg_dialog
     */
    if (window.event) {
        event = window.event;
        if (event.offsetX) {
            div_offset_x = event.clientX - x;
            div_offset_y = event.clientY - y;
            enable_move_div_by_cursor = true;
        }
    } else if (e) {
        event = e;
        if (event.layerX) {
            div_offset_x = event.screenX - x;
            div_offset_y = event.screenY - y;
            enable_move_div_by_cursor = true;
        }
    }
}


/* Deactivate msg_dialog movement
 */
function stop_move_div_by_cursor() {
    mouse_x_on_div = 0;
    mouse_y_on_div = 0;
    div_offset_x = 0;
    div_offset_y = 0;
    enable_move_div_by_cursor = false;
}


/* Move msg_dialog with cursor */
function move_div_by_cursor(e) {
    var event = false;
    var mouse_pos_x = 0;
    var mouse_pos_y = 0;
    var cur_div_x = 0;
    var cur_div_y = 0;
    var cur_id = 0;
    var dialog = null;


    if (undefined !== enable_move_div_by_cursor && enable_move_div_by_cursor == true) {

        if (document.getElementById('current_msg_dialogs')) {

            /* Get mouse position on screen
             */
            if (window.event) {
                event = window.event;
                mouse_pos_x = event.clientX;
                mouse_pos_y = event.clientY;
            } else if (e) {
                event = e;
                mouse_pos_x = event.screenX;
                mouse_pos_y = event.screenY;
            } else {
                return;
            }

            /* Get id of current msg_dialog */
            cur_id = document.getElementById('current_msg_dialogs').value;
            if (cur_id != "") {
                dialog = document.getElementById('e_layer' + cur_id);

                /* Calculate new position */
                cur_div_x = mouse_pos_x - div_offset_x;
                cur_div_y = mouse_pos_y - div_offset_y;

                /* Ensure that dialog can't be moved out of screen */
                if (cur_div_x < 0) cur_div_x = 0
                if (cur_div_y < 0) cur_div_y = 0

                /* Assign new values */
                dialog.style.left = (cur_div_x) + "px";
                dialog.style.top = (cur_div_y) + "px";
            }
        }
    }
}


function isVisible(obj) {
    if (obj == document) return true

    if (!obj) return false
    if (!obj.parentNode) return false
    if (obj.style) {
        if (obj.style.display == 'none') return false
        if (obj.style.visibility == 'hidden') return false
    }

    //Try the computed style in a standard way
    if (window.getComputedStyle) {
        var style = window.getComputedStyle(obj, "")
        if (style.display == 'none') return false
        if (style.visibility == 'hidden') return false
    }

    //Or get the computed style using IE's silly proprietary way
    var style = obj.currentStyle
    if (style) {
        if (style['display'] == 'none') return false
        if (style['visibility'] == 'hidden') return false
    }

    return isVisible(obj.parentNode)
}


/* Check if capslock is enabled */
function capslock(e) {
    e = (e) ? e : window.event;

    var charCode = false;
    if (e.which) {
        charCode = e.which;
    } else if (e.keyCode) {
        charCode = e.keyCode;
    }

    var shifton = false;
    if (e.shiftKey) {
        shifton = e.shiftKey;
    } else if (e.modifiers) {
        shifton = !!(e.modifiers & 4);
    }

    if (charCode >= 97 && charCode <= 122 && shifton) {
        return true;
    }

    if (charCode >= 65 && charCode <= 90 && !shifton) {
        return true;
    }

    return false;
}

function setProgressPie(context, percent) {
    context.clearRect(0, 0, 22, 22);

    var r = "FF";
    var g = "FF";
    var b = "FF";

    // // Fade yellow
    // if (percent > 50) {
    //     d = 255 - parseInt((percent-50) * 255 / 50)
    //         b = d.toString(16);
    // }

    // // Fade red
    // if (percent > 75) {
    //     d = 255 - parseInt((percent-75) * 255 / 25)
    //         g = d.toString(16);
    // }

    context.strokeStyle = "#" + r + g + b
    context.fillStyle = context.strokeStyle;

    context.beginPath();
    context.moveTo(11, 11)
    context.arc(11, 11, 8, -Math.PI / 2, -Math.PI / 2 + Math.PI * percent / 50, true);
    context.closePath();
    context.fill();

    context.moveTo(11, 11)
    context.beginPath();
    context.arc(11, 11, 8, 0, Math.PI * 2, false);
    context.closePath();
    context.stroke();
}

function initProgressPie() {
    var canvas = $('sTimeout');

    // Check the element is in the DOM and the browser supports canvas
    if (canvas && canvas.getContext) {
        var percent = 0.01;
        var context = canvas.getContext('2d');
        setProgressPie(context, percent);

        // Extract timeout and title string out out canvas.title
        var data = canvas.title;
        var timeout = data.replace(/\|.*$/, '');
        var title = data.replace(/^.*\|/, '');
        var interval = 1;
        var time = 0;
        setInterval(function() {

            // Calculate percentage
            percent += (interval / timeout) * 100;

            // Increase current time by interval
            time += interval;

            // Generate title
            var minutes = parseInt((timeout - time) / 60);
            var seconds = '' + parseInt((timeout - time) % 60);
            if (seconds.length == 1) seconds = '0' + seconds;
            minutes = minutes + ':' + seconds;

            // Set new  canval title
            canvas.title = title.replace(/%d/, minutes);
            setProgressPie(context, percent);

            if (percent > 99) percent = 99;
        }, (interval * 1000));
    }
}

/* Scroll down the body frame */
function scrollDown2() {
    document.body.scrollTop = document.body.scrollHeight - document.body.clientHeight;
}


// Global storage for baseSelector timer
var rtimer;

// vim:ts=2:syntax