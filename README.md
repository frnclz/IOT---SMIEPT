# Sistema de Monitoramento da Integridade Estrutural de Pontes e Trapiches

Disciplina: Internet das Coisas - Profª Salete Farias

Aluna: Franciele Alves

## Descrição do Problema

Pontes e trapiches localizados em ambientes marítimos estão constantemente expostos a condições adversas, como ação das ondas, marés, ressacas e corrosão causada pela água salgada. Esses fatores podem provocar deformações, vibrações excessivas e inclinações que comprometem a integridade estrutural ao longo do tempo.

A ausência de monitoramento contínuo dificulta a identificação precoce de falhas, aumentando o risco de acidentes e reduzindo a eficiência das ações de manutenção preventiva.

Este projeto propõe um sistema de monitoramento estrutural baseado em Internet das Coisas (IoT), capaz de acompanhar em tempo real as condições da estrutura e acionar mecanismos de segurança quando forem detectadas situações de risco.

## Objetivo

Desenvolver um sistema capaz de monitorar deformações e inclinações em pontes ou trapiches, analisando continuamente os dados coletados por sensores e executando ações automáticas de segurança quando necessário.

## Componentes Utilizados

* ESP32                                  
* MPU6050                                
* Potenciômetro (simulando extensômetro) 
* Servo Motor SG90                       
* Protoboard                             
* Jumpers                                
* Fonte de Alimentação                   

## Funcionamento do Sistema

O sistema realiza o monitoramento contínuo da estrutura através dos sensores instalados.

### Monitoramento de Inclinação

O sensor MPU6050 mede a inclinação da estrutura, permitindo identificar deslocamentos anormais causados por cargas excessivas, ressacas ou falhas estruturais.

### Monitoramento de Deformação

Um potenciômetro é utilizado para simular um extensômetro (strain gauge), representando a deformação da estrutura sob esforço mecânico.

### Ação de Segurança

Quando os valores de inclinação ou deformação ultrapassam os limites estabelecidos:

* A cancela de segurança é fechada automaticamente através do servo motor;
* O acesso à estrutura é bloqueado;
* Um alerta é enviado aos responsáveis pelo monitoramento;
* Os dados são registrados para futuras análises e manutenção preventiva.

## Fluxo do Sistema

1. Os sensores coletam dados de inclinação e deformação.
2. O ESP32 processa as informações localmente.
3. O sistema verifica os limites de segurança.
4. Em caso de risco:
   * A cancela é fechada;
   * Alertas são enviados aos responsáveis.
5. Os dados são publicadas via MQTT e enviadas ao painel de monitoramento para visualização em tempo real.

## Instalação

### Bibliotecas Necessárias
* Adafruit MPU6050
* Adafruit Unified Sensor
* ESP32Servo

### Configuração

1. Conecte o MPU6050 ao ESP32 utilizando comunicação I2C.
2. Conecte o potenciômetro a uma entrada analógica do ESP32.
3. Conecte o servo motor ao pino digital configurado no código.
4. Faça o upload do código para o ESP32.
5. Abra o Monitor Serial para acompanhar os dados coletados.

## Utilização

Após a inicialização:

1. O MPU6050 começa a monitorar a inclinação da estrutura.
2. O potenciômetro simula a deformação estrutural.
3. Os valores são analisados continuamente.
4. Quando um limite de segurança é ultrapassado, a cancela fecha automaticamente.
5. Quando os valores retornam à faixa segura, a cancela é reaberta.

## Circuito

<img width="767" height="670" alt="image" src="https://github.com/user-attachments/assets/c5439391-b6a6-41ea-bc39-34ec40b5ce36" />

Acesse o [circuito no Wokwi](https://wokwi.com/projects/466573165787430913) para visualizar a simulação do projeto.

## Dashboard
<img width="1847" height="868" alt="image" src="https://github.com/user-attachments/assets/4c92637d-fc1f-4507-a23f-a6d0f9b28c49" />
<img width="1847" height="862" alt="image" src="https://github.com/user-attachments/assets/de60b9bf-e8c5-46ee-a419-edad134b415c" />

Acesse o [dashboard do projeto](https://iot-smiept.vercel.app/) para visualizar em tempo real.

## Implementações Realizadas

Até o presente momento, foram desenvolvidas e testadas as seguintes funcionalidades do sistema:

* Monitoramento de Inclinação da Estrutura
* Simulação de Monitoramento de Deformação
* Sistema de Bloqueio de Segurança
* Processamento Local dos Dados

## Implementações Realizadas - v2
* Integração do protocolo MQTT para comunicação.
* Envio de alertas remotos em situações de risco.
* Desenvolvimento de dashboard para monitoramento em tempo real.
  
## Aplicações Futuras
* Armazenamento histórico dos dados coletados.
* Melhorar a função do botão de alerta.
* Implementação do extensômetro junto ao módulo HX711.
