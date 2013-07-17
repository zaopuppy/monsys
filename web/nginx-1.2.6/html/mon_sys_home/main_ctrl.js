

var DEV_TYPE_SMART_LIGHT = 1;
var DEV_LIST_REFRESH_INTERVAL = 10 * 1000;

function MainCtrl() {
  // "undefined"
  this.xml_http = null;

  this.init = function() {
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

	this.update_list_box = function(dev_info_list) {
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

		var tbl = $('<table id="device-table"></table>');
		for (var i = 0; i < devs.length; ++i) {
			var row = $('<tr class="device-record"></tr>');

			// Image, name, desc, state, operate
			var imageCell = $('<td></td>');
			var nameCell = $('<td></td>');
			var descCell = $('<td></td>');
			var operaCell = $('<td></td>');

			// image
			// var img = document.createElement("img");
			var img = this.get_icon_by_type(devs[i]["type"]);
			// img.src = "Safari_Agua.png";
			// can't use img's width, for async loading...
			imageCell.width(48);
			imageCell.append(img);
			// name
			nameCell.text(devs[i]["name"]);
			nameCell.width(80);
			// desc
			descCell.text(devs[i]["desc"]);
			descCell.width(400);
			// opera
			var btn = $('<input type="button" value="opera"></input>');
			btn.click(function(addr, type) {
				return function() {
					console.log("operation on device: addr=" + addr + ", type=" + type);
					do_opera_device(addr, type);
				}
			}(devs[i]["addr"], devs[i]["type"]));
			operaCell.append(btn);

			// append all elements
			row.append(imageCell);
			row.append(nameCell);
			row.append(descCell);
			row.append(operaCell);
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

