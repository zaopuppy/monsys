
var g_monsys_api = g_monsys_api || new MonsysApi();
var g_main_ctrl = g_main_ctrl || new MainController();
var g_main_ctrl_panel = null;
var g_fgw_ctrl = g_fgw_ctrl || new FGWController();
var g_fgw_ctrl_panel = null;
var g_dev_ctrl = g_dev_ctrl || new DevController();
var g_dev_ctrl_panel = null;

//////////////////////////////////////////////////////////////////////
// start
(function () {
  $(document).ready(function () {
    initialize();
  });
}());

function initialize() {
  console.log("initialize...")

  g_main_ctrl.init();

  show_main_panel();

  console.log("done")
}

function main_controller_panel() {
  return $('#main-controller-panel');
}

function get_main_controller_panel () {
  return g_main_ctrl.get_panel();
}

function get_fgw_controller_panel () {
  return g_fgw_ctrl.get_panel();
}

function get_dev_controller_panel () {
  return g_dev_ctrl.get_panel();
}

function show_fgw_panel (fgw) {
  console.log("show_fgw_panel(" + fgw + ")");

  g_fgw_ctrl.init(fgw);

  main_controller_panel().empty();

  var panel = get_fgw_controller_panel();
  main_controller_panel().append(panel);
}

function show_dev_panel (fgw, dev) {
  console.log("show_dev_panel(" + fgw + ", " + dev + ")");

  g_dev_ctrl.init(fgw, dev);

  main_controller_panel().empty();

  var panel = get_dev_controller_panel();
  main_controller_panel().append(panel);
}

function hide_all_panel () {
  // get_main_controller_panel().hide();
  // get_fgw_controller_panel().hide();
  // get_dev_controller_panel().hide();
}

function show_main_panel () {
  console.log("show_main_panel()");

  g_main_ctrl.init();

  main_controller_panel().empty();

  // create main control panel
  var ctrl_panel = get_main_controller_panel();
  main_controller_panel().append(ctrl_panel);
}

