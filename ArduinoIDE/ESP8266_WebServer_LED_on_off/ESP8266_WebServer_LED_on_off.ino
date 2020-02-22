// Wymagane biblioteki
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Ustawienie identyfikatora i hasła do sieci WIFI
const char* ssid = "NodeMCU";
const char* password = "esp8266mod";

// Inicjalizacja serwera WWW na porcie 80
// Instancja serwera nazywa się server
ESP8266WebServer server(80);

// do pin2 na nodeMCU podłączona jest wbudowana dioda LED
#define led 2

int stanLED = LOW;

// Funkcja wyświetlająca stronę powitalną
void handleRoot() {
  String message = "Czesc jestem serwerem sterujacym oswieteniem - NodeMCU ESP8266\n\n";
  message += "Wywolaj strone z parametrami:\n";
  message += "/LEDon  - zapalenie diody\n";
  message += "/LEDoff  - zgaszene diody\n";  
  message += "/LEDblink  - miganie dioda\n";
  message += "/LED  - panel z przyciskami\n";  
  server.send(200, "text/plain", message);
}

// Funkcja wyświetlana w przypadku nieznanej strony
void handleNotFound(){
  String message = "404 Strona nie znaleziona\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";  
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

// 
void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  Serial.begin(115200);
  // Inicjalizacja sieci WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Czekaj na polaczenie do WIFI. Co 500ms jest wyświetlana kropka
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Podlaczony do sieci o SIID:  ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("mDNS serwis wystartowal");
  }

  // Obsługa strony głównej
  server.on("/", handleRoot);

  // Obsługa podstrony /LEDon
  server.on("/LEDon", [](){
    server.send(200, "text/plain", "Zapalilem diode LED");
    digitalWrite(led, LOW);
    stanLED = HIGH; 
  });
  
  // Obsługa podstrony /LEDoff 
  server.on("/LEDoff", [](){
    server.send(200, "text/plain", "Zgasilem diode LED");
    digitalWrite(led, HIGH);
    stanLED = LOW;    
  });
  
  // Obsługa podstrony /LEDblink
  server.on("/LEDblink", [](){
    server.send(200, "text/plain", "Migam dioda LED");
    stanLED = HIGH;
    for(int i=0; i<=20; i++){
      digitalWrite(led, LOW);
      delay(100);    
      digitalWrite(led, HIGH);
      delay(100);        
    }
    stanLED = LOW;
  });
  
  // Obsługa podstrony /LED 
  server.on("/LED", [](){
  String message = "<!DOCTYPE HTML>";
  message += "<html>";
  message += "Stan diody LED: ";
  if(stanLED == HIGH) {
    message += "ON";
  } else {
    message += "OFF";
  }
  message += "<br><br>";
  message += "<a href=\"/LED=on\"\"><button>LED ON </button></a>";
  message += "<a href=\"/LED=off\"\"><button>LED OFF </button></a>";
  message += "<a href=\"/LED=blink\"\"><button>LED Blink </button></a><br />";
  message += "</html>";  
  server.send(200, "text/html", message);
  });
  
  // Obsługa podstrony /LED=on w trybie redirect (303) z przekierowaniem na podstronę /LED
  server.on("/LED=on", [](){
    digitalWrite(led, LOW);
    stanLED = HIGH;
    server.sendHeader("Location","/LED");
    server.send(303); 
  });
  
  // Obsługa podstrony /LED=off w trybie redirect (303) z przekierowaniem na podstronę /LED
  server.on("/LED=off", [](){
    digitalWrite(led, HIGH);
    stanLED = LOW;
    server.sendHeader("Location","/LED");
    server.send(303); 
  });
  
  // Obsługa podstrony /LED=blink w trybie redirect (303) z przekierowaniem na podstronę /LED
  server.on("/LED=blink", [](){
    stanLED = HIGH;
    for(int i=0; i<=6; i++){
      digitalWrite(led, LOW);
      delay(100);    
      digitalWrite(led, HIGH);
      delay(100);        
    }
    stanLED = LOW;
    server.sendHeader("Location","/LED");
    server.send(303); 
  });

  // Obsługa nieznanej strony
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server wystartowal");
}

void loop(void){
  server.handleClient();
}
