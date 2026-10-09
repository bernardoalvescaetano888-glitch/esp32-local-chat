#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <deque>
#include "chat.h"

const char* ssid = "ESP32-Chat-Local";
const char* password = "";

WebServer server(80);
WebSocketsServer webSocket(81);

struct ClientInfo {
    uint8_t id;
    String name;
    bool isAdmin;
};

struct MessageInfo {
    String name;
    String text;
};

std::vector<ClientInfo> clients;
std::deque<MessageInfo> messageHistory; // Guarda o histórico de mensagens
bool chatFrozen = false;

void notifyUserList() {
    StaticJsonDocument<512> doc;
    doc["type"] = "users";
    JsonArray arr = doc.createNestedArray("users");
    for(auto& c : clients) {
        arr.add(c.name);
    }
    String output;
    serializeJson(doc, output);
    webSocket.broadcastTXT(output);
}

void sendHistory(uint8_t num) {
    StaticJsonDocument<1024> doc;
    doc["type"] = "history";
    JsonArray arr = doc.createNestedArray("history");
    for(auto& m : messageHistory) {
        JsonObject obj = arr.createNestedObject();
        obj["name"] = m.name;
        obj["text"] = m.text;
    }
    String output;
    serializeJson(doc, output);
    webSocket.sendTXT(num, output);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    if (type == WStype_DISCONNECTED) {
        for(auto i = clients.begin(); i != clients.end(); ++i) {
            if(i->id == num) {
                String name = i->name;
                clients.erase(i);
                
                StaticJsonDocument<200> sysMsg;
                sysMsg["type"] = "system";
                sysMsg["text"] = name + " saiu do chat.";
                String out;
                serializeJson(sysMsg, out);
                webSocket.broadcastTXT(out);
                notifyUserList();
                break;
            }
        }
    } 
    else if (type == WStype_TEXT) {
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload, lenght);
        if (error) return;

        String msgType = doc["type"];

        if (msgType == "login") {
            String name = doc["name"].as<String>();
            
            bool exists = false;
            for(auto& c : clients) {
                if(c.name == name) {
                    exists = true;
                    break;
                }
            }

            StaticJsonDocument<200> res;
            if(exists) {
                res["status"] = "error";
                res["message"] = "Este nome já está em uso na rede!";
                String out;
                serializeJson(res, out);
                webSocket.sendTXT(num, out);
            } else {
                bool makeAdmin = clients.empty();
                clients.push_back({num, name, makeAdmin});
                
                res["status"] = "success";
                res["isAdmin"] = makeAdmin;
                String out;
                serializeJson(res, out);
                webSocket.sendTXT(num, out);

                // Envia o histórico para o recém-chegado
                sendHistory(num);

                // Avisa todos que entrou
                StaticJsonDocument<200> sysMsg;
                sysMsg["type"] = "system";
                sysMsg["text"] = name + " entrou no chat.";
                String outSys;
                serializeJson(sysMsg, outSys);
                webSocket.broadcastTXT(outSys);
                notifyUserList();
            }
        } 
        else if (msgType == "chat") {
            String senderName = "";
            bool senderIsAdmin = false;
            for(auto& c : clients) {
                if(c.id == num) {
                    senderName = c.name;
                    senderIsAdmin = c.isAdmin;
                    break;
                }
            }

            if(chatFrozen && !senderIsAdmin) return;

            String text = doc["text"];

            // Verifica se é mensagem privada (ex: "@Fulano olá")
            if (text.startsWith("@")) {
                int spaceIndex = text.indexOf(' ');
                if (spaceIndex > 0) {
                    String targetName = text.substring(1, spaceIndex);
                    String privateText = text.substring(spaceIndex + 1);
                    
                    uint8_t targetId = 255;
                    for(auto& c : clients) {
                        if(c.name == targetName) {
                            targetId = c.id;
                            break;
                        }
                    }

                    if(targetId != 255) {
                        StaticJsonDocument<256> pMsg;
                        pMsg["type"] = "private";
                        pMsg["name"] = senderName + " (Para você)";
                        pMsg["text"] = privateText;
                        String out;
                        serializeJson(pMsg, out);
                        
                        // Envia para o destinatário e para quem enviou
                        webSocket.sendTXT(targetId, out);
                        webSocket.sendTXT(num, out);
                    } else {
                        StaticJsonDocument<200> err;
                        err["type"] = "system";
                        err["text"] = "Usuário '" + targetName + "' não encontrado.";
                        String out;
                        serializeJson(err, out);
                        webSocket.sendTXT(num, out);
                    }
                }
            } else {
                // Mensagem pública normal
                StaticJsonDocument<256> chatMsg;
                chatMsg["type"] = "chat";
                chatMsg["name"] = senderName;
                chatMsg["text"] = text;
                String out;
                serializeJson(chatMsg, out);
                webSocket.broadcastTXT(out);

                // Salva no histórico (mantém apenas as últimas 10)
                messageHistory.push_back({senderName, text});
                if(messageHistory.size() > 10) {
                    messageHistory.pop_front();
                }
            }
        }
        else if (msgType == "freeze") {
            for(auto& c : clients) {
                if(c.id == num && c.isAdmin) {
                    chatFrozen = !chatFrozen;
                    StaticJsonDocument<200> fMsg;
                    fMsg["type"] = "freeze";
                    fMsg["frozen"] = chatFrozen;
                    String out;
                    serializeJson(fMsg, out);
                    webSocket.broadcastTXT(out);
                }
            }
        }
        else if (msgType == "kick") {
            String targetName = doc["name"];
            bool isAdminSender = false;
            for(auto& c : clients) {
                if(c.id == num && c.isAdmin) {
                    isAdminSender = true;
                    break;
                }
            }

            if(isAdminSender) {
                uint8_t targetId = 255;
                for(auto i = clients.begin(); i != clients.end(); ++i) {
                    if(i->name == targetName) {
                        targetId = i->id;
                        clients.erase(i);
                        break;
                    }
                }
                if(targetId != 255) {
                    StaticJsonDocument<200> kMsg;
                    kMsg["type"] = "kick";
                    String out;
                    serializeJson(kMsg, out);
                    webSocket.sendTXT(targetId, out);
                    webSocket.disconnect(targetId);
                    notifyUserList();
                }
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    WiFi.softAP(ssid, password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    if (MDNS.begin("chat")) {
        Serial.println("mDNS iniciado: http://chat.local");
    }

    server.on("/", HTTP_GET, []() {
        server.send_P(200, "text/html", index_html);
    });

    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    server.handleClient();
    webSocket.loop();
}