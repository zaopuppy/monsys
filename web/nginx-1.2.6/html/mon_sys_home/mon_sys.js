

// DOM events:
// click, keypress, submit, load
// dblclick	keydown	change	resize
// mouseenter	keyup	focus	scroll
// mouseleave	 	blur	unload

var g_monsys = null;
var g_smart_light_ctrl = null;
var g_main_ctrl = null;

(function () {
	console.log("initializing...")

	// on ready
	$(document).ready(function() {
		console.log("Ready");

		// for device list
		g_monsys = new Monsys();
		g_monsys.init(onHTTPReadyStateChange);

		// for main control
		g_main_ctrl = new MainCtrl();
		g_main_ctrl.init();

		// for smart light
		g_smart_light_ctrl = new SmartLightControl();
		g_smart_light_ctrl.init();

		// initial main UI
		hide_all_panel();
		g_main_ctrl.get_dev_list();
		g_main_ctrl.show();

		// refresh device list every 10 seconds
		// onGetDevListRsp({"devs": []});

		// auto_get_dev_list();
		// setTimeout(auto_get_dev_list(), 5*1000);
	});

	console.log("initialized")
}());

function auto_get_dev_list() {
	g_main_ctrl.get_dev_list();
	setTimeout(auto_get_dev_list, DEV_LIST_REFRESH_INTERVAL);
}

function do_opera_device(addr, type) {
	console.log("do_opera_device()");
	hide_all_panel();
	if (type == DEV_TYPE_SMART_LIGHT) {
		show_smart_light_panel(addr);
	} else {
		show_unknown_device_panel(addr);
	}
}

function show_main_control_panel() {
	hide_all_panel();
	g_main_ctrl.show();
}

function hide_all_panel() {
	console.log("hide_all_panel()");
	// $("#main-control-panel").hide();
	g_main_ctrl.hide();
	g_smart_light_ctrl.hide();
	$("#unknwon-device-panel").hide();
}

function show_smart_light_panel(addr) {
	g_smart_light_ctrl.addr_ = addr;
	g_smart_light_ctrl.show();
}

function show_unknown_device_panel(addr) {
	$("#unknwon-device-panel").show();
}

function onGetDevListRsp(jobj) {
	console.log("onGetDevListRsp()");

	g_main_ctrl.clear_device_list_box();
	g_main_ctrl.update_list_box(jobj);
}

function onHTTPReadyStateChange() {
	if (g_monsys.xml_http.readyState == 4) {
		console.log("4: request finished and response is ready");
		console.log("status: " + g_monsys.xml_http.status);
		console.log(g_monsys.xml_http.responseText);

		var rsp = null;

		try {
			rsp = eval("(" + g_monsys.xml_http.responseText + ")");
		} catch (e) {
			console.log("bad json");
			rsp = null;
		}

		if (rsp != null && rsp.hasOwnProperty("cmd")) {
			console.log("response message: " + rsp["cmd"]);
			if (rsp["cmd"] == "get-dev-list-rsp") {
				onGetDevListRsp(rsp);
			}
		}
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

