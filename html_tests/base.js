var spirograf = function(){
	var cs = document.getElementById("c-sprigraph");
	cs.width = 800;
	cs.height = 800;
	var ctx = cs.getContext("2d");
	ctx.fillStyle = "#fff";	
	
	var R = 130;
	var r = 50;
	var d = 30;
	var teta = 0.0;
	var timer;
	
	function spiro() {
		var x = (R-r)*Math.cos(teta) + d*Math.cos(teta*(R-r)/r);
		var y = (R-r)*Math.sin(teta) - d*Math.sin(teta*(R-r)/r);
		teta = teta + 0.1;	
		ctx.fillRect(400+x, 400+y, 3, 3);
		timer = setTimeout(spiro, 50);
	}
	
	function onChange() {
		var t = document.getElementById("Rb").value;
		if (! isNaN(t)) {
			R = t;
		}
		t = document.getElementById("Rm").value;
		if (! isNaN(t)) {
			r = t;
		}
		t = document.getElementById("d").value;
		if (! isNaN(t)) {
			d = t;
		}
		console.log(R, r, d);
		ctx.clearRect(0, 0, 800, 800);
	}
	
	function initListener() {
		var l = document.getElementsByTagName("input");
		for (var i = 0; i < l.length; i++) {
			l[i].onchange = onChange;
		}
	}
	
	initListener();
	spiro();
}