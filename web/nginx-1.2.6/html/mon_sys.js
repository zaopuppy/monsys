
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

	this.send_raw = function(raw_text) {
		console.log("sending:");
		console.log("[" + raw_text + "]");
		this.xml_http.open("POST", "monsys", true);
		this.xml_http.send(raw_text);
	};

	this.get_dev_info = function(uid, dev_id, addr) {
		// TODO:
		// var req_obj = {
		// 	"cmd": "get-dev-info",
		// 	"uid": "uid001",
		// 	"dev-id": "5000",
		// };
		var req_obj = "{ \"cmd\": \"get-dev-info\", \"uid\": " + uid + ", \"dev-id\": " + dev_id + ", \"addr\": " + addr + " }";
		this.xml_http.open("POST", "monsys", true);
		this.xml_http.send(req_obj);
	};

	this.set_dev_info = function(uid, dev_id, dev_vals) {
		// TODO:
		// var req_obj = {
		// 	"cmd": "set-dev-info",
		// 	"uid": 001,
		// 	"dev-id": 5000,
		//  "vals": [
		//		{ "id": 1, "val": 2},
		//		{ "id": 2, "val": 4} ]
		// };
		// [ {"id":1, "val":4}, {"id":2, "val":5} ]
		var req_obj = "{ \"cmd\": \"set-dev-info\", \"uid\": " + uid + ", \"dev-id\": " + dev_id + ", \"vals\": " + dev_vals + " }";
		this.xml_http.open("POST", "monsys", true);
		this.xml_http.send(req_obj);
	};
}

function send_raw(text_id) {
	console.log("get_dev_list()");

	var raw_text = document.getElementById(text_id).value;

	if (raw_text.length == 0) {
		console.log("No, it's empty!");
		return;
	}

	g_monsys.send_raw(raw_text);
}

function get_dev_list() {
	send_raw("get-dev-list-input");
}
function get_dev_info() {
	send_raw("get-dev-info-input");
}
function set_dev_info() {
	send_raw("set-dev-info-input");
}

function test002() {
	console.log("Test002");

	var tbl = document.getElementById("params");
	var uid = tbl.rows[0].cells[1].firstChild.value;
	var dev_id = tbl.rows[1].cells[1].firstChild.value;
	var dev_vals = tbl.rows[2].cells[1].firstChild.value;

	console.log("uid: " + uid);
	console.log("dev-id: " + dev_id);
	console.log("dev-vals: " + dev_vals);
	g_monsys.set_dev_info(uid, dev_id, dev_vals);
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


