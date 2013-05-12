
// (function() {
// })();

var g_monsys = new Monsys();

window.onload = function() {
	console.log("initializing...");
	g_monsys.init();
	g_monsys.xml_http.onreadystatechange = onHTTPReadStateChange;
	console.log("initialized");
};

function Monsys() {
	// "undefined"
	this.xml_http = null;

	this.init = function() {
		if (window.XMLHttpRequest) {
			this.xml_http = new XMLHttpRequest();
		} else {
			this.xml_http = new ActiveXObject("Microsoft.XMLHTTP");
		}
		// this.xmlHttp.onreadystatechange = onHTTPReadStateChange;
	}

	this.get_dev_info = function(uid, dev_id) {
		// TODO:
		// var req_obj = {
		// 	"cmd": "get-dev-info",
		// 	"uid": "uid001",
		// 	"dev-id": "5000",
		// };
		var req_obj = "{ \"cmd\": \"get-dev-info\", \"uid\": " + uid + ", \"dev-id\": " + dev_id + " }";
		this.xml_http.open("POST", "monsys", true);
		this.xml_http.send(req_obj);
	};
	this.set_dev_info = function(uid, dev_id) {
		// TODO:
		// var req_obj = {
		// 	"cmd": "set-dev-info",
		// 	"uid": "uid001",
		// 	"dev-id": "5000",
		// };
		var req_obj = "{ \"cmd\": \"set-dev-info\", \"uid\": " + uid + ", \"dev-id\": " + dev_id + " }";
		this.xml_http.open("POST", "monsys", true);
		this.xml_http.send(req_obj);
	};
}

function test001() {
	console.log("Test001");

	var tbl = document.getElementById("params");
	var uid = tbl.rows[0].cells[1].firstChild.value;
	var dev_id = tbl.rows[1].cells[1].firstChild.value;

	console.log("uid: " + uid);
	console.log("dev_id: " + dev_id);
	g_monsys.get_dev_info(uid, dev_id);
}
function test002() {
	console.log("Test002");

	var tbl = document.getElementById("params");
	var uid = tbl.rows[0].cells[1].firstChild.value;
	var dev_id = tbl.rows[1].cells[1].firstChild.value;

	console.log("uid: " + uid);
	console.log("dev_id: " + dev_id);
	g_monsys.set_dev_info(uid, dev_id);
}
function test003() {
	console.log("Test003");
}
function test004() {
	console.log("Test004");
}

function onHTTPReadStateChange() {
	if (g_monsys.xml_http.readyState == 4) {
		console.log("4: request finished and response is ready");
		console.log("status: " + g_monsys.xml_http.status);
		console.log(g_monsys.xml_http.responseText);
	} else if (g_monsys.xml_http.readyState == 1) {
		console.log("1: server connection established");
	} else if (g_monsys.xml_http.readyState == 2) {
		console.log("2: request received");
	} else if (g_monsys.xml_http.readyState == 3) {
		console.log("3: processing request");
	} else {
		console.log("0: request not initialized");
		console.log("bu...");
	}
}


