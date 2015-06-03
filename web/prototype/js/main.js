
var SETTING = {
  break_time: 5,
  study_time: 5,
  // margin_left: 20,  // in px
  // margin_top: 20,   // in px
};

var g_counter = 1;
var g_first = true;
var g_noti_block = undefined;
var g_timer_text = undefined;

window.onload = function () {
  console.log("hi, I'm in content_script.js...");

  g_noti_block = get_or_create_noti_block();
  g_noti_block.style.visibility = "hidden";

  var body = document.getElementsByTagName("body")[0];
  body.appendChild(g_noti_block);

  setInterval(on_timer, 1000);
}

function on_timer () {
  ++g_counter;
  if (g_counter <= SETTING["break_time"]) {
    console.log("chi...");
    // return;
  } else if (g_counter <= (SETTING["break_time"] + SETTING["study_time"])) {
    g_timer_text.textContent = '' + (g_counter - SETTING["break_time"]);
    if (!g_first) {
      console.log("not first time");
      return;
    }

    g_noti_block.style.visibility = "visible";

    g_first = false;
  } else {
    g_counter = 1;
    g_first = true;
    g_noti_block.style.visibility = "hidden";
  }
}

function get_or_create_noti_block () {
  var my_id = "jieweibo-nonono";
  var noti_block = document.getElementById(my_id);
  if (noti_block) {
    console.log("node exists, ignore");
  } else {
    noti_block = create_container();
    noti_block.id = my_id;

    var title = document.createElement('h2');
    title.textContent = "STOP!";
    noti_block.appendChild(title);

    g_timer_text = document.createElement('p');
    g_timer_text.textContent = '' + (SETTING["study_time"]);
    noti_block.appendChild(g_timer_text);
  }
  return noti_block;
}

function create_container () {
  var e = document.createElement("div");
  // var width = document.body.clientWidth - (2*SETTING["margin_left"]);
  // var height = document.body.clientHeight - (2*SETTING["margin_top"]);
  e.style.width = '95%';
  e.style.height = '95%';
  e.style.marginLeft = "auto";
  e.style.marginRight = "auto";
  e.style.border = "1px solid red";
  e.style.position = "fixed";
  e.style.left = "20px";
  e.style.top = "20px";
  e.style.backgroundColor = "#FFFFFF";
  e.style.textAlign = "center";
  return e;
}


