Projeto: Dispensador Automático de Remédios (IoT)
Um protótipo de um dispensador de comprimidos inteligente, com firmware em C++ (Arduino) e controlo via App (Flutter).

Colaboradores
App Mobile (Flutter): Stanley 

Firmware & Integração iOS (Eu): João Ribeiro

Minhas Contribuições Específicas:
Neste projeto, o meu foco foi em duas áreas críticas:

1. Integração e Depuração do Hardware (Arduino/C++):

Defini a arquitetura do hardware (Arduino Uno, RTC DS3231, LCD I2C, Motor 28BYJ-48, BLE HM-10).

O firmware base em C++ foi gerado usando IA (Google Gemini) como ferramenta de co-piloto.

O meu trabalho foi adaptar, integrar e depurar esse código para funcionar com o hardware físico.

Resolvi conflitos de comunicação I2C que faziam o relógio (RTC) e a tela (LCD) falharem.

Resolvi conflitos de pinos do Bluetooth (HM-10), migrando a comunicação para SoftwareSerial (Pinos 7 e 5) para obter uma conexão estável.

Testei e calibrei os componentes individualmente (motor, sensores, tela) para garantir o funcionamento.

2. Deployment e Integração iOS (Flutter para iPhone):

Fui responsável por pegar no código-fonte do App (Flutter) e fazê-lo funcionar num dispositivo iOS.

Geri a instalação do ambiente de build (Flutter SDK, Xcode 14.2) no macOS 12.7, resolvendo problemas de PATH (zsh: command not found).

Configurei as permissões nativas do iOS (no Info.plist) para Bluetooth (BLE) e Localização.
