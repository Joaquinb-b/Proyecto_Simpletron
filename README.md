# Proyecto Simpletron
# Computadora Virtual Simpletron y Compilador Simple a SML

Este proyecto final es la implementación completa de la computadora virtual **Simpletron**, capaz de ejecutar su lenguaje de máquina (SML). Además, incluye un compilador que traduce programas escritos en el lenguaje de alto nivel **Simple** hacia código **SML** ejecutable.

El proyecto está diseñado bajo lenguaje C, separando la lógica del simulador base, la evaluación de expresiones y las fases de compilación (primera y segunda pasada con manejo de tabla de símbolos).

## Integrantes
* Joaquín Blanca Barranco (ID: 3419391)

## Descripción de Archivos Principales
* `src/simpletron/simpletron.c`: Simulador de la máquina Simpletron. Ejecuta las instrucciones SML, administra la memoria (100 palabras) y registros, y detecta errores fatales.
* `src/expresiones/anidamiento.c`: Validador de balanceo de símbolos de agrupación `()`, `[]`, `{}` mediante pilas.
* `src/expresiones/postfijo.c`: Evaluador de expresiones aritméticas (infijas a postfijas) manejando precedencias y asociatividad.
* `src/compilador/compilador.c`: Traductor de Simple a SML. Realiza la lectura del código fuente, construye la tabla de símbolos y resuelve referencias adelantadas (goto/if).
* `programa1.simple`: Código de prueba escrito en alto nivel.

## Instrucciones para poder convertir cualquier .simple a .sml
Primero se usa este codigo
gcc src/compilador/compilador.c -o compilador p

Después este, cambia NOMBRE_DEL_ARCHIVO por el nombre de tu .simple
.\compilador.exe NOMBRE_DEL_ARCHIVO.simple

Después se ejecuta este código
gcc src/simpletron/simpletron.c -o simpletron -lm

Después se hace lo mismo para poder usar el simpletron
.\simpletron.exe NOMBRE_DEL_ARCHIVO.sml
