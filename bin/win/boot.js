var hxchrome;
if(!hxchrome) hxchrome = {};
if(!hxchrome.fs) hxchrome.fs = {};
(function() {
	hxchrome.fs.readFile = function(fileName) {
		native function readFile(fileName);
		return readFile(fileName);
	};
})();