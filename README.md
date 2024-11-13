<h1 align="center">
    CheckPoint 2 - Edge Computing e Edge Systems 
</h1>

### Sobre

O objetivo inical do CheckPoint é atender a demanda da **Vinharia Agnello**, onde era necessário a criação de um sistema de monitoramento controlado por Arduino que permite verificar se o nível de luminosidade, temperatura e umidade são adequadas para a conservação do vinho.

<hr>

### Tecnologias utilizadas

O projeto foi desenvolvido com as seguintes tecnologias

Hardware

- Arduino 
- 1 Placa de ensaio
- 9 Resistores                         
- 9 LEDs (3 Verdes, 3 Amarelos e 3 Vermelhos)
- 1 Fotorresitor 
- 1 Buzzer
- 20 Cabos jumpers macho para macho 
- 1 Cabo USB Tipo A para Tipo B
- 1 Tela LCD
- 1 Sensor DHT11 
- 1 RTC (Real Time Clock)

Software

- [Arduino IDE](https://www.arduino.cc/en/software)
- [Tinkercard](https://www.tinkercad.com)
- [Wokwi](https://wokwi.com/)

<hr>

### Instalação

1. Baixe e instale o [Arduino IDE](https://www.arduino.cc/en/software).
2. Copie o código do projeto no Wokwi por [aqui](https://wokwi.com/projects/412632960863151105).
3. Cole o código na IDE do Arduino.
4. Verifique se as bibliotecas necessárias estão instaladas.
5. Conecte seu Arduino ao computador.
6. Selecione a placa correta em Ferramentas > Placa (por exemplo, Arduino Uno).
7. Selecione a porta correta em Ferramentas > Porta.
8. Carregue o código no Arduino clicando no botão **"Upload"**.

### Utilização 

1. Coloque o dispositivo em um ambiente que deseja monitorar.
2. Conecte o sistema ao fornecimento de energia via USB.
3. A tela LCD se acenderá com a logo e nome da empresa. Em seguida irá mostrar as medidas de luminosidade, temperatura e umidade. A tela exibe o horário e a data também.  
 
#### Luminosidade - Temperatura - Umidade:

- Se o ambiente estiver com a luminosidade ou temperatura ou umidade **adequada**, o **LED Verde** acenderá.
- Se o ambiente estiver com a luminosidade ou temperatura ou umidade **estável**, o **LED Amarelo** acenderá junto ao **Buzzer**, emitindo um som contínuo e agradável.
- Se o ambiente estiver com a luminosidade ou temperatura ou umidade **inadequada**, o **LED Vermelho** acenderá junto ao **Buzzer**, emitindo buzinas alto.

Caso haja algum problema sinalizado pelos LEDs ou pelo Buzzer, verificar as medidas pela tela LCD. 

<hr>
