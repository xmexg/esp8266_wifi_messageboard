// ESP8266 WiFi Captive Portal
// By 125K (github.com/125K)

// Includes
#include <ESP8266WiFi.h>
#include <DNSServer.h> 
#include <ESP8266WebServer.h>

// User configuration
#define SSID_NAME "连我试试"
#define TITLE "壶内洞天"
#define POST_TITLE "保存留言"
#define POST_BODY "保存留言"
#define PASS_TITLE "显示留言"
#define CLEAR_TITLE "清理留言"

// Init System Settings
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1); // Gateway

String Credentials="";
unsigned long bootTime=0, lastActivity=0, lastTick=0, tickCtr=0;
DNSServer dnsServer; ESP8266WebServer webServer(80);

String input(String argName) {
  String a=webServer.arg(argName);
  a.replace("<","&lt;");a.replace(">","&gt;");
  a.substring(0,200);
  return a;
}

String footer() {
  String script = "<script>"
    "function updateCountdown() {"
    "var targetDate = new Date(\"2024-03-30T00:00:00Z\");"
    "var currentDate = new Date();"
    "var timeRemaining = targetDate - currentDate;"
    "var days = Math.floor(timeRemaining / (1000 * 60 * 60 * 24));"
    "var hours = Math.floor((timeRemaining % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));"
    "var minutes = Math.floor((timeRemaining % (1000 * 60 * 60)) / (1000 * 60));"
    "var seconds = Math.floor((timeRemaining % (1000 * 60)) / 1000);"
    "document.getElementById(\"countdown\").innerHTML = days + \"天 \" + hours + \"小时 \" + minutes + \"分钟 \" + seconds + \"秒 \";"
    "}"
    "setInterval(updateCountdown, 1000);"
    "</script>";
   return "<div class=q><a>&#169; 滚回去学习!</a></div>" + script + "</body></html>";
}

String header(String t) {
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }" 
    "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
    "div { padding: 0.5em; }"
    "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
    "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; }"
    "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
    "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
    "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
    "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html>"
    "<head><meta charset=UTF-8><title>" + t + "</title>"
    "<style>"+CSS+"</style></head>"
    "<body><nav>你还有多少时间? <b><div id='countdown'></div></b>而你却在这里玩! </nav>";
  return h; 
}

String creds() {
  return header(PASS_TITLE) + "<ol>" + Credentials + "</ol><br><center><p><a style='color: blue' href=/>前往首页</a></p></center>" + footer();
}

String index() {
  return header(TITLE) + "<div>你可在此时写下留言</div><div><form action=/post method=post>" +
    "<b>昵称:</b> <center><input type=text name=username maxlength=10></input></center>" +
    "<b>留言:</b> <center><textarea type=text name=usertext style='height: 6em;'></textarea><input type=submit value='提交'></form></center></div>" + footer();
}

String posted() {
  String username = input("username");
  String usertext = input("usertext");
  Credentials="<li>昵称: <b>" + username + "</b></br>留言: <b>" + usertext + "</b></li>" + Credentials;
  return header(POST_TITLE) + "<div style='font-size: 1.4em'>保存成功,<a href='/creds'>前往此处查看留言</a></div>" + footer();
}

String clear() {
  String username="<p></p>";
  String usertext="<p></p>";
  Credentials="<p></p>";
  return header(CLEAR_TITLE) + "<div><p>已清除所有留言</div></p><center><a style=\"color:blue\" href=/>点击此处前往首页</a></center>" + footer();
}

void BLINK() { // The internal LED will blink 5 times when a password is received.
  int count = 0;
  while(count < 2){
    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(500);
    count = count + 1;
  }
}

void setup() {
  bootTime = lastActivity = millis();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  dnsServer.start(DNS_PORT, "*", APIP); // DNS spoofing (Only HTTP)
  webServer.on("/post",[]() { webServer.send(HTTP_CODE, "text/html", posted()); BLINK(); });
  webServer.on("/creds",[]() { webServer.send(HTTP_CODE, "text/html", creds()); });
  webServer.on("/clear",[]() { webServer.send(HTTP_CODE, "text/html", clear()); });
  webServer.onNotFound([]() { lastActivity=millis(); webServer.send(HTTP_CODE, "text/html", index()); });
  webServer.begin();
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}


void loop() { 
  if ((millis()-lastTick)>TICK_TIMER) {lastTick=millis();} 
  dnsServer.processNextRequest();
  webServer.handleClient(); 
}
