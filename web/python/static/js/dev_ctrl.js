
// init();
// get_panel();
function DevController () {
  // this.panel_ = null;
  this.fgw_ = null;
  this.dev_ = null;
  this.dev_module_ = null;

  this.init = function (fgw, dev) {
    console.log("DevController.init()");
    this.fgw_ = fgw;
    this.dev_ = dev;
    // this.panel_ = null;
    // TODO:
    this.dev_module_ = null;
    this.get_panel();

    // g_monsys_api.get_dev_info(
    //   this.fgw_,
    //   this.dev_["addr"],
    //   [0],			// all info
    //   $.proxy(this.callback_success, this),
    //   $.proxy(this.callback_error, this)
    // );
  };

  this.get_panel = function () {
    console.log("DevController.get_panel()");

    // if (this.panel_ != null) {
    //   return this.panel_;
    // }

    this.get_dev_module();

    // this.panel_ = $('<div id="dev_controller"></div>');

    return this.dev_module_.get_panel();
  };

  this.get_dev_module = function () {
    if (this.dev_module_ != null) {
      return this.dev_module_;
    }

    var type = this.dev_["type"];

    this.dev_module_ = new DevCommon();
    this.dev_module_.init(this.fgw_, this.dev_);

    return this.dev_module_;
  }

  this.on_get_dev_info_rsp = function (msg) {
    console.log("DevController.on_get_dev_info_rsp()");

  };

  this.on_set_dev_info_rsp = function (msg) {
    console.log("DevController.on_set_dev_info_rsp()");

  };

  // Function( PlainObject data, String textStatus, jqXHR jqXHR )
  this.callback_success = function (data, status, jq_xhr) {
    console.log("DevController.callback_success");
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
    if (cmd == "get-dev-info-rsp") {
      this.on_get_dev_list_rsp(jmsg);
    } else if (cmd == "set-dev-info-rsp") {
      this.on_set_dev_list_rsp(jmsg);
    } else {
      console.log("Unknown message: [" + cmd + "]");
    }
  };

  // Function( jqXHR jqXHR, String textStatus, String errorThrown )
  this.callback_error = function (jq_xhr, status, err) {
    console.log("DevController.callback_error");
    console.log("status: [" + status + "]");
    console.log("error: [" + err + "]");
    console.log("timeout: [" + this.timeout + "]");
  };
}

