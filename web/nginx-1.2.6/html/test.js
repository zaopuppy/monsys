
var g_ctx = null;
var g_width = 800;
var g_height = 800;
var g_block_size = 30;
var g_area_row = 20;
var g_area_col = 10;
var g_area_pos = [100, 20]; // x, y
var g_area = null;

g_shape_type = 0; // 0 ~ 6

g_shape_T = [
    // 0
    [[1, 1, 1],
     [0, 1, 0]],
    // 1
    [[0, 1],
     [1, 1],
     [0, 1]],
    // 2
    [[0, 1, 0],
     [1, 1, 1]],
    // 3
    [[1, 0],
     [1, 1],
     [1, 0]]
];

g_shape_rot = 0;
g_shape_pos = [g_area_row, g_area_col/2 - 2]; // row, col

g_shapes = [
  g_shape_T,
];

g_keys = [];

(function() {
  console.log("init");

  window.onload = function() {
    init();
  }

}());

function init() {
  // data struct
  g_area = new Array(g_area_row);
  for (var i = g_area.length - 1; i >= 0; i--) {
    g_area[i] = new Array(g_area_col);
    for (var j = g_area[i].length - 1; j >= 0; j--) {
      g_area[i][j] = 0;
    }
  }

  g_area[15][5] = 1;

  // initialize canvas
  var canvas = document.getElementById("main-canvas");
  canvas.width = g_width;
  canvas.height = g_height;

  g_ctx = canvas.getContext("2d");

  draw();
  setInterval(step, 500);

  // keyboard event
  window.addEventListener("keydown", on_keydown);
  window.addEventListener("keyup", on_keyup);
}

function on_keydown(event) {
  // console.log("keydown: " + event.keyCode);
  var keycode = event.keyCode;
  if (!g_keys[keycode]) {
    if (keycode == 'S'.charCodeAt(0)) {
      if (!check_collide(g_shape_pos[0], g_shape_pos[1] - 1, g_shape_rot)) {
        --g_shape_pos[1];
        draw();
      }
    };
    if (keycode == 'F'.charCodeAt(0)) {
      if (!check_collide(g_shape_pos[0], g_shape_pos[1] + 1, g_shape_rot)) {
        ++g_shape_pos[1];
        draw();
      }
    };
    if (keycode == 'E'.charCodeAt(0)) {
      var new_rot = g_shape_rot + 1;
      if (new_rot >= 4) {
        new_rot = 0;
      }
      if (!check_collide(g_shape_pos[0], g_shape_pos[1], new_rot)) {
        g_shape_rot = new_rot;
      }
    }
  }
  g_keys[keycode] = true;
}

function on_keyup(event) {
  // console.log("keyup: " + event.keyCode);
  g_keys[event.keyCode] = false;
}

function new_block() {
  g_shape_type = Math.floor(Math.random() * 7);
  g_shape_rot = Math.floor(Math.random() * 4);
}

function step() {
  if (check_collide(g_shape_pos[0] - 1, g_shape_pos[1], g_shape_rot)) {
    // TODO: merge to area
    return
  };

  --g_shape_pos[0];
  draw();
}

function check_collide(r, c, rot) {
  var shape = g_shapes[g_shape_type][rot];
  var area = g_area;

  if (r < 0 /*|| (r + shape.length) >= g_area_row*/
    || c < 0 || (c + shape[0].length) > g_area_col) {
    return true;
  }

  for (var i = shape.length - 1; i >= 0; i--) {
    for (var j = shape[i].length - 1; j >= 0; j--) {
      if (shape[i][j] != 0) {
        var new_r = i + r;
        var new_c = j + c;
        if (new_r >= 0 && new_r < g_area_row
          && new_c >= 0 && new_c < g_area_col
          && g_area[new_r][new_c] != 0) {
          return true;
        }
      }
    }
  }

  return false;
}

function draw() {
  if (g_ctx == null) { return; }

  // clear first
  g_ctx.clearRect(0, 0, g_width, g_height);

  g_ctx.fillStyle = "rgba(200, 0, 0, 0.5)";
  // g_ctx.fillRect(0, 0, 100, 100);

  // TODO: use image instead of drawing
  for (var r = g_area.length - 1; r >= 0; r--) {
    for (var c = g_area[r].length - 1; c >= 0; c--) {
      draw_block(r, c, g_area[r][c]);
    };
  };

  // block
  var block = g_shapes[g_shape_type][g_shape_rot];
  var block_row = block.length;
  for (var r = block.length - 1; r >= 0; r--) {
    for (var c = block[r].length - 1; c >= 0; c--) {
      if (block[r][c] != 0) {
        draw_block(
          g_shape_pos[0] + r,
          g_shape_pos[1] + c,
          block[r][c]);
      }
    };
  };
}

function draw_block(r, c, type) {
  if (type == 0) {
    g_ctx.strokeRect(
      c * g_block_size + g_area_pos[0],
      g_height - (r * g_block_size + g_area_pos[1]),
      g_block_size, -g_block_size); // minus width for coordination transfer
  } else {
    g_ctx.fillRect(
      c * g_block_size + g_area_pos[0],
      g_height - (r * g_block_size + g_area_pos[1]),
      g_block_size, -g_block_size);
  }
}

