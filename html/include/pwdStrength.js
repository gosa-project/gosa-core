
/* ************************************************************
Created: 20060120
Author:  Steve Moitozo <god at zilla dot us>
Description: This is a quick and dirty password quality meter 
		 written in JavaScript
License: MIT License (see below)
=================================
Revision Author: Dick Ervasti (dick dot ervasti at quty dot com)
Revision Description: Exchanged text based prompts for a graphic thermometer
=================================
Revision Author: Jay Bigam jayb <o> tearupyourlawn <o> com
Revision Date: Feb. 26, 2007
Revision Description: Changed D. Ervasti's table based "thermometer" to CSS.
Revision Notes: - Verified to work in FF2, IE7 and Safari2
                - Modified messages to reflect Minimum strength requirement
                - Added formSubmit button disabled until minimum requirement met
=================================
Modified: 20061111 - Steve Moitozo corrected regex for letters and numbers
                     Thanks to Zack Smith -- zacksmithdesign.com
                     and put MIT License back in
					 
---------------------------------------------------------------
Copyright (c) 2006 Steve Moitozo <god at zilla dot us>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

   The above copyright notice and this permission notice shall
be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
---------------------------------------------------------------
 
USAGE: 

You can play with the pwdTest div style to make it go where you want.
 
In this case it sits to the right side of the input

<!-- Code To Add To HTML Form Page -->

<!-- the include -->
<script type="text/javascript" src="scripts/pwdStrength.js"></script>

<!-- the input -->
<input type="text" name="password" id="password" onkeyup="testPassword(document.forms.YourFormName.password.value);">

<!-- the "meter" -->
<div id="pwdTest" style="padding:3px;float:right;font-size:.8em;width:180px;min-width:220;border:1px dashed;margin-top:-40px;">
	Password Strength:<br>
	<span id="meterEmpty" style="padding:0;margin:0;width:100%;background-color:##DC143C;display:block;height:5px;">
	<span id="meterFull" style="padding:0;margin:0;z-index:100;width:0;background-color:##006400;display:block;height:5px;"></span></span>
	<span id="Words" style="font-size:.8em; margin-top;-25px;">Minimum Strength Not Met</span>
</div>		
<!-- the submit button -->
<input disabled type="submit" id ="formSubmit">	
<!-- End of HTML -->

===================================
Password Strength Factors and Weightings

password length:
level 0 (3 point): less than 4 characters
level 1 (6 points): between 5 and 7 characters
level 2 (12 points): between 8 and 15 characters
level 3 (18 points): 16 or more characters

letters:
level 0 (0 points): no letters
level 1 (5 points): all letters are lower case
level 2 (7 points): letters are mixed case

numbers:
level 0 (0 points): no numbers exist
level 1 (5 points): one number exists
level 1 (7 points): 3 or more numbers exists

special characters:
level 0 (0 points): no special characters
level 1 (5 points): one special character exists
level 2 (10 points): more than one special character exists

combinatons:
level 0 (1 points): letters and numbers exist
level 1 (1 points): mixed case letters
level 1 (2 points): letters, numbers and special characters 
					exist
level 1 (2 points): mixed case letters, numbers and special 
					characters exist


************************************************************ */
function testPasswordCss(passwd)
{
		var description = new Array();
		description[0] = "Minimum Strength Not Met";
		description[1] = "Weak";
		description[2] = "Improving";
		description[3] = "Strong";
		description[4] = "Strongest";
		description[5] = "Begin Typing";

		var intScore   = 0
		var strVerdict = 0
		
		// PASSWORD LENGTH
		if (passwd.length==0 || !passwd.length)                         // length 0
		{
			intScore = -1
		}
		else if (passwd.length>0 && passwd.length<5) // length between 1 and 4
		{
			intScore = (intScore+3)
		}
		else if (passwd.length>4 && passwd.length<8) // length between 5 and 7
		{
			intScore = (intScore+6)
		}
		else if (passwd.length>7 && passwd.length<12)// length between 8 and 15
		{
			intScore = (intScore+12)
		}
		else if (passwd.length>11)                    // length 16 or more
		{
			intScore = (intScore+18)
		}
		
		
		// LETTERS (Not exactly implemented as dictacted above because of my limited understanding of Regex)
		if (passwd.match(/[a-z]/))                              // [verified] at least one lower case letter
		{
			intScore = (intScore+1)
		}
		
		if (passwd.match(/[A-Z]/))                              // [verified] at least one upper case letter
		{
			intScore = (intScore+5)
		}
		
		// NUMBERS
		if (passwd.match(/\d+/))                                 // [verified] at least one number
		{
			intScore = (intScore+5)
		}
		
		if (passwd.match(/(.*[0-9].*[0-9].*[0-9])/))             // [verified] at least three numbers
		{
			intScore = (intScore+5)
		}
		
		
		// SPECIAL CHAR
		if (passwd.match(/.[!,@,#,$,%,^,&,*,?,_,~]/))            // [verified] at least one special character
		{
			intScore = (intScore+5)
		}
		
																 // [verified] at least two special characters
		if (passwd.match(/(.*[!,@,#,$,%,^,&,*,?,_,~].*[!,@,#,$,%,^,&,*,?,_,~])/))
		{
			intScore = (intScore+5)
		}
	
		
		// COMBOS
		if (passwd.match(/([a-z].*[A-Z])|([A-Z].*[a-z])/))        // [verified] both upper and lower case
		{
			intScore = (intScore+2)
		}

        if (passwd.match(/([a-zA-Z])/) && passwd.match(/([0-9])/)) // [verified] both letters and numbers
        {
                intScore = (intScore+2)
        }

																  // [verified] letters, numbers, and special characters
		if (passwd.match(/([a-zA-Z0-9].*[!,@,#,$,%,^,&,*,?,_,~])|([!,@,#,$,%,^,&,*,?,_,~].*[a-zA-Z0-9])/))
		{
			intScore = (intScore+2)
		}

	
//if you don't want to prevent submission of weak passwords you can comment out
//		   document.getElementById("formSubmit").disabled = true;
	
		if(intScore == -1)
		{
		   strVerdict = description[5];
		   document.getElementById("meterEmpty").style.width= "100%";
   		   document.getElementById("meterFull").style.width= "0";
   		   document.getElementById("formSubmit").disabled = true;
		}
		else if(intScore > -1 && intScore < 16)
		{
		   strVerdict = description[0];
		   document.getElementById("meterEmpty").style.width= "100%";
   		   document.getElementById("meterFull").style.width= "0%";
   		   document.getElementById("formSubmit").disabled = true;
		}
		else if (intScore > 15 && intScore < 25)
		{
		   strVerdict = description[1];
		   document.getElementById("meterEmpty").style.width= "100%";
   		   document.getElementById("meterFull").style.width= "25%";
   		   document.getElementById("formSubmit").disabled = false;
		}
		else if (intScore > 24 && intScore < 35)
		{
		   strVerdict = description[2];
		  document.getElementById("meterEmpty").style.width= "100%";
   		  document.getElementById("meterFull").style.width= "50%";
   		  document.getElementById("formSubmit").disabled = false;
		}
		else if (intScore > 34 && intScore < 45)
		{
		   strVerdict = description[3];
		   document.getElementById("meterEmpty").style.width= "100%";
   		   document.getElementById("meterFull").style.width= "75%";
   		   document.getElementById("formSubmit").disabled = false;
		}
		else
		{
		   strVerdict = description[4];
		   document.getElementById("meterEmpty").style.width= "100%";
   		   document.getElementById("meterFull").style.width= "100%";
   		   document.getElementById("formSubmit").disabled = false;
		}
	
	//Changed by <pollmeier@gonicus.de>: no need for words
	//document.getElementById("Words").innerHTML= (strVerdict);
	
}
