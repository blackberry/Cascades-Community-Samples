function randomColor(opacity) {
	return 'rgba(' + Math.round(Math.random() * 255) + ', ' + Math.round(Math.random() * 255)
			+ ', ' + Math.round(Math.random() * 255) + ',' + opacity + ')';
}
function createRandomArray(size, sourceArray) {
	var d = [];
	for (var i = 0; i < size; i++) {
		var h = parseInt((Math.random() * sourceArray.length), 10);
		d[i] = sourceArray[h % sourceArray.length];
	}
	return d;
}
function createRandomPointsArray(size, sourceArray) {
	var d = [];
	for (var i = 0; i < size; i++) {
		var h1 = parseInt((Math.random() * sourceArray.length), 10);
		var h2 = parseInt((Math.random() * sourceArray.length), 10);
		var h3 = parseInt((Math.random() * sourceArray.length), 10);
		d[i] = {
			x : h1,
			y : h2,
			r : Math.abs(h3) % 10 + 10
		// +10 to increase the minimum size
		};
	}
	return d;
}
function createBackgroundColorArray(num, opacity) {
	var d = [];
	for (var i = 0; i < num; i++) {
		d[i] = randomColor(opacity);
	}
	return d;
}
function createBorderColorArray(num) {
	var c = randomColor(1);
	var d = [];
	for (var i = 0; i < num; i++) {
		d[i] = c;
	}
	return d;
}
function createLabelArray(prefix, num) {
	var d = [];
	for (var i = 0; i < num; i++) {
		d[i] = prefix + ' ' + (i + 1);
	}
	return d;
}