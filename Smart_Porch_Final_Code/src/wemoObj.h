#include "application.h"

TCPClient WemoClient;

class Wemo {
    int wemoPort = 49153;
    int _wemo;
    const char *wemoIP[5] = {"192.168.1.30","192.168.1.31","192.168.1.32","192.168.1.33","192.168.1.34"};

    public:
    Wemo(int wemo){
        _wemo = wemo;
    }
    void switchON() {
        String data1;
  
        Serial.printf("SwitchON %i \n",_wemo);
        data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
        if (WemoClient.connect(wemoIP[_wemo],wemoPort)) {
            WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
            WemoClient.println("Content-Type: text/xml; charset=utf-8");
            WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
            WemoClient.println("Connection: keep-alive");
            WemoClient.print("Content-Length: ");
            WemoClient.println(data1.length());
            WemoClient.println();
            WemoClient.print(data1);
            WemoClient.println();
        }

        if (WemoClient.connected()) {
            WemoClient.stop();
        }
    }

    void switchOFF(){
        String data1;
  
        Serial.printf("switchOFF %i \n",_wemo);
        data1+="<?xml version=\"1.0\" encoding=\"utf-8\"?><s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body><u:SetBinaryState xmlns:u=\"urn:Belkin:service:basicevent:1\"><BinaryState>0</BinaryState></u:SetBinaryState></s:Body></s:Envelope>"; // Use HTML encoding for comma's
        if (WemoClient.connect(wemoIP[_wemo],wemoPort)) {
            WemoClient.println("POST /upnp/control/basicevent1 HTTP/1.1");
            WemoClient.println("Content-Type: text/xml; charset=utf-8");
            WemoClient.println("SOAPACTION: \"urn:Belkin:service:basicevent:1#SetBinaryState\"");
            WemoClient.println("Connection: keep-alive");
            WemoClient.print("Content-Length: ");
            WemoClient.println(data1.length());
            WemoClient.println();
            WemoClient.print(data1);
            WemoClient.println();
        }
   
    if (WemoClient.connected()) {
        WemoClient.stop();
        }
    }
    void wemoWrite(bool wemoState) {
        if(wemoState) {
            switchON();
        }
        else {
            switchOFF();
        }
    }
};