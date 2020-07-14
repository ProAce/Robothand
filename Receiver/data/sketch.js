const socket = new WebSocket("ws://" + location.hostname + ":8080/");

var thumb_slider = document.getElementById("thumb");
var index_finger_slider = document.getElementById("index_finger");
var middle_finger_slider = document.getElementById("middle_finger");
var ring_finger_slider = document.getElementById("ring_finger");
var pinky_slider = document.getElementById("pinky");

var thumb_value = document.getElementById("thumb_value");
var index_finger_value = document.getElementById("index_finger_value");
var middle_finger_value = document.getElementById("middle_finger_value");
var ring_finger_value = document.getElementById("ring_finger_value");
var pinky_value = document.getElementById("pinky_value");

thumb_slider.oninput = function () {
  update_fingers();
  thumb_value.innerHTML = this.value;
};

index_finger_slider.oninput = function () {
  update_fingers();
  index_finger_value.innerHTML = this.value;
};

middle_finger_slider.oninput = function () {
  update_fingers();
  middle_finger_value.innerHTML = this.value;
};

ring_finger_slider.oninput = function () {
  update_fingers();
  ring_finger_value.innerHTML = this.value;
};

pinky_slider.oninput = function () {
  update_fingers();
  pinky_value.innerHTML = this.value;
};

function update_fingers() {
  var data = [
    "f",
    thumb_slider.value,
    index_finger_slider.value,
    middle_finger_slider.value,
    ring_finger_slider.value,
    pinky_slider.value,
  ];

  console.log(data);
  socket.send(data);
}

socket.addEventListener("open", function (event) {
  socket.send("i"); // Request initial settings on the opening off the websocket
});

socket.addEventListener("message", function (event) {
  console.log("Message from server ", event.data);

  var splitData = event.data.split(",");

  switch (
    splitData[0] // Parse the incoming messages
  ) {
    case "f":
      thumb_slider.value = splitData[1];
      thumb_value.innerHTML = splitData[1];

      index_finger_slider.value = splitData[2];
      index_finger_value.innerHTML = splitData[2];

      middle_finger_slider.value = splitData[3];
      middle_finger_value.innerHTML = splitData[3];

      ring_finger_slider.value = splitData[4];
      ring_finger_value.innerHTML = splitData[4];

      pinky_slider.value = splitData[5];
      pinky_value.innerHTML = splitData[5];
      break;
    case "m":
      selector.selectedIndex = splitData[1];
      break;
  }
});

window.onbeforeunload = function () {
  socket.close();
  window.location.reload(true);
};
