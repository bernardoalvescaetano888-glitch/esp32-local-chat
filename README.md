📡 ESP32 Local Wi-Fi Chat (WebSockets) 🚀💬
Um sistema de chat local em tempo real executado diretamente em um ESP32, sem precisar de internet! 🌐 O ESP32 cria a sua própria rede Wi-Fi (Access Point) e hospeda um servidor WebSockets para que qualquer smartphone ou computador conectado possa conversar instantaneamente. 📱💻

✨ Funcionalidades Principais
📶 Rede Local Própria (Access Point): Funciona totalmente offline em qualquer lugar, acampamentos, festas ou locais sem sinal!

⚡ Conexão via WebSockets: Mensagens instantâneas em tempo real sem precisar atualizar a página.

👑 Painel de Administrador Automático: O primeiro usuário a se conectar na rede ganha privilégios de Admin automaticamente!

🚫 Validação de Nomes: Impede que duas pessoas entrem com o mesmo nome na sala.

👥 Contador de Usuários Online: Mostra em tempo real quantas pessoas estão conectadas.

📜 Histórico de Mensagens: Exibe as últimas 10 mensagens trocadas para quem acabou de chegar.

🔒 Mensagens Privadas: Envie mensagens diretas usando o formato @NomeDoUsuario mensagem.

🛠️ Controles de Administrador:

Visualizar lista de usuários conectados.

Congelar/Descongelar o chat (bloqueia o envio de mensagens para usuários comuns).

Remover (Kickar) usuários indesejados da sala.

🔗 Suporte a mDNS: Acesse digitando [http://chat.local](http://chat.local) além do IP padrão [http://192.168.4.1](http://192.168.4.1).

🛠️ Tecnologias e Bibliotecas Utilizadas
Hardware: ESP32 NodeMCU / DevKit 🔌

Linguagem: C++ (Arduino IDE) 💻

Bibliotecas Necessárias:

WebServer (Nativa do ESP32 Core)

WebSocketsServer (por Markus Sattler)

ArduinoJson (versão 6 ou superior, por Benoit Blanchon)

📦 Como Instalar e Configurar
1️⃣ Pré-requisitos
Certifique-se de ter a Arduino IDE configurada com o suporte às placas ESP32 instalado. ⚙️

2️⃣ Instalação das Bibliotecas
Abra a Arduino IDE e vá em Ferramentas > Gerenciador de Bibliotecas... e instale:

WebSockets (procure por WebSockets by Markus Sattler) 📦

ArduinoJson (procure por ArduinoJson) 📦

3️⃣ Estruturação dos Arquivos
Crie uma pasta chamada chat e coloque os dois arquivos do projeto dentro dela:

chat.ino (Código principal do ESP32) 📄

chat.h (Interface gráfica HTML/CSS/JS) 🎨

4️⃣ Personalização da Rede (Opcional)
No arquivo chat.ino, você pode alterar o nome da rede Wi-Fi (SSID) e a senha nas linhas iniciais:

C++
const char* ssid = "ESP32-Chat-Local";
const char* password = ""; // Deixe vazio para rede aberta ou adicione senha (mínimo 8 caracteres)
5️⃣ Upload
Conecte o ESP32 ao computador via USB. 🔌

Selecione a placa correta em Ferramentas > Placa (ex: ESP32 Dev Module).

Selecione a porta COM correspondente e clique no botão Carregar (Upload). 🚀

📱 Como Usar
Ligue o ESP32 na tomada ou bateria. 🔋

No seu celular ou computador, procure pelas redes Wi-Fi disponíveis e conecte-se na rede: ESP32-Chat-Local. 📶

Abra o navegador de sua preferência e acesse:

IP Direto: [http://192.168.4.1](http://192.168.4.1) 🌐

Ou via mDNS: [http://chat.local](http://chat.local) 🔗

Digite seu nome e comece a conversar! O primeiro a entrar será o administrador. 🎉

🔮 Próximos Passos (Evolução para App)
Este projeto local serve como base para entendermos a arquitetura de WebSockets. Futuramente, a lógica poderá ser migrada para a nuvem (utilizando Node.js/Firebase) combinada com um aplicativo mobile desenvolvido em Flutter ou React Native, permitindo conversas via internet de qualquer lugar do mundo! 🌍📱

📄 Licença
Este projeto está sob a licença MIT. Sinta-se à vontade para modificar e melhorar! 💡
