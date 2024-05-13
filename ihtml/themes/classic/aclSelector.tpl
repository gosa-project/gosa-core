<input type='hidden' name='acl_dummy_0_0_0' value='1'>
<button type='button' style='width:100px;' name='toggle_all_create' onClick="acl_toggle_all('_0_c$');" >Toggle C</button>
<button type='button' style='width:100px;' name='toggle_all_move'   onClick="acl_toggle_all('_0_m$');" >Toggle M</button>
<button type='button' style='width:100px;' name='toggle_all_remove' onClick="acl_toggle_all('_0_d$');" >Toggle D</button> - 
<button type='button' style='width:100px;' name='toggle_all_read'   onClick="acl_toggle_all('_0_r$');" >Toggle R</button>
<button type='button' style='width:100px;' name='toggle_all_write'  onClick="acl_toggle_all('_0_w$');" >Toggle W</button> - 
<button type='button' style='width:100px;' name='toggle_all_sub_read'  onClick="acl_toggle_all('[^0]_r$');" >R+</button>
<button type='button' style='width:100px;' name='toggle_all_sub_write'  onClick="acl_toggle_all('[^0]_w$');" >W+</button>
<br>
<button type='button' style='width:50px;' name='set_true_all_create' onClick="acl_set_all('_0_c$',true);" >C+</button>
<button type='button' style='width:50px;' name='set_false_all_create' onClick="acl_set_all('_0_c$',false);" >C-</button>
<button type='button' style='width:50px;' name='set_true_all_move' onClick="acl_set_all('_0_m$',true);" >M+</button>
<button type='button' style='width:50px;' name='set_false_all_move' onClick="acl_set_all('_0_m$',false);" >M-</button>
<button type='button' style='width:50px;' name='set_true_all_remove' onClick="acl_set_all('_0_d$',true);" >D+</button>
<button type='button' style='width:50px;' name='set_false_all_remove' onClick="acl_set_all('_0_d$',false);" >D-</button> - 
<button type='button' style='width:50px;' name='set_true_all_read' onClick="acl_set_all('_0_r$',true);" >R+</button>
<button type='button' style='width:50px;' name='set_false_all_read' onClick="acl_set_all('_0_r$',false);" >R-</button>
<button type='button' style='width:50px;' name='set_true_all_write' onClick="acl_set_all('_0_w$',true);" >W+</button>
<button type='button' style='width:50px;' name='set_false_all_write' onClick="acl_set_all('_0_w$',false);" >W-</button> - 
<button type='button' style='width:50px;' name='set_true_all_read' onClick="acl_set_all('[^0]_r$',true);" >R+</button>
<button type='button' style='width:50px;' name='set_false_all_read' onClick="acl_set_all('[^0]_r$',false);" >R-</button>
<button type='button' style='width:50px;' name='set_true_all_write' onClick="acl_set_all('[^0]_w$',true);" >W+</button>
<button type='button' style='width:50px;' name='set_false_all_write' onClick="acl_set_all('[^0]_w$',false);" >W-</button>