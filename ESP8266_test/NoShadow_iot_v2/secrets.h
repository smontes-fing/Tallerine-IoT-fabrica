#include <pgmspace.h>
 
#define SECRET
 
// const char WIFI_SSID[] = "sebastian";               //TAMIM2.4G
// const char WIFI_PASSWORD[] = "12345678";           //0544287380
const char WIFI_SSID[] = "wifing";               //TAMIM2.4G
const char WIFI_PASSWORD[] = "wifing-pub";           //0544287380 
#define THINGNAME "NoShadow_iot"

                          //NYC(USA): -5 UTC
                          //Montevideo (Uy): -3 UTC
int8_t TIME_ZONE = -3; 
 
const char MQTT_HOST[] = "a1gbg0pky9tuvc-ats.iot.us-east-2.amazonaws.com"; // Device data endpoint 
 
 
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
 
// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUXaCo08Cr+AUnq5yNvHuye7psb10wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MDIwNTE5MzAz
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMZWRuEJmBKPkyppXhL1
dKgF7P5hNYh4I43dy/wDxDuJuKWbmxuXr76UPWVfXM++2PkGq0RKtWRZm5kwoLRG
49Mth4r693cKAviGe6fP9YfGB/xeylHPxhbpS9oXsJ7+lbyi4fVi6YfnK+qTPb0J
wTWjfVmHED+L1G4i76GePszN5jLCRSvFsfwwN9uu30XJ2Kj5yfTgLHP0O9hqJXef
PCgUvsNuX+E26zAsYkSMrP+4II6afKSQ9O8IWArLQim1pkRhtKjbdpZPCP/+5kvX
kwgj8h8xTxF4Qry/wMyvE8z9GZuFF3epF4f0loYPwJv9GB+3ZlrZ9GfkNQS2J0Cf
vt0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUX6IrTK6grpz3BhwdPgzC2fQoM/EwHQYD
VR0OBBYEFKBFD2j/8nVj6hSHTphr1j6i5np3MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQDciu7dn8YfToPrXpOPuqz6jMRy
2xwkZA2xlhIYwOm6QxwA3pb8Eq592gsbo0Rjxa7GYiWf6wLBvESgZWXWu2rAEVjC
PG4+75L7SOeHMy32uZ2DK73BplQcM3mIATv9MyyR0QM6faGrvFlLDG9Ip7keYTVu
tJW1tDPA7eYGSzI5O+sTmhFnSaw49q4yTWyh5K2yLJB3wpj0hGHzTDs11R7FR7gV
1C4rEwdLiEwv6Z54zP/Qj2HPyNDW+fpKa1oEuCQUPO8q2qdghZoUmHuTupVgxc3g
3jc0FoRZQBYSps3Dp7FalP6VdFx57YgIJPa5mJqdn5vLLByFPaw20s+hXSn3
-----END CERTIFICATE-----
 
)KEY";
 
 
// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAxlZG4QmYEo+TKmleEvV0qAXs/mE1iHgjjd3L/APEO4m4pZub
G5evvpQ9ZV9cz77Y+QarREq1ZFmbmTCgtEbj0y2Hivr3dwoC+IZ7p8/1h8YH/F7K
Uc/GFulL2hewnv6VvKLh9WLph+cr6pM9vQnBNaN9WYcQP4vUbiLvoZ4+zM3mMsJF
K8Wx/DA3267fRcnYqPnJ9OAsc/Q72Gold588KBS+w25f4TbrMCxiRIys/7ggjpp8
pJD07whYCstCKbWmRGG0qNt2lk8I//7mS9eTCCPyHzFPEXhCvL/AzK8TzP0Zm4UX
d6kXh/SWhg/Am/0YH7dmWtn0Z+Q1BLYnQJ++3QIDAQABAoIBACasbhe1UjTUUCLl
9tUFjaCxvxt/AvxASNaB2COk7F+bpCiOpfkEY91qdGpO5P8RPKeaOE0RsDsICgId
7EgaxpPH/fUIqLZKenfRaU04jKzT3XzFUvgO5pZTGVpGYkcuZy4oKJ07k0PPvM15
toEk5Mq7vVZ2sqFhd6OeJlHCVIaEiwWlreQwbjuBp5/hiH82CFHorSLuAtUxAODZ
nyAit/l1ggPprQnBZWaYpHq6k+/N7Jo2/ZZPhNe0A9MzvU0s4qMLYVVUmm65WL/7
PToFgSxY4HYrZYJR5hybFn9M9rrEwRVFXHeqZjaaIGIaDxA+WYkyiG9fUc9Hp0dr
ofbRtkECgYEA/pKPeGUyw27IQJ7aZS/JT/uzbybinH14nuxdUtpYavpXDJTWfjBM
1sxwLNrEjE+aqcwnw7dGdbqPoLjoprTz1Wt3WZsTbrq+rEAD3A/jRettbM6DPTZ+
2MopoitnY3hh2ImzV4ewf7FUtp2EDal8IjBUdgvxdsbLFgI5+Wt/C5ECgYEAx3L9
gG7dFccpwtdHvQp4+xsTNbtRH8t0nr98qqx29DFMFIPRvM1K0LLdOKauNjxT8fV6
4TT8i8LZFWjuhEeeiqxxMHhB4BX78weoj3zTe8WMiSP4LYGjuYIOy6qGpse5xGeS
NfNRuMW64RXzi8wISYBSqMr04y8U8W72/ipQYI0CgYEA8T2/+c6kqXDdEee3fHYp
ZsNWSfnme3eni6rf0Tka2t27CjAth02MM21uzALQiUqSfV/7Osv+WGhf3WX9CqWl
/9QkOOTtKFyNfjv3rF3d+Q9eFeMpmSOAVz5U79oD0s5sor/zFPp/9rH8XkWssymu
edOk5mzw+KbwIz4e5E7LCRECgYEAwiwN5kqITJpv6rNhsQYQEA8k3Yu2h+iwaZhs
y1BKIqQXuGU4/ME0VAe+XVfQhRaodwgREE1gjMQuSay1gOykycbmD1UUdQ+SEOFk
1gSlc3s4vY7aH5t1hLW8p8UlgBsznKVkb8epyOQfcuDHEVOawcv/LTshvEdq8T2h
koIpbEUCgYBVvYT2yJFZKqZHsKDXGvh3CQv4yrLS/rVZ159dQtkjzlVWuDXoVIs7
6rwAipDpuA9pwhsHHgMuMkzRZyCj8Q5b+k44A6Dg5Fx82/wGBNvXHer1QQ0S2774
yej4sYjgkFNaVVR5NZEX8pYY9OpwHVx+DbIdp2wKJwfgnoVot7yweA==
-----END RSA PRIVATE KEY-----
 
)KEY";
