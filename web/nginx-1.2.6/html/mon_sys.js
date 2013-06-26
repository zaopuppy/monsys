
// (function() {
// })();
////////////////////////////////////////////////////////////////////////////////////
// for drawing
var g_monsys = new Monsys();
var g_canvas_elem = null;
var g_canvas = null;
var g_img = new Image();

var g_select = null;
var g_switch = null;
var g_light_level = null;

var g_mouse_down = false;
var g_current_mouse_pos = null;


window.onload = function() {
	console.log("initializing...");

	g_monsys.init();
	g_monsys.xml_http.onreadystatechange = onHTTPReadStateChange;

	// -------------------------
	// --- select ----
	g_select = document.getElementById("dev-list");
	// -------------------------
	// canvas
	g_canvas_elem = document.getElementById("main-canvas");

	g_canvas_elem.addEventListener("click", on_click, false);
	// mouse
	g_canvas_elem.addEventListener("mousemove", on_mouse_move, false);
	g_canvas_elem.addEventListener("mousedown", on_mouse_down, false);
	g_canvas_elem.addEventListener("mouseup", on_mouse_up, false);
	// touch(for touch devices)
	g_canvas_elem.addEventListener("touchmove", on_mouse_move, false);
	g_canvas_elem.addEventListener("touchstart", on_mouse_down, false);
	g_canvas_elem.addEventListener("touchend", on_mouse_up, false);
	// g_canvas = g_canvas_elem.getContext("2d");
	// g_canvas = g_canvas_elem.getContext("2d");
	// g_img.src = "ClownFish.png";
	g_img.src = "Shapes.png";
	g_img.onload = on_image_laoded;

	// setTimeout(on_draw, 100);
	// No, we can't invoke this method here, on_draw() will call setTimeout(), it will
	// make a closure so g_canvas in on_draw will never change
	// on_draw();
	// -------------------------
	// check box
	g_switch = document.getElementById("dev-switch");
	g_switch.addEventListener("click", on_switch_changed, false);
	// -------------------------
	// light level
	g_light_level = document.getElementById("light-level");
	// g_light_level.onchange = on_light_changed;
	g_light_level.addEventListener("mouseup", on_light_changed, false);
	g_light_level.addEventListener("touchend", on_light_changed, false);
	// -------------------------

	console.log("initialized");
};

function on_image_laoded() {
	console.log("on_image_laoded()");
	if (g_canvas_elem != null) {
		g_canvas_elem.width = g_img.width;
		g_canvas_elem.height = g_img.height;
		g_canvas = g_canvas_elem.getContext("2d");
		// --- for debugging ---
		g_canvas.fillStyle = "#00FF00";
		g_canvas.fillRect(20, 20, 20, 20);
		// --- for debugging ---
		on_draw();
	}
}

function on_light_changed() {
	console.log("light changed: " + g_light_level.value);

	// get address
	var selectedIndex = g_select.selectedIndex;
	if (selectedIndex < 0) {
		console.log("You should select at least on device.");
		// g_switch.checked = !g_switch.checked;
		return;
	}

	var addr = parseInt(g_select.options[selectedIndex].value);

	var v = parseInt(g_light_level.value);
	g_monsys.set_dev_info(44, addr, [11, 12, 13], [v, v, v]);
}

function on_switch_changed() {
	console.log("switch changed: " + g_switch.checked);

	// get address
	var selectedIndex = g_select.selectedIndex;
	if (selectedIndex < 0) {
		console.log("You should select at least on device.");
		g_switch.checked = !g_switch.checked;
		return;
	}

	var addr = parseInt(g_select.options[selectedIndex].value);

	if (g_switch.checked) {
		g_monsys.set_dev_info(44, addr, [4], [1]);
	} else {
		g_monsys.set_dev_info(44, addr, [4], [0]);
	}
}

// simplest
// Modern browser's now seem to handle this for you now.
// Chrome and IE9 (at least) support the offsetX/Y and
// Firefox supports layerX/Y. The following function gives
// me what I need. Just pass it the event from the click handler.
function getRelativeCoords(event) {
    if (event.offsetX !== undefined && event.offsetY !== undefined) {
    	return { x: event.offsetX, y: event.offsetY };
    }

    return { x: event.layerX, y: event.layerY };
}

// most vote
// http://stackoverflow.com/questions/55677/how-do-i-get-the-coordinates-of-a-mouse-click-on-a-canvas-element
function relMouseCoords(event){
    var totalOffsetX = 0;
    var totalOffsetY = 0;
    var canvasX = 0;
    var canvasY = 0;
    var currentElement = this;

    do{
        totalOffsetX += currentElement.offsetLeft - currentElement.scrollLeft;
        totalOffsetY += currentElement.offsetTop - currentElement.scrollTop;
    }
    while(currentElement = currentElement.offsetParent)

    canvasX = event.pageX - totalOffsetX;
    canvasY = event.pageY - totalOffsetY;

    return {x:canvasX, y:canvasY}
}

function on_mouse_down(event) {
	// 0: left, 1: middle, 2: right
	if (event.button != 0) {
		return;
	}

	g_mouse_down = true;

	g_current_mouse_pos = getRelativeCoords(event);
}

function on_mouse_up(event) {
	if (event.button != 0) {
		return;
	}

	console.log("on_mouse_up");

	g_mouse_down = false;

	// check pos
	var pos = g_current_mouse_pos;
	if (pos == null) {
		console.log("pos == null");
		return;
	}

	// image
	var p = g_canvas.getImageData(pos.x, pos.y, 1, 1).data;
	console.log("R: " + p[0] + ", G: " + p[1] + ", B: " + p[2]);

	// addr
	var selectedIndex = g_select.selectedIndex;
	if (selectedIndex < 0) {
		console.log("You should select at least on device.");
		// g_switch.checked = !g_switch.checked;
		return;
	}

	var addr = parseInt(g_select.options[selectedIndex].value);

	// R, G, B
	g_monsys.set_dev_info(44, addr, [11, 12, 13], [p[0], p[1], p[2]]);
}

function on_click(event) {
	var pos = getRelativeCoords(event);
	// console.log("on_click: (" + pos.x + ", " + pos.y + ")");
}

function on_mouse_move(event) {
	if (g_mouse_down) {
		var pos = getRelativeCoords(event);
		g_current_mouse_pos = pos;
		// console.log("on_mouse_move: (" + pos.x + ", " + pos.y + ")");
	}
}

function on_draw() {
	if (g_canvas == null) {
		console.log("g_canvas is not initialized yet.");
		return;
	}

	// console.log("on_draw()");
	g_canvas.drawImage(g_img, 0, 0);

	// g_canvas.moveTo(5, 5);
	var pos = g_current_mouse_pos;
	if (pos != null) {
		var p = g_canvas.getImageData(pos.x, pos.y, 1, 1).data;
		// g_canvas.fillStyle = "#FF0000";
		g_canvas.fillStyle = "#" + ("000000" + rgbToHex(p[0], p[1], p[2])).slice(-6);
		g_canvas.fillRect(5, 5, 50, 50);
	} else {
		// set color whick get from devices
		g_canvas.fillStyle = "#FFFFFF";
		g_canvas.fillRect(5, 5, 50, 50);
	}

	g_canvas.strokeStyle = "#FFFFFF";
	g_canvas.strokeRect(5, 5, 50, 50);


	setTimeout(on_draw, 100);
}

function rgbToHex(r, g, b) {
  if (r > 255 || g > 255 || b > 255)
    throw "Invalid color component";
  return ((r << 16) | (g << 8) | b).toString(16);
}

////////////////////////////////////////////////////////////////////////////////////
// API
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

	this.send_json = function (jobj) {
		var jobj_str = JSON.stringify(jobj);
		this.send_raw(jobj_str);
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

	this.set_dev_info = function(uid, addr, dev_ids, dev_vals) {

		if (dev_ids.length != dev_vals.length) {
			console.log("ids-length != vals-length");
			return;
		}

		var id_val_array = [];

		for (var i = dev_ids.length - 1; i >= 0; i--) {
			id_val_array.push({ "id": dev_ids[i], "val": dev_vals[i]});
		}

		var jreq = {
			"cmd": "set-dev-info",
			"uid": uid,
			"addr": addr,
			"vals": id_val_array
		};

		this.send_json(jreq);
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

	// send_raw("get-dev-list-input");

	var jreq = {
		"cmd": "get-dev-list",
		"uid": 44
	};

	g_monsys.send_json(jreq);

	// console.log("size of dev-list: " + select_obj.size);
	// console.log("selected: " + select_obj.selectedIndex);
	// if (select_obj.selectedIndex >= 0) {
	// 	console.log("value: " + select_obj.options[select_obj.selectedIndex].value);
	// } else {
	// 	console.log("no value");
	// }
	// // console.log("");
	// var op = document.createElement("option");
	// op.text = "new";
	// op.value = 444;
	// select_obj.add(op, null);
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

function onGetDevListRsp (jobj) {
	// clear
	for (var i = g_select.length - 1; i >= 0; i--) {
		g_select.remove(0);
	};

	// refresh
	if (jobj.hasOwnProperty("devs")) {
		var devs = jobj["devs"];
		for (var i = 0; i < devs.length; ++i) {
			var op = document.createElement("option");
			// should be a real name
			op.text = devs[i]["addr"];
			op.value = devs[i]["addr"];
			g_select.add(op, null);
		}
	}
}

function onHTTPReadStateChange() {
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


