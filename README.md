# Trabajo práctico integrador DAIoT

## Firmware del embebido

Firmware desarrollado para esp32c3 con bmp280

Para el flash del embebido se utilizó la plataforma de espressif

Pasos:

- Habilitar la herramienta:

-- cd /esp/esp-idf/
-- . ./export.sh
-- cd  /carpeta del proyecto

- Seleccionar el dispositivo:

-- idf.py set-target esp32c3
-- idf.py flash monitor

### Tópicos mqtt

Para encender o apagar la electroválvula:
/topic/dispositivos/estadoElectrovalvula

Para obtener el registro de la electrónica:
/topic/dispositivos

Se utiliza un cliente MQTT para encender o apagar la electroválvula

### Datos de telemetría

{dev_id:1,temperatura:23.4,pressure:98999.91,rssi:-41,electrState:1}


### Broker

Se utlizó mosquitto con usuario y contraseña
