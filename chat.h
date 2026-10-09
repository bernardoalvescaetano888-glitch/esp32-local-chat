#ifndef CHAT_H
#define CHAT_H

#include <Arduino.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Local Chat</title>
    <style>
        body { font-family: Arial, sans-serif; background: #f0f2f5; margin: 0; padding: 20px; display: flex; flex-direction: column; align-items: center; }
        #login-container, #chat-container { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); width: 100%; max-width: 400px; }
        #chat-container { max-width: 600px; height: 85vh; display: flex; flex-direction: column; }
        #messages { flex: 1; border: 1px solid #ddd; border-radius: 4px; padding: 10px; overflow-y: auto; margin-bottom: 10px; background: #fafafa; }
        .msg { margin-bottom: 8px; word-break: break-word; }
        .msg span.user { font-weight: bold; color: #007bff; }
        .private { background: #ffeeba; padding: 4px 8px; border-radius: 4px; border-left: 4px solid #ffc107; }
        #admin-panel { background: #fff3cd; padding: 10px; border-radius: 4px; margin-bottom: 10px; display: none; }
        input, button { padding: 10px; margin: 5px 0; width: 100%; box-sizing: border-box; border: 1px solid #ccc; border-radius: 4px; }
        button { background: #007bff; color: white; border: none; cursor: pointer; }
        button:hover { background: #0056b3; }
        .danger { background: #dc3545; }
        .danger:hover { background: #a71d2a; }
    </style>
</head>
<body>

    <div id="login-container">
        <h2>Entrar no Chat</h2>
        <input type="text" id="username" placeholder="Digite seu nome..." />
        <button onclick="login()">Entrar</button>
        <p id="error-msg" style="color: red;"></p>
    </div>

    <div id="chat-container" style="display:none;">
        <h2>ESP32 Chat Local (<span id="online-count">0</span> online)</h2>
        <div id="admin-panel">
            <h3>Painel do Administrador</h3>
            <div id="user-list"></div>
            <button class="danger" onclick="toggleFreeze()">Congelar/Descongelar Chat</button>
        </div>
        <div id="messages"></div>
        <div id="input-area">
            <input type="text" id="msg-input" placeholder="Digite sua mensagem (use @nome para privado)..." onkeypress="checkEnter(event)" />
            <button onclick="sendMessage()">Enviar</button>
        </div>
    </div>

<script>
    let ws;
    let myName = "";
    let isAdmin = false;
    let isFrozen = false;

    function login() {
        const nameInput = document.getElementById('username').value.trim();
        if(!nameInput) return;
        
        ws = new WebSocket('ws://' + window.location.hostname + ':81/');
        
        ws.onopen = function() {
            ws.send(JSON.stringify({type: 'login', name: nameInput}));
        };
        
        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            if(data.status === 'error') {
                document.getElementById('error-msg').innerText = data.message;
                ws.close();
            } else if(data.status === 'success') {
                myName = nameInput;
                isAdmin = data.isAdmin;
                document.getElementById('login-container').style.display = 'none';
                document.getElementById('chat-container').style.display = 'flex';
                if(isAdmin) {
                    document.getElementById('admin-panel').style.display = 'block';
                }
            } else if(data.type === 'history') {
                const msgs = document.getElementById('messages');
                data.history.forEach(m => {
                    msgs.innerHTML += `<div class="msg"><span class="user">${m.name}:</span> ${m.text}</div>`;
                });
                msgs.scrollTop = msgs.scrollHeight;
            } else if(data.type === 'chat') {
                const msgs = document.getElementById('messages');
                msgs.innerHTML += `<div class="msg"><span class="user">${data.name}:</span> ${data.text}</div>`;
                msgs.scrollTop = msgs.scrollHeight;
            } else if(data.type === 'private') {
                const msgs = document.getElementById('messages');
                msgs.innerHTML += `<div class="msg private"><span class="user">[Privado] ${data.name}:</span> ${data.text}</div>`;
                msgs.scrollTop = msgs.scrollHeight;
            } else if(data.type === 'system') {
                const msgs = document.getElementById('messages');
                msgs.innerHTML += `<div class="msg" style="color: gray; font-style: italic;"><i>${data.text}</i></div>`;
                msgs.scrollTop = msgs.scrollHeight;
            } else if(data.type === 'freeze') {
                isFrozen = data.frozen;
                document.getElementById('msg-input').disabled = isFrozen;
            } else if(data.type === 'kick') {
                alert("Você foi removido pelo administrador.");
                location.reload();
            } else if(data.type === 'users') {
                document.getElementById('online-count').innerText = data.users.length;
                if(isAdmin) {
                    let html = "<h4>Usuários Online:</h4><ul>";
                    data.users.forEach(u => {
                        if(u !== myName) {
                            html += `<li>${u} <button onclick="kickUser('${u}')" class="danger" style="width:auto;padding:2px 5px;">Remover</button></li>`;
                        } else {
                            html += `<li>${u} (Você)</li>`;
                        }
                    });
                    html += "</ul>";
                    document.getElementById('user-list').innerHTML = html;
                }
            }
        };
    }

    function sendMessage() {
        if(isFrozen && !isAdmin) return;
        const input = document.getElementById('msg-input');
        const text = input.value.trim();
        if(text && ws) {
            ws.send(JSON.stringify({type: 'chat', text: text}));
            input.value = '';
        }
    }

    function checkEnter(e) {
        if(e.key === 'Enter') sendMessage();
    }

    function toggleFreeze() {
        if(isAdmin && ws) {
            ws.send(JSON.stringify({type: 'freeze'}));
        }
    }

    function kickUser(name) {
        if(isAdmin && ws) {
            ws.send(JSON.stringify({type: 'kick', name: name}));
        }
    }
</script>
</body>
</html>
)rawliteral";

#endif