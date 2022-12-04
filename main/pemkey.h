/*
 * pemkey.h
 *
 *  Created on: Jul 16, 2020
 *      Author: leopoldo
 */

#ifndef MAIN_PEMKEY_H_
#define MAIN_PEMKEY_H_

/*
 * Clave PRIVADA del dispositivo registrado en Google Cloud Platform - IotCore
 * https://cloud.google.com/iot/docs/how-tos/credentials/keys
 *
 * Crear claves publica y privada para cada dispositivo desde la consola de GCP con la herramienta OPENSSL
 * 
 * openssl req -x509 -nodes -newkey rsa:2048 -keyout rsa_lz_private.pem -out rsa_lz_cert.pem -subj "/CN=unused" -days 36500 
 *  
 * rsa_private.pem: The private key that must be securely stored on the device and used to sign the authentication JWT.
 * rsa_public.pem: The public key that must be stored in Cloud IoT Core and used to verify the signature of the authentication JWT.
 */

const char GCP_PEM_KEY[] =

"-----BEGIN PRIVATE KEY-----\n"
"MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQCz0iB8jwPQz1o/\n"
"xH4P9e8nHf+yncZsGnRGfktKylgfrH2/0Ke2Nm2wvQBQr81eKa3cVP5PnU5vxS/3\n"
"xNgOBByVyZEJi3rkllbNZXX5wFlOGh4mj0nuJlDr+HdpMrt2VJuT7vEZE0eOA95R\n"
"J1o/A8EwElhjEG1R+BbItFRGz278YKgw1lwKCSD3Vu1f0wuDL/4I+DOlGVD/Conj\n"
"WHeAfv72NnPEBISPm7NDTvjVhdqZQjciUTpCtFtFP8bW6mAQtnEtjTrstclPW0N2\n"
"gv/kqJTb6BViaEcrA/cJAraOhfYO5YRPJj2ZJvwNGxdIn9GSuLBRkQApeyNj8jDq\n"
"jA15n6EvAgMBAAECggEBAKxGfM84qNwtO3ZqRqf940AjsmwqUgYRliJPLs0ZbDH0\n"
"LIlPWs3r+Uzg1eN3ehZjcejsGLSnrZTRYrJKiP9smwI6GVT+px9GlSS0tsEusaBs\n"
"z0P79UVOuipIYgaMDyM/zcl9msh+Owho1l4DiKdkbnoiqUFHZKSqPRHOCgvJAbiR\n"
"OY4w5hW4GDjVTKL2Eo4/jfmhGaeg2Ygp/TwgWihWTsXFSI0gdwtsaYuIDsPrwB+w\n"
"1pe0i9QUQoDE2Z4NuLLA8tKGILfYs4HAzWfcDT6FkWzuLpcoZuqQw9MEqpkgd2aq\n"
"H3hLRtDYNtsFhNCBIUjbNB7QX98y+dXHyOHnE4sLDokCgYEA5kMhrGjXuiBXiNNQ\n"
"8eI7MIov7Llc3n7/sYGb+Y8nI/srWbYXC55ZugwKNlb9xAXh9rPIyvm9cfhkg0c2\n"
"FMWDsk8LBylN2toUqr3sRa4AyrpGdFEDcO7uECqQYtLACNrGqhO0AVtvl7uWPxkk\n"
"27DqqCKloPGuoLknoWZzditokHsCgYEAx+uh+GDEZmhulhtq/wPZpJ+1yhzHlfKt\n"
"TX88sVBJ8XqOT7KNPFrCbJY1qQ5kYypgyvWI/K60Vc36dSQ0mlu9vd9VGS8DA2rj\n"
"wjZ6F54UlXJkP2pwBUEGQXeHsINFHXYy5VQwLMFNXhmA9qzdboEYCN5BsoS6xBaL\n"
"AmxQP4Vxhd0CgYEAp3V/EdIVwekvf/Se06sB/OUiwXh1T+Lwtflao/42KvbJqi7B\n"
"R9Pb0XBcJPTTDTJyCWbqyqMh4bcR8l9ht22Fv7G4hXbGhjk/+oSxRmOrc8RQ5Nre\n"
"M82wGNW7uAMpYccp4BsAOFR1LdZhCHaS1asunSTFa+0YYQ8HAVxu8CvVme0CgYBr\n"
"piTEEo8OuaL6bu8RPCiAbYM8vnx18IOC3EV5EMuQHLbukS1d110jjUp9qoNBpnsS\n"
"Ll5B+WyfW1dGpmUwzL+BsvUmp1rPB0p2tFDP0M4ynrL+Nrj1C0tsCwnkcPrRX2kO\n"
"dWOvZbLcPY1kxjZBn3+qnsrvbmsG1UCq4FnhqmAzfQKBgQDIq8nLWpQl6zi+Hx0j\n"
"TcTRllFJ3BcS9bzCeWo+3zR9alJcgl82ZYuOa5M7RWzL2C1KxIjbjwZMy30PRi5A\n"
"H25KRhYS2WrqzKCzvh1D7hFB67NWbLtM7KoaKmkPXXzVmnffmm0pBwzVjd24+rnE\n"
"o3kAsxQ8sFr6Q458BrBo0cn4jw==\n"
"-----END PRIVATE KEY-----\n";

#endif /* MAIN_PEMKEY_H_ */
