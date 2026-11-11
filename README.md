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

## Instrucciones de Compilación
Para compilar los módulos del proyecto, utiliza el compilador `gcc` desde la terminal:

```bash
# Compilar el simulador Simpletron
gcc src/simpletron/simpletron.c -o simpletron

# Compilar el Traductor/Compilador de Simple
gcc src/compilador/compilador.c -o compilador