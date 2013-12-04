
// init();
// get_panel();
function FGWController () {
  this.panel_ = null;
  this.fgw_ = null;

  this.init = function (fgw) {
    console.log("FGWController.init()");
    this.fgw_ = fgw;
    this.get_panel();

    g_monsys_api.get_dev_list(
      this.fgw_,
      $.proxy(this.callback_success, this),
      $.proxy(this.callback_error, this)
    );
  };

  this.get_panel = function () {
    console.log("FGWController.get_panel()");
    if (this.panel_ == null) {
      console.log("create new FGW panel");
      this.panel_ = $('<div id="fgw_controller"></div>');
    }

    return this.panel_;
  };

  this.on_get_dev_list_rsp = function (msg) {
    console.log("FGWController.on_get_dev_list_rsp()");

    if (!msg.hasOwnProperty("devs")) {
      console.log("field 'fgws' doesn't exsit");
      // TODO: show error
      return;
    }

    this.panel_.empty();

    // --- for debugging only ---
    msg = {
      "cmd": "get-dev-list-rsp",
      "devs": [
	{"type": 0, "addr": 0, "name": "dev-00", "state": 0},
	{"type": 1, "addr": 1, "name": "dev-01", "state": 1},
	{"type": 2, "addr": 2, "name": "dev-02", "state": 2},
	{"type": 3, "addr": 3, "name": "dev-03", "state": 3},
	{"type": 4, "addr": 4, "name": "dev-04", "state": 4}],
      "status": 0
    };
    // --- for debugging only ---

    var devs = msg["devs"];
    if (devs != null) {
      var ul = $('<ul style="list-style: none;"></ul>');
      for (var i = 0; i < devs.length; ++i) {
        var li = $('<li></li>');
        var btn = $('<input type="button" style="font-size: 1.5em; width: 90%;"></input>');
        btn.val(devs[i]["name"]);
        btn.click((function (fgw, dev) {
	  return function () {
	    show_dev_panel(fgw, dev);
	  };
	})(this.fgw_, devs[i]));
        li.append(btn);
        ul.append(li);
      }

      this.panel_.append(ul);
    }
  };

  // Function( PlainObject data, String textStatus, jqXHR jqXHR )
  this.callback_success = function (data, status, jq_xhr) {
    console.log("FGWController.callback_success");
    console.log("data: [" + data + "]");
    console.log("status: [" + status + "]");
    console.log("timeout: [" + this.timeout + "]");

    // decode it first
    var jmsg = null;
    try {
      jmsg = eval("(" + data + ")");
    } catch (e) {
      console.log("bad json");
    }

    if (jmsg == null || !jmsg.hasOwnProperty("cmd")) {
      // TODO: show error
      console.log("bad message");
      return;
    }

    var cmd = jmsg["cmd"];
    if (cmd == "get-dev-list-rsp") {
      this.on_get_dev_list_rsp(jmsg);
    } else {
      console.log("Unknown message: [" + cmd + "]");
    }
  };

  // Function( jqXHR jqXHR, String textStatus, String errorThrown )
  this.callback_error = function (jq_xhr, status, err) {
    console.log("FGWController.callback_error");
    console.log("status: [" + status + "]");
    console.log("error: [" + err + "]");
    console.log("timeout: [" + this.timeout + "]");
  };
}

