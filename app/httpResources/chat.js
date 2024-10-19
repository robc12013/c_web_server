let mainNavToggle = document.querySelector(".mainNavTopBar").lastElementChild;

let mainNav = document.querySelector(".mainNav")

let sendInputButton = document.querySelector(".sendInput");

let messageInput = document.querySelector(".messageInput");

let chatMessagePane = document.querySelector(".chatMessages");

let chatInputForm = document.querySelector(".chatInput form");

chatInputForm.addEventListener("submit",sendMessage);


sendInputButton.addEventListener("click",sendMessage);

mainNavToggle.addEventListener("click",toggleNav);


// SSE

// const testEventSource = new EventSource("receiveMessage");

// testEventSource.onmessage = (event) => {
//   console.log("receive message!");
// };

// WebSocket

const chatSocket = new WebSocket("ws://172.26.6.86:8081/socket");
console.log(chatSocket.readyState);

let navSelects = document.querySelectorAll(".mainNavTopBar > nav");
for (let index = 0; index < navSelects.length - 1; index++) {
  navSelects[index].addEventListener("click",selectNav);
}


function toggleNav() {
  if (!mainNav.classList.contains("mainNavClosed")) {
    mainNav.classList.add("mainNavClosed");
    mainNavToggle.classList.add("mainNavToggleOpen");
  } else {
    mainNav.classList.remove("mainNavClosed");
    mainNavToggle.classList.remove("mainNavToggleOpen");
  }
}


function selectNav(e) {
  let navItems = document.querySelectorAll(".mainNav > ul");
  for (let index = 0; index < navItems.length; index++) {
    navItems[index].setAttribute("hidden", true);
  }
  switch (this.className) {
    case "chatSelect":
      document.querySelector(".chatNav").removeAttribute("hidden");
      break;
    case "settingsSelect":
      document.querySelector(".settingsNav").removeAttribute("hidden");
      break;
    default:
      console.log(e);
  }
}


function sendMessage(e) {
  e.preventDefault();

  let connection = new XMLHttpRequest();

  console.log(messageInput.value);

  chatMessagePane.appendChild(document.createElement("div"));

  let newMessageContainer = chatMessagePane.lastChild;

  newMessageContainer.className = "outgoingMessage";
  newMessageContainer.innerHTML = messageInput.value;

  connection.open("POST","http://172.26.6.86:8081/sendMessage");
  connection.send(`user:ballsmoi,message:${messageInput.value}`);

  messageInput.value = "";
}


function receiveMessage(message) {
  chatMessagePane.appendChild(document.createElement("div"));
  let newMessageContainer = chatMessagePane.lastChild;
  newMessageContainer.className = "incomingMessage";
  newMessageContainer.innerHTML = message;

}
