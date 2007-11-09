
/* Register resize event to force redraw of all
 *  displayed lists
 */
window.onload = updateObjectListViewportSize;
window.onresize = updateObjectListViewportSize;

   function updateObjectListViewportSize() {

		var header 		= 0;
		var footer 		= 0;
		var viewport	= 0;
		var viewport_w	= 0;
		var all 		= new Array();
		var i			= 0;
		var id_pref		= "";
		var calc		= 0;
	
        /* Somehow IE do not need to be resized
         *  So skip the following part if the client uses ie
         */
        if(document.all && !window.opera){
            all = getElementsByStyleClass('ObjectListViewport_Entry_Cover');
            for (i = 0; i < all.length; ++i){
                document.getElementById(all[i].id).style.height= "100%";
            }
            return;
        }

        all = getElementsByStyleClass('ObjectListViewport');
        for (i = 0; i < all.length; ++i){

            id_pref = all[i].id.replace(/[^0-9]*/,"");

            /* Set Viewport to min height, to ensure
             *  that resize will work correctly in konqueror
             */
            document.getElementById('ObjectListViewport_Entry_Cover' + id_pref).style.height= "50px";

            /* Get values of displayed header and footer heights to be
             *  able to recalculate the Viewport
             */
            viewport    = getObjectHeight('ObjectListViewport_Table' + id_pref);

			/* Get Header height for ViewPort height calculation */
			if(document.getElementById('ObjectListViewport_TD_Header' + id_pref)){
            	header = getObjectHeight('ObjectListViewport_TD_Header' + id_pref);
			}else{
				header =0;
			}
		
			/* Get Footer height for ViewPort height calculation */
			if(document.getElementById('ObjectListViewport_TD_Footer' + id_pref)){
            	footer = getObjectHeight('ObjectListViewport_TD_Footer' + id_pref);
			}else{
				footer = 0
			}

            /* Calculate the new visible entry part height. */
            calc = (viewport  - ( header + footer ));
            document.getElementById('ObjectListViewport_Entry_Cover' + id_pref).style.height = calc;

            /* Reduce width of entry list, we need some space to
             *  display the scrollbar without breaking the layout
			 * Only required if a header is shown.
             */
			if(document.getElementById('ObjectListViewport_TD_Header' + id_pref)){
				viewport_w = getObjectWidth('ObjectListViewport_Table' + id_pref);
				document.getElementById('ObjectListViewport_Entry_Table' + id_pref).style.width = (viewport_w - 18) + 'px';
				document.getElementById('ObjectListViewport_Header_Table' + id_pref).style.width = (viewport_w - 18) + 'px';
			}
        }
    }


    /* Return all elements of a specific class */
    function getElementsByStyleClass (className) {
        var all = document.all ? document.all : document.getElementsByTagName('*');
        var elements = new Array();
        for (var e = 0; e < all.length; e++)
            if (all[e].className == className)
                elements[elements.length] = all[e];
        return elements;
    }


    /* Return integer value of style attribute width for specified ID  */
    function getObjectWidth(obj){
		if(document.getElementById(obj)){
			obj = getComputedStyle(document.getElementById(obj),"").width;
			return(parseInt(obj.replace(/px/,'')));
		}else{
			return(0);
		}
    }

    /* Return integer value of style attribute height for specified ID  */
    function getObjectHeight(obj){
		if(document.getElementById(obj)){
			obj = getComputedStyle(document.getElementById(obj),"").height;
			return(parseInt(obj.replace(/px/,'')));
		}else{
			return(0);
		}
    }
