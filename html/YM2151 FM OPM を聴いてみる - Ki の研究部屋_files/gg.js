var adFlg = 'gg';
var divAuFlg = false;

var url = 'http://' + document.domain+location.pathname;
var ref = document.referrer;

divAu();

function divAu() {

	if (divAuFlg) { return; }
	divAuFlg=true;

	if (!checkDisplayAd()) { return; }

	writeGgAd();

}

function checkDisplayAd() {

	if (isFrameset()) { return false; }

	if (parent.frames.length != 0) {
		if(!checkFrame()) { return false; }
	}

	return true;

}

function isFrameset() {

	var numFr = 0;
	numFr = frames.length;
	numFr = numFr - document.getElementsByTagName("iframe").length;
	isFr = (numFr > 0) ? true : false;

	return isFr;

}

function checkFrame() {

	var x = 0;
	var y = 0;

	if (document.body) { x = document.body.clientWidth; }
	if (document.body) { y = document.body.clientHeight; }

	return (x > 400 && y > 300) ? true : false;

}

// gg

function writeGgAd() {

        document.write("<div id=y_gc_div_adcntr class=y_gcss_ovrtr_au><iframe src=\"https://bc-geocities.yahoo.co.jp/bc/gg?i=" + jps + "&u=" + encodeURIComponent(url) + "&ref=" + encodeURIComponent(ref) + "\" width=740 height=103 border=0 marginwidth=0 marginheight=0 hspace=0 vspace=0 frameborder=0 scrolling=no>no iframe</iframe></div>");
	document.getElementById('y_gc_div_adcntr').style.visibility = "visible";
	document.getElementById('y_gc_div_adcntr').style.position = "relative";
	document.getElementById('y_gc_div_adcntr').style.zIndex = 2147483647;

}
