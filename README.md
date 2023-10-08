"# servotempesp32" 
# PRACTICA III, IV SISTEMAS INFORMATICOS IOT
## Universidad Carlos III de Madrid

[![C|Arduino](https://i.ibb.co/sj2PmKF/c.png)Powered by Arduino Lenguage C](https://www.arduino.cc/en/software)

# Project Title

Ejercicio Servomotor control temperatura cliente web UC3M LABS.

## Description

El siguiente script tiene como objetivo programar la ESP32 como microcontrolador SoPC, acompañado de un sensor de temperatura BSM280 y un actuador el servomotor SG90. Permitiendo comprender los componentes de arquitectura de hardware utilizados, de igual forma el script que acompaña al microcontrolador para establecer unas condiciones de acuerdo con la práctica III y IV de la clase Sistemas Informáticos de IoT, Master universitario Internet de las Cosas UC3M, estableciendo estructuras de control en C para interactuar ambos elementos sensor y servomotor dentro unas condiciones predefinidas. 
Esta información esta publicada mediante un servidor web que se expone mediante el router y el componente de wifi incluido en el microcontrolador obteniendo un segmento de red para la publicación de los datos medidos con una respuesta HTML, ante la petición por el método GET del protocolo HTTP al servidor.

### Dependencies

* Las librerias utilizadas estan disponibles por el fabricante y por colaboradores en la internet.
* Licensed MIT Libreria Servomotor SG90, BME280.

This is heavily based on library https://github.com/erropix/ESP32_AnalogWrite/tree/master
This is library BME280 https://www.sparkfun.com/products/14348

## Authors

Contributors names and contact info

Mario Edwin Beltrán Sarmiento 
email: 100505609@alumnos.uc3m.es
Bryan David Tamayo 
email. 100449123@alumnos.uc3m.es

## Version History

* 0.2
    * Ajustes y optimizacion
* 0.1
    * Release inicial

## License

The code contained in this repository and the executable distributions are licensed under the terms of the GNU AGPLv3. The executable distributions contain third-party code licensed under other compatible licenses such as GPLv2, MIT and BSD-3. If you have questions about licensing please contact us at license@arduino.cc.