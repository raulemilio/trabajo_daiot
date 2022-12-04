/*
 * gcp_min_ca.h
 *
 *  Created on: May 29, 2020
 *      Author: leopoldo
 */

#ifndef GCP_MIN_CA_H_
#define GCP_MIN_CA_H_

/*
 * Certificado CA mínimo de Google Cloud Platform (IoT Core)
 * https://cloud.google.com/iot/docs/how-tos/mqtt-bridge#using_a_long-term_mqtt_domain
 * Cert primario: https://pki.goog/gtsltsr/gtsltsr.crt
 * Cert backup: https://pki.goog/gsr4/GSR4.crt
 * 
 * Convertir certificados a formato PEM:
 * openssl x509 -inform DER -in gtsltsr.crt -out primary.pem -text
 * openssl x509 -inform DER -in GSR4.crt -out secondary.pem -text
 * 
 * Copiar el fragmento desde la línea que contiene BEGIN, hasta la que contiene END.
 */
const char GCP_MIN_CA[] =

"-----BEGIN CERTIFICATE-----\n"
"MIIBxTCCAWugAwIBAgINAfD3nVndblD3QnNxUDAKBggqhkjOPQQDAjBEMQswCQYD\n"
"VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzERMA8G\n"
"A1UEAxMIR1RTIExUU1IwHhcNMTgxMTAxMDAwMDQyWhcNNDIxMTAxMDAwMDQyWjBE\n"
"MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n"
"QzERMA8GA1UEAxMIR1RTIExUU1IwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAATN\n"
"8YyO2u+yCQoZdwAkUNv5c3dokfULfrA6QJgFV2XMuENtQZIG5HUOS6jFn8f0ySlV\n"
"eORCxqFyjDJyRn86d+Iko0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUw\n"
"AwEB/zAdBgNVHQ4EFgQUPv7/zFLrvzQ+PfNA0OQlsV+4u1IwCgYIKoZIzj0EAwID\n"
"SAAwRQIhAPKuf/VtBHqGw3TUwUIq7TfaExp3bH7bjCBmVXJupT9FAiBr0SmCtsuk\n"
"miGgpajjf/gFigGM34F9021bCWs1MbL0SA==\n"
"-----END CERTIFICATE-----\n";

#endif /* GOOGLE_MIN_CA_H_ */
