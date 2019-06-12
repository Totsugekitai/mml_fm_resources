var geovisitFlg;
var onloadFlg;

var url = 'http://' + document.domain+location.pathname;
var ref = document.referrer;

var siteHeight = 0;

function canUseGetBoundingClientRect() {

	var ua = window.navigator.userAgent.toLowerCase();
	var ver = window.navigator.appVersion.toLowerCase();

	if (ua.indexOf("msie") != -1) {

		if (ver.indexOf("msie 7.") != -1) {
			return false
		} else if (ver.indexOf("msie 8.") != -1) {
			return false
		} else if (ver.indexOf("msie 9.") != -1) {
			return false
		}

	}

	return true;

}

/* exec */

function geovisit() {

	if (geovisitFlg) { return; }
	geovisitFlg = true;

	if (adFlg === 'gg') {
		writeUAd();
	}

}

if (window.addEventListener) {
	window.addEventListener("load", execWriteUadOnload, false);
} else if (window.attachEvent) {
	window.attachEvent("onload", execWriteUadOnload);
} else {
	window.onload = execWriteUadOnload;
}

function execWriteUadOnload() {

	if (onloadFlg) { return; }
	onloadFlg = true;

	if (adFlg === 'sq') {
		writeUAd();
	}

}

function checkUad() {

	var ua = window.navigator.userAgent.toLowerCase();
	var ver = window.navigator.appVersion.toLowerCase();
	if (ua.indexOf("msie") != -1) {
		if (ver.indexOf("msie 7.") != -1 && ver.indexOf("trident") == -1) {
			return false;
		}
	}

	var adElement = document.getElementById("y_gc_div_adcntr");
	if (adElement == null) {
		return false;
	}

	var adFrame = adElement.getElementsByTagName("iFrame");
	if (adFrame.length == 0) {
		return false;
	}

	if (adFrame.item(0).width == 0 && adFrame.item(0).height == 0) {
		return false;
	}

	return true;

}

/* writeAd */

//uad

function writeUAd() {

	if (!checkUad()) { return; }

	var doubleMdRecWrapper = document.createElement('div');
	doubleMdRecWrapper.id = 'ydn-double-md-rec-wrapper';
	doubleMdRecWrapper.style.textAlign = 'center';
	doubleMdRecWrapper.style.width = '100%';
	doubleMdRecWrapper.style.left = '0';

	if (adFlg === 'sq') {

		measureSiteHeight();	// siteHeight
		var sqHeight = getSqBottomHeight();

		if (siteHeight < sqHeight) {
			siteHeight = sqHeight
		}

		doubleMdRecWrapper.style.position = 'absolute';
		doubleMdRecWrapper.style.top = siteHeight + 'px';
	}

	var uadWrapper = document.createElement('div');
	uadWrapper.id = 'y_gc_div_uad_wrapper';

	document.body.appendChild(doubleMdRecWrapper);
	doubleMdRecWrapper.appendChild(uadWrapper);

	var uadElement = getUAd();
	uadWrapper.appendChild(uadElement);

}

function getUAd() {

	var element = document.createElement('div');
	element.id = 'y_gc_div_uadcntr';
	element.style.visibility = "visible";
	element.style.position = "relative";
	element.style.zIndex = 2147483647;
	element.style.clear = "both";

	var ifrmHeight = 480;
	var shpAdFlg = 0;

	element.innerHTML = "<center><iframe style=\"border:none;\" src=\"https://bc-geocities.yahoo.co.jp/bc/uad?i=" + jps + "&u=" + encodeURIComponent(url) + '&ref=' + encodeURIComponent(ref) + '&ad=' + encodeURIComponent(adFlg)+ '&shp=' + encodeURIComponent(shpAdFlg) + "\" width=615 height=" + ifrmHeight + " border=0 marginwidth=0 marginheight=0 hspace=0 vspace=0 frameborder=0 scrolling=no allowtransparency=\"true\"></iframe></center>";
	return element;

}

function getSqBottomHeight() {

	var divAu1 = document.getElementById("y_gc_div_au1");
	if (divAu1) {
		var sqRect = divAu1.getBoundingClientRect();
		var sqbottomHeight = sqRect.top + sqRect.height;
	}

	var scrollTop = document.body.scrollTop || document.documentElement.scrollTop;
	sqbottomHeight += scrollTop;
	return sqbottomHeight;

}

function measureSiteHeight() {

	if (!canUseGetBoundingClientRect()) {
		var bodyHeight = Math.max.apply( null, [document.body.clientHeight, document.body.scrollHeight, document.documentElement.scrollHeight, document.documentElement.clientHeight]);
		if ( siteHeight < bodyHeight ) {
			siteHeight = bodyHeight;
		}
	} else {
		getSiteHeightFromElem(document.body);
		var scrollTop = document.body.scrollTop || document.documentElement.scrollTop;
		siteHeight += scrollTop;
	}

}

function getSiteHeightFromElem(elem) {

	if (elem.getBoundingClientRect) {

		var children = elem.children;

		for (var i = 0; i < children.length; i++) {

			var bottomY = 0;

			var rect = children[i].getBoundingClientRect();
			bottomY = rect.top + rect.height;

			if ( siteHeight < bottomY ) {
				siteHeight = bottomY;
			}
			getSiteHeightFromElem(children[i]);

		}

	}

}
