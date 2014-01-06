

function MonsysApi() {
  this.timeout = 3*1000;

  // Function( PlainObject data, String textStatus, jqXHR jqXHR )
  this.callback_success = function (data, status, jq_xhr) {
    console.log("MonsysApi.callback_success");
    console.log("data: [" + data + "]");
    console.log("status: [" + status + "]");
    console.log("timeout: [" + this.timeout + "]");
    // jq_xhr.responseText
    // jq_xhr.responseXML
    // jq_xhr.getResponseHeader()
  };

  // Function( jqXHR jqXHR, String textStatus, String errorThrown )
  this.callback_error = function (jq_xhr, status, err) {
    console.log("MonsysApi.callback_error");
    console.log("status: [" + status + "]");
    console.log("error: [" + err + "]");
    console.log("timeout: [" + this.timeout + "]");
  };

  this.init = function () {
    console.log("MonsysApi.init()");
  };

  this.get_fgw_list = function (account, cb_success, cb_error) {
    console.log("MonsysApi.get_fgw_list()");
    var jreq = {
      "cmd": "get-fgw-list",
      "uid": "11",
      "account": account,
    };
    this.send_json(jreq, cb_success, cb_error);
  };

  // this.gen_get_fgw_list_req = function (account) {
  //   var jreq = {
  //     "cmd": "get-fgw-list",
  //     "uid": "11",
  //     "account": account,
  //   };
  //   return jreq;
  // };
  
  this.get_dev_list = function (fgw, cb_success, cb_error) {
    console.log("MonsysApi.get_dev_list()");
    var jreq = {
      "cmd": "get-dev-list",
      "uid": "22",
      "fgw": fgw,
    };

    this.send_json(jreq, cb_success, cb_error);
  };

  this.get_dev_info = function (fgw, addr, id_list, cb_success, cb_error) {
    console.log("MonsysApi.get_dev_info()");
    var jreq = {
      "cmd": "get-dev-info",
      "uid": "33",
      "fgw": fgw,
      "addr": addr,
      "id-list": id_list,
    };

    this.send_json(jreq, cb_success, cb_error);
  };

  this.set_dev_info = function (fgw, addr, id, val) {
    console.log("MonsysApi.set_dev_info()");
    var jreq = {
      "cmd": "set-dev-info",
      "uid": "44",
      "fgw": fgw,
      "addr": addr,
      "vals": [ { "id": id, "val": val } ]
    };

    this.send_json(jreq);
  };

  this.set_dev_info_batch = function (fgw, addr, kv_list) {
    console.log("MonsysApi.set_dev_info()");
    var jreq = {
      "cmd": "set-dev-info",
      "uid": "44",
      "fgw": fgw,
      "addr": addr,
      "vals": kv_list
    };

    this.send_json(jreq);
  };

  this.send_json = function (jobj, cb_success, cb_error) {
    console.log("MonsysApi.send_json()");
    var jobj_str = JSON.stringify(jobj);
    this.send_raw(jobj_str, cb_success, cb_error);
  };

  this.send_raw = function (raw_text, cb_success, cb_error) {
    console.log("MonsysApi.send_raw()");
    console.log("[" + raw_text + "]");
    $.ajax({
      url: "/interface",
      data: raw_text,
      type: "POST",
      timeout: this.timeout,
      success: cb_success ? cb_success : this.callback_success,
      error: cb_error ? cb_error : this.callback_error,
    });
  };
}


