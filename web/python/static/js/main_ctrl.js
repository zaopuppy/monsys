
// init();
// // show();
// // hide();
// get_panel();
function MainController () {
  this.panel = null;

  this.init = function () {
    console.log("MainController.init()");
    this.get_panel();
    g_monsys_api.get_fgw_list(
      'ztest@gmail.com',
      $.proxy(this.callback_success, this),
      $.proxy(this.callback_error, this));
  };

  this.get_panel = function () {
    console.log("MainController.get_panel()");
    if (this.panel == null) {
      console.log("create new main panel");
      this.panel = $('<div id="main_controller"></div>');
    }
    return this.panel;
  };

  this.on_get_fgw_list_rsp = function (msg) {
    console.log("MainController.on_get_fgw_list_rsp()");

    if (!msg.hasOwnProperty("fgws")) {
      console.log("field 'fgws' doesn't exsit");
      // TODO: show error
      return;
    }

    this.panel.empty();

    var fgws = msg["fgws"];
    if (fgws != null) {
      var ul = $('<ul style="list-style: none;"></ul>');
      for (var i = 0; i < fgws.length; ++i) {
        var li = $('<li></li>');
        var btn = $('<input type="button" style="font-size: 1.5em; width: 90%; padding-bottom: 20px;"></input>');
        btn.val(fgws[i]);
        btn.click((function (fgw) {
	  return function () {
	    show_fgw_panel(fgw);
	  };
	})(fgws[i]));
        li.append(btn);
        ul.append(li);
      }

      this.panel.append(ul);
    }
  };

  // Function( PlainObject data, String textStatus, jqXHR jqXHR )
  this.callback_success = function (data, status, jq_xhr) {
    console.log("MainController.callback_success");
    console.log("data: [" + data + "]");
    console.log("status: [" + status + "]");
    console.log("timeout: [" + this.timeout + "]");
    // jq_xhr.responseText
    // jq_xhr.responseXML
    // jq_xhr.getResponseHeader()

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
    if (cmd == "get-fgw-list-rsp") {
      this.on_get_fgw_list_rsp(jmsg);
    } else {
      console.log("Unknown message: [" + cmd + "]");
    }
  };

  // Function( jqXHR jqXHR, String textStatus, String errorThrown )
  this.callback_error = function (jq_xhr, status, err) {
    console.log("MainController.callback_error");
    console.log("status: [" + status + "]");
    console.log("error: [" + err + "]");
    console.log("timeout: [" + this.timeout + "]");
  };
}

