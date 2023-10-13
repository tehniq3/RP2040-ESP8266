/***********
original: Juan A. Villalpando, KIO4.COM - 25/11/22
https://community.appinventor.mit.edu/t/examples-with-the-esp8266-01-wifi-led-on-off-arduino-standalone-mqtt/70751/4
adapted for Raspberry Pi (RP2040) + ESP8266-01 by Nicu FLORICA (niq_ro)
***********/

#define DEBUG true
#define serialCommunicationSpeed 115200
#define led 25  // onboard led

void setup()
{
pinMode(led,OUTPUT);  // internal led
Serial.begin(serialCommunicationSpeed);
Serial.println("-");
Serial1.begin(serialCommunicationSpeed); // Importante la velocidad del módulo.

sendData("AT+RST\r\n",2000,DEBUG); // Borra la configuración que tenía el módulo
sendData("AT+CWJAP=\"Nombre_de_tu_WiFi\",\"Clave_de_tu_WiFi\"\r\n", 2000, DEBUG);
delay(1500); // Espera un poco que conecte con el Router.
sendData("AT+CWMODE=1\r\n",1500,DEBUG); // Modo de cliente y servidor.
sendData("AT+CIFSR\r\n",1500,DEBUG); // En el Serial Monitor aparece la IP de cliente y servidor.
sendData("AT+CIPMUX=1\r\n",1500,DEBUG); // Multiples conexiones.
sendData("AT+CIPSERVER=1,80\r\n",1500,DEBUG); // El Puerto web es el 80
}
   
void loop(){
if(Serial1.available()) // Consulta si el módulo está enviando algún mensaje
{
if(Serial1.find("+IPD,"))
{
delay(500);

int connectionId = Serial1.read()-48; 
// Aquí las construcción de la PAGINA WEB.
String webpage = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n\r\n\r\n";
webpage += "<h1>arduinotehniq.com</h1>";
webpage += "<form method=\"get\" action=\"/enci\">";
webpage += "<button type=\"submit\">ON - APRINDE</button></form>";
webpage += "<form method=\"get\" action=\"/apag\">";
webpage += "<button type=\"submit\">OFF - STINGE</button></form>\r\n\r\n";

String cipSend = "AT+CIPSEND=";
cipSend += connectionId;
cipSend += ",";
cipSend +=webpage.length();
cipSend +="\r\n";

sendData(cipSend,500,DEBUG);
sendData(webpage,500,DEBUG);

// Lee el pin 13
int pin13 = digitalRead(led); 
// Retorno de la lectura.
String retorno = "HTTP/1.1 200 OK\r\n Content-Type: text/html\r\n\r\n\r\n";
if (pin13 == 1) {retorno += "<br> ON - Aprins";}
if (pin13 == 0) {retorno += "<br> OFF - Stins";}

// ResponseCode App Inventor
cipSend = "AT+CIPSEND=";
cipSend += connectionId;
cipSend += ",";
cipSend += retorno.length();
cipSend +="\r\n";

sendData(cipSend,500,DEBUG);
sendData(retorno,500,DEBUG);

// Cierra la conexión
String closeCommand = "AT+CIPCLOSE="; 
closeCommand+=connectionId;
closeCommand+="\r\n";

sendData(closeCommand,500,DEBUG);
}
}
}

// Función para Enviar datos al Servidor.
String sendData(String command, const int timeout, boolean debug){
String response = "";
Serial1.print(command); // Envía la información de command al servidor
long int time = millis();

while( (time+timeout) > millis())
{
while(Serial1.available())
{
// A response van los datos que regresan al servidor.
char c = Serial1.read(); // Va leyendo caracter a caracter.
response+=c;

// Consulta si en la información que regresa al servidor
// viene "GET /enci" o "GET /apag"
// Encenderá o apagará el LED13 del Arduino
if(response.indexOf("GET /enci") >0){
// Serial.print("enciende");
digitalWrite(led,HIGH);
}
if(response.indexOf("GET /apag") >0){
//Serial.print("apaga");
digitalWrite(led,LOW);
}
} 
}

if(debug)
{
Serial.print(response);
} 
return response;
}
