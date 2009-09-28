function initDatepickers() {
	$$("*").findAll(function(node) {
		return Element.hasClassName(node,'date');
	}).each(function(node) {
		datepickers[node.id] = new DatePicker({
			relative 			: node.id,
			language 			: 'de',
			enableCloseEffect : false,
			enableShowEffect 	: true,
			closeEffect			: 'squish',
			showEffect			: 'appear'
		});
		datepickers[node.id].setDateFormat([ "dd", "mm", "yyyy" ], ".");
	});
}
Event.observe(window, 'load', initDatepickers, false);
