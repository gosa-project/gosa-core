	function populate(objForm,resultForm) {
		if(objForm.year) {
			yearSel=objForm.year.options[objForm.year.selectedIndex].text;
		
			monthIx=objForm.month.selectedIndex;
			nextMonthIx=0;
			if(monthIx!=11) {
				nextMonthIx=monthIx+1;
			}

			nextMonthSel=objForm.month.options[nextMonthIx].value;
			monthSel=objForm.month.options[monthIx].value;

			daySel=1;
			if(objForm.day.selectedIndex!=-1) {
				daySel=objForm.day.options[objForm.day.selectedIndex].text;
			}

			timeOfDayOne = new Date(yearSel, nextMonthSel, 1);
			timeDifference = timeOfDayOne - 86400000;
			timeOfLastDay = new Date(timeDifference);

			var daysInMonth = timeOfLastDay.getDate();

			if(daySel>daysInMonth) daySel=daysInMonth;

			selectedDate= new Date(yearSel,monthSel,daySel);
			for (var i = 0; i < objForm.day.length; i++) {
				objForm.day.options[0] = null;
			}
			for (var i = 0; i < daysInMonth; i++) {
				objForm.day.options[i] = new Option(i+1);
			}
			objForm.day.selectedIndex=daySel-1;
			saveResult(selectedDate,resultForm);
		}
	}

	function createResult(objForm,resultForm){
		yearSel=objForm.year.options[objForm.year.selectedIndex].text;
		monthIx=objForm.month.selectedIndex;
		monthSel=objForm.month.options[monthIx].value;
		daySel=objForm.day.options[objForm.day.selectedIndex].value;
		selectedDate= new Date(yearSel,monthSel,daySel);
		saveResult(selectedDate,resultForm);
	}

	function saveResult(selectedDate,resultForm){
		timeInSeconds=Math.floor((selectedDate.getTime() - selectedDate.getTimezoneOffset() * 60000 ) / 1000);
		resultForm.value=timeInSeconds;
	}
	
	function populate_sambaLogonTime(objForm,resultForm) {
		var yearSel=objForm.sambaLogonTime_year.options[objForm.sambaLogonTime_year.selectedIndex].text;
		var monthIx=objForm.sambaLogonTime_month.selectedIndex;
		var nextMonthIx=0;
		if(monthIx!=11)
			nextMonthIx=monthIx+1;

		var nextMonthSel=objForm.sambaLogonTime_month.options[nextMonthIx].value;
		var monthSel=objForm.sambaLogonTime_month.options[monthIx].value;

		var daySel=1;
		if(objForm.sambaLogonTime_day.selectedIndex!=-1)
			daySel=objForm.sambaLogonTime_day.options[objForm.sambaLogonTime_day.selectedIndex].value;

		var timeOfDayOne = new Date(yearSel, nextMonthSel, 1);
		var timeDifference = timeOfDayOne - 86400000;
		var timeOfLastDay = new Date(timeDifference);

		var daysInMonth = timeOfLastDay.getDate();

		if(daySel>daysInMonth) daySel=daysInMonth;

		var selectedDate= new Date(yearSel,monthSel,daySel);
		for (var i = 0; i < objForm.sambaLogonTime_day.length; i++) {
			objForm.sambaLogonTime_day.options[0] = null;
		}
		for (var i = 0; i < daysInMonth; i++) {
			objForm.sambaLogonTime_day.options[i] = new Option(i+1);
		}
		objForm.sambaLogonTime_day.selectedIndex=daySel-1;
		saveResult(selectedDate,resultForm);
	}
	function populate_sambaLogoffTime(objForm,resultForm) {
		var yearSel=objForm.sambaLogoffTime_year.options[objForm.sambaLogoffTime_year.selectedIndex].text;
		var monthIx=objForm.sambaLogoffTime_month.selectedIndex;
		var nextMonthIx=0;
		if(monthIx!=11)
			nextMonthIx=monthIx+1;

		var nextMonthSel=objForm.sambaLogoffTime_month.options[nextMonthIx].value;
		var monthSel=objForm.sambaLogoffTime_month.options[monthIx].value;

		var daySel=1;
		if(objForm.sambaLogoffTime_day.selectedIndex!=-1)
			daySel=objForm.sambaLogoffTime_day.options[objForm.sambaLogoffTime_day.selectedIndex].value;

		var timeOfDayOne = new Date(yearSel, nextMonthSel, 1);
		var timeDifference = timeOfDayOne - 86400000;
		var timeOfLastDay = new Date(timeDifference);

		var daysInMonth = timeOfLastDay.getDate();

		if(daySel>daysInMonth) daySel=daysInMonth;

		var selectedDate= new Date(yearSel,monthSel,daySel);
		for (var i = 0; i < objForm.sambaLogoffTime_day.length; i++) {
			objForm.sambaLogoffTime_day.options[0] = null;
		}
		for (var i = 0; i < daysInMonth; i++) {
			objForm.sambaLogoffTime_day.options[i] = new Option(i+1);
		}
		objForm.sambaLogoffTime_day.selectedIndex=daySel-1;
		saveResult(selectedDate,resultForm);
	}
	function populate_sambaKickoffTime(objForm,resultForm) {
		var yearSel=objForm.sambaKickoffTime_year.options[objForm.sambaKickoffTime_year.selectedIndex].text;
		var monthIx=objForm.sambaKickoffTime_month.selectedIndex;
		var nextMonthIx=0;
		if(monthIx!=11)
			nextMonthIx=monthIx+1;

		var nextMonthSel=objForm.sambaKickoffTime_month.options[nextMonthIx].value;
		var monthSel=objForm.sambaKickoffTime_month.options[monthIx].value;

		var daySel=1;
		if(objForm.sambaKickoffTime_day.selectedIndex!=-1)
			daySel=objForm.sambaKickoffTime_day.options[objForm.sambaKickoffTime_day.selectedIndex].value;

		var timeOfDayOne = new Date(yearSel, nextMonthSel, 1);
		var timeDifference = timeOfDayOne - 86400000;
		var timeOfLastDay = new Date(timeDifference);

		var daysInMonth = timeOfLastDay.getDate();

		if(daySel>daysInMonth) daySel=daysInMonth;

		var selectedDate= new Date(yearSel,monthSel,daySel);
		for (var i = 0; i < objForm.sambaKickoffTime_day.length; i++) {
			objForm.sambaKickoffTime_day.options[0] = null;
		}
		for (var i = 0; i < daysInMonth; i++) {
			objForm.sambaKickoffTime_day.options[i] = new Option(i+1);
		}
		objForm.sambaKickoffTime_day.selectedIndex=daySel-1;
		saveResult(selectedDate,resultForm);
	}

	function populate_sambaPwdCanChange(objForm,resultForm) {

		var yearSel=objForm.sambaPwdCanChange_year.options[objForm.sambaPwdCanChange_year.selectedIndex].text;
		var monthIx=objForm.sambaPwdCanChange_month.selectedIndex;
		var nextMonthIx=0;
		if(monthIx!=11)
			nextMonthIx=monthIx+1;

		var nextMonthSel=objForm.sambaPwdCanChange_month.options[nextMonthIx].value;
		var monthSel=objForm.sambaPwdCanChange_month.options[monthIx].value;

		var daySel=1;
		if(objForm.sambaPwdCanChange_day.selectedIndex!=-1)
			daySel=objForm.sambaPwdCanChange_day.options[objForm.sambaPwdCanChange_day.selectedIndex].value;

		var timeOfDayOne = new Date(yearSel, nextMonthSel, 1);
		var timeDifference = timeOfDayOne - 86400000;
		var timeOfLastDay = new Date(timeDifference);

		var daysInMonth = timeOfLastDay.getDate();

		if(daySel>daysInMonth) daySel=daysInMonth;

		var selectedDate= new Date(yearSel,monthSel,daySel);
		for (var i = 0; i < objForm.sambaPwdCanChange_day.length; i++) {
			objForm.sambaPwdCanChange_day.options[0] = null;
		}
		for (var i = 0; i < daysInMonth; i++) {
			objForm.sambaPwdCanChange_day.options[i] = new Option(i+1);
		}
		objForm.sambaPwdCanChange_day.selectedIndex=daySel-1;
		saveResult(selectedDate,resultForm);
	}

	function createResult_sambaLogonTime(objForm,resultForm){
		yearSel=objForm.sambaLogonTime_year.options[objForm.sambaLogonTime_year.selectedIndex].text;
		monthIx=objForm.sambaLogonTime_month.selectedIndex;
		monthSel=objForm.sambaLogonTime_month.options[monthIx].value;
		daySel=objForm.sambaLogonTime_day.options[objForm.sambaLogonTime_day.selectedIndex].value;
		selectedDate= new Date(yearSel,monthSel,daySel);
		saveResult(selectedDate,resultForm);
	}
	function createResult_sambaLogoffTime(objForm,resultForm){
		yearSel=objForm.sambaLogoffTime_year.options[objForm.sambaLogoffTime_year.selectedIndex].text;
		monthIx=objForm.sambaLogoffTime_month.selectedIndex;
		monthSel=objForm.sambaLogoffTime_month.options[monthIx].value;
		daySel=objForm.sambaLogoffTime_day.options[objForm.sambaLogoffTime_day.selectedIndex].value;
		selectedDate= new Date(yearSel,monthSel,daySel);
		saveResult(selectedDate,resultForm);
	}
	function createResult_sambaKickoffTime(objForm,resultForm){
		yearSel=objForm.sambaKickoffTime_year.options[objForm.sambaKickoffTime_year.selectedIndex].text;
		monthIx=objForm.sambaKickoffTime_month.selectedIndex;
		monthSel=objForm.sambaKickoffTime_month.options[monthIx].value;
		daySel=objForm.sambaKickoffTime_day.options[objForm.sambaKickoffTime_day.selectedIndex].value;
		selectedDate= new Date(yearSel,monthSel,daySel);
		saveResult(selectedDate,resultForm);
	}
	function createResult_sambaPwdCanChange(objForm,resultForm){
		yearSel=objForm.sambaPwdCanChange_year.options[objForm.sambaPwdCanChange_year.selectedIndex].text;
		monthIx=objForm.sambaPwdCanChange_month.selectedIndex;
		monthSel=objForm.sambaPwdCanChange_month.options[monthIx].value;
		daySel=objForm.sambaPwdCanChange_day.options[objForm.sambaPwdCanChange_day.selectedIndex].value;
		selectedDate= new Date(yearSel,monthSel,daySel);
		saveResult(selectedDate,resultForm);
	}

    function createResult2(objForm,resultForm){
        yearSel=objForm.syear.options[objForm.syear.selectedIndex].text;
        monthIx=objForm.smonth.selectedIndex;
        monthSel=objForm.smonth.options[monthIx].value;
        daySel=objForm.sday.options[objForm.sday.selectedIndex].value;
        selectedDate= new Date(yearSel,monthSel,daySel);
        saveResult(selectedDate,resultForm);
    }

function populate2(objForm,resultForm) {
        if(objForm.syear) {
            yearSel=objForm.syear.options[objForm.syear.selectedIndex].text;

            monthIx=objForm.smonth.selectedIndex;
            nextMonthIx=0;
            if(monthIx!=11) {
                nextMonthIx=monthIx+1;
            }

            nextMonthSel=objForm.smonth.options[nextMonthIx].value;
            monthSel=objForm.smonth.options[monthIx].value;

            daySel=1;
            if(objForm.sday.selectedIndex!=-1) {
                daySel=objForm.sday.options[objForm.sday.selectedIndex].text;
            }

            timeOfDayOne = new Date(yearSel, nextMonthSel, 1);
            timeDifference = timeOfDayOne - 86400000;
            timeOfLastDay = new Date(timeDifference);

            var daysInMonth = timeOfLastDay.getDate();

            if(daySel>daysInMonth) daySel=daysInMonth;

            selectedDate= new Date(yearSel,monthSel,daySel);
            for (var i = 0; i < objForm.sday.length; i++) {
                objForm.sday.options[0] = null;
            }
            for (var i = 0; i < daysInMonth; i++) {
                objForm.sday.options[i] = new Option(i+1);
            }
            objForm.sday.selectedIndex=daySel-1;
            saveResult(selectedDate,resultForm);
        }
    }

