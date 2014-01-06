

function DevCommon () {
  this.panel_ = null;
  this.fgw_ = null;
  this.dev_ = null;
  this.max_id_ = 50;
  this.slide_btns_ = null;
  this.value_boxes_ = null;

  this.init = function (fgw, dev) {
    console.log("DevCommon.init()");
    this.fgw_ = fgw;
    this.dev_ = dev;

    this.get_panel();

    g_monsys_api.get_dev_info(
      this.fgw_,
      this.dev_["addr"],
      [0],
      $.proxy(this.callback_success, this),
      $.proxy(this.callback_error, this)
    );
  };

  this.get_panel = function () {
    console.log("DevCommon.get_panel()");

    if (this.panel_ != null) {
      return this.panel_;
    }

    this.panel_ = $('<div id="common_dev_module"></div>');

    // FIXME: not good...
    this.slide_btns_ = [];
    this.value_boxes_ = [];

    var ul = $('<ul style="list-style: none;" ></ul>');
    for (var i = 1; i <= this.max_id_; ++i) {
      ul.append(this.gen_config_item(i));
    }

    this.panel_.append(ul);

    return this.panel_;
  };

  this.gen_config_item = function (id) {
    // console.log("DevCommon.gen_config_item(" + id + ")");
    var li = $('<li></li>');
    var span = $('<span></span>');
    var label = $('<label></label>');

    var label_text = $('<span></span>');
    label_text.text(id);
    var value_box = $('<span></span>');
    value_box.text(0);

    var slide_btn =
      $('<input type="range" step="1" style="width: 80%;" value="0"></input>');
    slide_btn.attr("disabled", true);
    slide_btn.attr("min", 0);
    slide_btn.attr("max", 255);

    // event handler
    var on_mouseup = (function (fgw, addr, id) {
      return function () {
	console.log("mouseup(" + fgw + ", " + addr + ", " + id + ")");
	value_box.text($(this).val());
	g_monsys_api.set_dev_info(fgw, addr, id, parseInt($(this).val()));
      };
    })(this.fgw_, this.dev_["addr"], id);

    slide_btn.bind({
      mouseup: on_mouseup,
      touchend: on_mouseup,
      change: function () {
	value_box.text($(this).val());
      },
    });

    label.append(label_text);
    label.append(slide_btn);
    label.append(value_box);

    // FIXME: not good...
    this.slide_btns_.push(slide_btn);
    this.value_boxes_.push(value_box);

    li.append(label);

    return li;
  };

  this.on_get_dev_info_rsp = function (msg) {
    console.log("DevCommon.on_get_dev_info_rsp()");

    if (!msg.hasOwnProperty("id-infos")) {
      console.log("field 'info' doesn't exist");
      return;
    }

    var info = msg["id-infos"];

    this.disable_all_config_item();

    for (var i = 0; i < info.length; ++i) {
      var id = parseInt(info[i]["id"]);
      var val = parseInt(info[i]["val"]);
      if (id <= 0 || id > this.max_id_) {
	// XXX: bibi!
	console.log("!!!! error handle needed");
	continue;
      }
      this.slide_btns_[id-1].attr("disabled", false);
      this.slide_btns_[id-1].val(val);
      this.value_boxes_[id-1].text(val);
    }

  }

  this.disable_all_config_item = function () {
    for (var i = 0; i < this.max_id_; ++i) {
      this.slide_btns_[i].attr("disabled", true);
    }
  }

  // Function( PlainObject data, String textStatus, jqXHR jqXHR )
  this.callback_success = function (data, status, jq_xhr) {
    console.log("DevCommon.callback_success");
    console.log("data: [" + data + "]");
    console.log("status: [" + status + "]");
    console.log("timeout: [" + this.timeout + "]");

    // // --- FOR DEBUGGING ONLY ---
    // this.on_get_dev_info_rsp({
    //   "info": [
    // 	{ "id": 4, "value": 67 },
    // 	{ "id": 3, "value": 23 }
    //   ]
    // });
    // // --- FOR DEBUGGING ONLY ---

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
      this.on_get_dev_info_rsp(jmsg);
    } else {
      console.log("Unknown message: [" + cmd + "]");
    }
  };

  // Function( jqXHR jqXHR, String textStatus, String errorThrown )
  this.callback_error = function (jq_xhr, status, err) {
    console.log("DevCommon.callback_error");
    console.log("status: [" + status + "]");
    console.log("error: [" + err + "]");
    console.log("timeout: [" + this.timeout + "]");
  };

}

