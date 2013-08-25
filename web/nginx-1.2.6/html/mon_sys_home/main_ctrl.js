

var DEV_TYPE_SMART_LIGHT = 1;
var DEV_LIST_REFRESH_INTERVAL = 10 * 1000;

function MainCtrl() {
  // "undefined"
  this.xml_http = null;

  this.init = function() {
  	// --- for debugging only ---
  	this.update_list_box(null);
  	// --- for debugging only ---
  }

  this.show = function() {
		// hide_all_panel();
		// this.get_dev_list();
		$("#main-control-panel").show();
	}
	this.hide = function() {
		$("#main-control-panel").hide();
	}

	this.get_dev_list = function() {
		var jreq = {
			"cmd": "get-dev-list",
			"uid": 44
		};

		g_monsys.send_json(jreq);
	}

	this.clear_device_list_box = function() {
		$('#device-list').empty();
	}

	this.gen_record = function(dev_info) {
		var record = $('<div></div>');

		// icon
		var img = this.get_icon_by_type(dev_info["type"]);
		// button
		var btn = $('<input type="button"></input>');
		btn.val(dev_info["name"]);

		// append them
		record.append(img);
		record.append(btn);
		btn.click(function(addr, type) {
			return function() {
				console.log("operation on device: addr=" + addr + ", type=" + type);
				do_opera_device(addr, type);
			}
		}(dev_info["addr"], dev_info["type"]));

		return record;
	}

	this.update_list_box = function(dev_info_list) {

		this.clear_device_list_box();

		// --- for debugging only ---
		dev_info_list = {"cmd": "get-dev-list-rsp", "devs": [{"type": 0, "addr": 0, "name": "dev-00", "state": 0}, {"type": 1, "addr": 1, "name": "dev-01", "state": 1}, {"type": 2, "addr": 2, "name": "dev-02", "state": 2}, {"type": 3, "addr": 3, "name": "dev-03", "state": 3}, {"type": 4, "addr": 4, "name": "dev-04", "state": 4}], "status": 0};
		// --- for debugging only ---

		// refresh
		// TODO: should do it at the very beginning
		// if (jobj.hasOwnProperty("devs"))
		// var tbl = $("#device-table");
		var devs = dev_info_list["devs"];
		if (devs.length <= 0) {
			// append empty notice
			var notice = $('<p>No device was found:(</p>');
			$('#device-list').append(notice);
			return;
		}

		var device_list = $('#device-list');

		for (var i = 0; i < devs.length; ++i) {
			var record = this.gen_record(devs[i]);
			device_list.append(record);
		}

	}

	this.update_list_box_old = function(dev_info_list) {

		this.clear_device_list_box();

		// --- for debugging only ---
		dev_info_list = {"cmd": "get-dev-list-rsp", "devs": [{"type": 0, "addr": 0, "name": "dev-00", "state": 0}, {"type": 1, "addr": 1, "name": "dev-01", "state": 1}, {"type": 2, "addr": 2, "name": "dev-02", "state": 2}, {"type": 3, "addr": 3, "name": "dev-03", "state": 3}, {"type": 4, "addr": 4, "name": "dev-04", "state": 4}], "status": 0};
		// --- for debugging only ---

		// refresh
		// TODO: should do it at the very beginning
		// if (jobj.hasOwnProperty("devs"))
		// var tbl = $("#device-table");
		var devs = dev_info_list["devs"];
		if (devs.length <= 0) {
			// append empty notice
			var notice = $('<p>No device was found:(</p>');
			$('#device-list').append(notice);
			return;
		}

		var tbl = $('<ul id="device-table"></ul>');
		for (var i = 0; i < devs.length; ++i) {
			var row = $('<li class="device-record"></li>');

			// Image, name, desc, state, operate
			var imageCell = $('<span class="image"></span>');
			var nameCell = $('<span class="name"></span>');

			// image
			var img = this.get_icon_by_type(devs[i]["type"]);
			// can't use img's width, for async loading...
			// imageCell.width(48);
			imageCell.append(img);
			// name
			// nameCell.text(devs[i]["name"]);
			// nameCell.width(80);
			// XXX
			// nameCell.width($('#device-list').width() - imageCell.width() - 48);
			// desc
			// descCell.text(devs[i]["desc"]);
			// descCell.width(400);
			// opera
			var btn = $('<input type="button"></input>');
			btn.val(devs[i]["name"]);
			btn.click(function(addr, type) {
				return function() {
					console.log("operation on device: addr=" + addr + ", type=" + type);
					do_opera_device(addr, type);
				}
			}(devs[i]["addr"], devs[i]["type"]));
			// operaCell.append(btn);
			nameCell.append(btn);

			// append all elements
			row.append(imageCell);
			row.append(nameCell);
			// row.append(descCell);
			// row.append(operaCell);
			tbl.append(row);
		}

		$('#device-list').append(tbl);
	}

	this.get_icon_by_type = function(type) {
		var img = null;
		if (type == DEV_TYPE_SMART_LIGHT) {
			img = $('<img src="Safari_Agua.png"></img>');		
		} else {
			img = $('<img src="Audio_MIDI.png"></img>');		
		}

		return img;
	}


}

