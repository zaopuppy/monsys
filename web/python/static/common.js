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

  do {
      totalOffsetX += currentElement.offsetLeft - currentElement.scrollLeft;
      totalOffsetY += currentElement.offsetTop - currentElement.scrollTop;
  } while(currentElement = currentElement.offsetParent)

  canvasX = event.pageX - totalOffsetX;
  canvasY = event.pageY - totalOffsetY;

  return {x:canvasX, y:canvasY}
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

  this.init = function(state_change_callback) {
    if (window.XMLHttpRequest) {
      this.xml_http = new XMLHttpRequest();
    } else {
      this.xml_http = new ActiveXObject("Microsoft.XMLHTTP");
    }
    this.xml_http.onreadystatechange = state_change_callback;
  }

  this.send_json = function (jobj) {
    var jobj_str = JSON.stringify(jobj);
    this.send_raw(jobj_str);
  }

  this.send_raw = function(raw_text) {
    console.log("sending:");
    console.log("[" + raw_text + "]");
    this.xml_http.open("POST", "/interface", true);
    this.xml_http.send(raw_text);
  };

  // this.get_dev_info = function(uid, dev_id, addr) {
  //   var jreq = {
  //     "cmd": "get-dev-info",
  //     "uid": uid,
  //     "dev-id": dev_id,
  //     "addr": addr
  //   };
  //   send_json(jreq);
  //   // this.xml_http.open("POST", "monsys", true);
  //   // this.xml_http.send_json(jreq);
  // };

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


