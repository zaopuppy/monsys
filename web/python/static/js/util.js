
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
