# 📡 ESP32 Local Wi-Fi Chat (WebSockets)

Um sistema de chat local em tempo real executado diretamente em um **ESP32**, sem necessidade de internet. O ESP32 cria a sua própria rede Wi-Fi (Access Point) e hospeda um servidor WebSockets para que qualquer smartphone ou computador conectado à rede possa conversar instantaneamente.

---

## 🚀 Funcionalidades

- **Rede Local Própria (Access Point):** Funciona totalmente offline em qualquer lugar.
- **Conexão via WebSockets:** Mensagens em tempo real sem precisar atualizar a página.
- **Painel de Administrador Automático:** O **primeiro usuário** a se conectar ganha privilégios de Admin.
- **Validação de Nomes:** Impede que duas pessoas entrem com o mesmo nome na rede.
- **Contador de Usuários Online:** Mostra em tempo real quantas pessoas estão conectadas.
- **Histórico de Mensagens:** Exibe as últimas 10 mensagens trocadas para quem acabou de entrar.
- **Mensagens Privadas:** Envie mensagens diretas usando o formato `@NomeDoUsuario mensagem`.
- **Controles de Administrador:** 
  - Visualizar lista de usuários conectados.
  - **Congelar/Descongelar o chat** (bloqueia o envio de mensagens para usuários comuns).
  - **Remover (Kickar)** usuários indesejados da sala.
- **Suporte a mDNS:** Acesse digitando `http://chat.local` além do IP padrão `http://192.168.4.1`.

---

## 🛠️ Tecnologias e Bibliotecas Utilizadas

- **Hardware:** ESP32 NodeMCU / DevKit.
- **Linguagem:** C++ (Arduino IDE).
- **Bibliotecas Necessárias:**
  - `WebServer` (Nativa do ESP32 Core)
  - `WebSocketsServer` (por *Markus Sattler*)
  - `ArduinoJson` (versão 6 ou superior, por *Benoit Blanchon*)

---

## 📦 Como Instalar e Configurar

### 1. Pré-requisitos
Certifique-se de ter a **Arduino IDE** configurada com o suporte às placas ESP32 instalado.

### 2. Instalação das Bibliotecas
Abra a Arduino IDE e vá em **Ferramentas > Gerenciador de Bibliotecas...** e instale:
1. **WebSockets** (procure por *WebSockets by Markus Sattler*)
2. **ArduinoJson** (procure por *ArduinoJson*)

### 3. Estruturação dos Arquivos
Crie uma pasta chamada `chat` e coloque os dois arquivos do projeto dentro dela:
- `chat.ino` (Código principal do ESP32)
- `chat.h` (Interface gráfica HTML/CSS/JS)

### 4. Personalização da Rede (Opcional)
No arquivo `chat.ino`, você pode alterar o nome da rede Wi-Fi (SSID) e a senha nas linhas iniciais:
```cpp
const char* ssid = "ESP32-Chat-Local";
const char* password = ""; // Deixe vazio para rede aberta ou adicione senha (mínimo 8 caracteres)
