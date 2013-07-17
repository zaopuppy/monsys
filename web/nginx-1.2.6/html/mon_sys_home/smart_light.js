
function SmartLightControl() {
	this.canvas_elem_ = null;
	this.canvas_ = null;
	this.img_ = new Image();

	this.addr_ = null;
	this.switch_ = null;
	this.light_level_ = null;

	this.mouse_down_ = false;
	this.current_mouse_pos_ = null;

	// init
	this.init = function() {
		// -------------------------
		// canvas
		this.canvas_elem_ = document.getElementById("light-color-picker");

		this.canvas_elem_.addEventListener("click", $.proxy(this.on_click, this), false);
		// mouse
		this.canvas_elem_.addEventListener("mousemove", $.proxy(this.on_mouse_move, this), false);
		this.canvas_elem_.addEventListener("mousedown", $.proxy(this.on_mouse_down, this), false);
		this.canvas_elem_.addEventListener("mouseup", $.proxy(this.on_mouse_up, this), false);
		// touch(for touch devices)
		this.canvas_elem_.addEventListener("touchmove", $.proxy(this.on_mouse_move, this), false);
		this.canvas_elem_.addEventListener("touchstart", $.proxy(this.on_mouse_down, this), false);
		this.canvas_elem_.addEventListener("touchend", $.proxy(this.on_mouse_up, this), false);
		// g_img.src = "ClownFish.png";
		this.img_.src = "Shapes.png";
		this.img_.onload = $.proxy(this.on_image_loaded, this);

		// setTimeout(on_draw, 100);
		// No, we can't invoke this method here, on_draw() will call setTimeout(), it will
		// make a closure so g_canvas in on_draw will never change
		// on_draw();
		// -------------------------
		// check box
		this.switch_ = document.getElementById("light-switch");
		this.switch_.addEventListener("click", $.proxy(this.on_switch_changed, this), false);
		// -------------------------
		// light level
		this.light_level_ = document.getElementById("light-bright-level");
		this.light_level_.addEventListener("mouseup", $.proxy(this.on_light_changed, this), false);
		this.light_level_.addEventListener("touchend", $.proxy(this.on_light_changed, this), false);
	}

	this.show = function() {
		console.log("SmartLightControl.show()");
		$("#light-control-panel").show();
	}

	this.hide = function() {
		console.log("SmartLightControl.hide()");
		$("#light-control-panel").hide();
	}
	this.on_click = function(event) {
		var pos = getRelativeCoords(event);
		// console.log("on_click: (" + pos.x + ", " + pos.y + ")");
	}
	this.on_mouse_move = function(event) {
		if (this.mouse_down_) {
			var pos = getRelativeCoords(event);
			this.current_mouse_pos_ = pos;
			// console.log("on_mouse_move: (" + pos.x + ", " + pos.y + ")");
		}
	}

	this.on_mouse_down = function(event) {
		// 0: left, 1: middle, 2: right
		if (event.button != 0) {
			return;
		}

		this.mouse_down_ = true;

		this.current_mouse_pos_ = getRelativeCoords(event);
	}

	this.on_mouse_up = function(event) {
		if (event.button != 0) {
			return;
		}

		console.log("on_mouse_up");

		this.mouse_down_ = false;

		// check pos
		var pos = this.current_mouse_pos_;
		if (pos == null) {
			console.log("pos == null");
			return;
		}

		// image
		var p = this.canvas_.getImageData(pos.x, pos.y, 1, 1).data;
		console.log("R: " + p[0] + ", G: " + p[1] + ", B: " + p[2]);

		// R, G, B
		g_monsys.set_dev_info(44, this.addr_, [11, 12, 13], [p[0], p[1], p[2]]);
	}

	this.on_image_loaded = function() {
		console.log("on_image_loaded()");
		if (this.canvas_elem_ != null) {
			this.canvas_elem_.width = this.img_.width;
			this.canvas_elem_.height = this.img_.height;
			this.canvas_ = this.canvas_elem_.getContext("2d");
			// --- for debugging ---
			this.canvas_.fillStyle = "#00FF00";
			this.canvas_.fillRect(20, 20, 20, 20);
			// --- for debugging ---
			this.on_draw();
		}
	}

	this.on_draw = function() {
		if (this.canvas_ == null) {
			console.log("this.canvas_ is not initialized yet.");
			return;
		}

		// console.log("on_draw()");
		this.canvas_.drawImage(this.img_, 0, 0);

		// this.canvas_.moveTo(5, 5);
		var pos = this.current_mouse_pos_;
		if (pos != null) {
			var p = this.canvas_.getImageData(pos.x, pos.y, 1, 1).data;
			// this.canvas_.fillStyle = "#FF0000";
			this.canvas_.fillStyle = "#" + ("000000" + rgbToHex(p[0], p[1], p[2])).slice(-6);
			this.canvas_.fillRect(5, 5, 50, 50);
		} else {
			// set color whick get from devices
			this.canvas_.fillStyle = "#FFFFFF";
			this.canvas_.fillRect(5, 5, 50, 50);
		}

		this.canvas_.strokeStyle = "#FFFFFF";
		this.canvas_.strokeRect(5, 5, 50, 50);

		setTimeout($.proxy(this.on_draw, this), 100);
	}

	this.on_light_changed = function() {
		console.log("light changed: " + this.light_level_.value);

		var v = parseInt(this.light_level_.value);
		g_monsys.set_dev_info(44, this.addr_, [11, 12, 13], [v, v, v]);
	}

	this.on_switch_changed = function() {
		console.log("switch changed: " + this.switch_.checked);

		if (this.switch_.checked) {
			g_monsys.set_dev_info(44, this.addr_, [4], [1]);
		} else {
			g_monsys.set_dev_info(44, this.addr_, [4], [0]);
		}
	}

}

