#include <stdio.h>

// Definimos el tamano de la memoria y el numero centinela para parar la carga
#define MEM_SIZE 100
#define SENTINEL 9999

// Definimos los codigos de las operaciones de Entrada y Salida (LMS / SML)
#define READ        10 // Lee una palabra del teclado y la almacena en una direccion
#define WRITE       11 // Escribe en pantalla la palabra de una direccion especifica

// Definimos los codigos para Cargar y Guardar datos en memoria
#define LOAD        20 // Carga una palabra de la memoria hacia el acumulador
#define STORE       21 // Almacena el contenido del acumulador en la memoria

// Definimos los codigos para las operaciones matematicas
#define ADD         30 // Suma el valor de la memoria al acumulador
#define SUBTRACT    31 // Resta el valor de la memoria al acumulador
#define DIVIDE      32 // Divide el acumulador entre el valor de la memoria
#define MULTIPLY    33 // Multiplica el acumulador por el valor de la memoria

// Definimos los codigos para los saltos (bifurcaciones) y para detener el programa
#define BRANCH      40 // Salto incondicional a una posicion de memoria
#define BRANCHNEG   41 // Salto condicional si el acumulador es estrictamente negativo (< 0)
#define BRANCHZERO  42 // Salto condicional si el acumulador es exactamente cero (== 0)
#define HALT        43 // Detiene la simulacion de forma ordenada

// Prototipo de la funcion para imprimir el vaciado de memoria y registros (Dump)
void dump(int acumulador, int instructionCounter, int instructionRegister, 
          int operationCode, int operand, int memoria[]);

int main(void) {
    // Arreglo para simular la memoria de 100 palabras, inicializado todo en 0
    int memoria[MEM_SIZE];
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        memoria[i] = 0; // Llenamos la memoria de ceros al inicio
    }

    // Registros de la computadora Simpletron inicializados en 0
    int acumulador = 0;          // Guarda los resultados temporales y operaciones
    int instructionCounter = 0;  // Apunta a la posicion de la instruccion actual
    int instructionRegister = 0; // Guarda la instruccion completa de 4 digitos
    int operationCode = 0;       // Guarda los dos primeros digitos (codigo de operacion)
    int operand = 0;             // Guarda los dos ultimos digitos (direccion de memoria)

    // Variables de control para saber si el simulador sigue corriendo
    int corriendo = 1; // 1 significa verdadero (sigue corriendo), 0 significa falso (se detiene)
    int salto = 0;     // Bandera para saber si hubo un branch y no incrementar el contador

    // Imprimimos el mensaje de bienvenida tal como pide el manual
    printf("*** Bienvenido a Simpletron! ***\n");
    printf("*** Introduzca su programa una instruccion ***\n");
    printf("*** (o palabra de datos) a la vez en la linea ***\n");
    printf("*** de texto de entrada. Yo indicare el numero ***\n");
    printf("*** de posicion y una interrogacion (?). Usted ***\n");
    printf("*** tecleara entonces la palabra para esa ***\n");
    printf("*** posicion. Teclee el centinela 9999 para ***\n");
    printf("*** dejar de introducir su programa. ***\n\n");

    // =========================================================================
    // FASE 1: CARGA DEL PROGRAMA EN LA MEMORIA
    // =========================================================================
    
    int entrada = 0;
    int posicionCarga = 0;

    // Se reciben instrucciones una a una hasta llenar la memoria o recibir el centinela
    while (posicionCarga < MEM_SIZE) {
        // Mostramos el numero de posicion con dos digitos y el signo de interrogacion
        printf("%02d ? ", posicionCarga);
        scanf("%d", &entrada);

        // Si el usuario introduce 9999, terminamos la carga de instrucciones
        if (entrada == SENTINEL) {
            break;
        }

        // Validamos que el numero este dentro del rango permitido (-9999 a 9998)
        if (entrada < -9999 || entrada > 9998) {
            printf("*** Instruccion fuera de rango (-9999 a +9998). Intente de nuevo. ***\n");
            // Usamos continue para que no avance de posicion y vuelva a pedir el valor
            continue; 
        }

        // Si es valido, guardamos la instruccion en la memoria y pasamos a la siguiente posicion
        memoria[posicionCarga] = entrada;
        posicionCarga = posicionCarga + 1;
    }

    printf("\n*** Se termino de cargar el programa ***\n");
    printf("*** Comienza la ejecucion del programa ***\n\n");

    // =========================================================================
    // FASE 2: CICLO DE EJECUCION DE LA CPU (FETCH, DECODE, EXECUTE)
    // =========================================================================

    while (corriendo == 1 && instructionCounter < MEM_SIZE) {
        
        // ---------------------------------------------------------------------
        // 1. FETCH (Traer la instruccion desde la memoria)
        // ---------------------------------------------------------------------
        // Tomamos la instruccion apuntada actualmente por el instructionCounter
        instructionRegister = memoria[instructionCounter];

        // ---------------------------------------------------------------------
        // 2. DECODE (Decodificar la instruccion)
        // ---------------------------------------------------------------------
        // En una palabra de 4 digitos (+OOMM):
        // La division entera descarta los ultimos dos digitos y obtiene el codigo de operacion (OO)
        operationCode = instructionRegister / 100;
        // El operador residuo (%) obtiene unicamente los dos ultimos digitos (la direccion MM)
        operand = instructionRegister % 100;

        // Si el operando salio negativo por el signo de la instruccion, lo convertimos a positivo
        if (operand < 0) {
            operand = -operand;
        }

        // Validamos que la direccion a la que apunta el operando exista en la memoria (0 a 99)
        if (operand >= MEM_SIZE) {
            printf("\n*** Direccion de memoria fuera de rango (%02d) ***\n", operand);
            printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
            corriendo = 0;
            break;
        }

        // Reiniciamos la bandera de salto antes de ejecutar la instruccion
        salto = 0;

        // ---------------------------------------------------------------------
        // 3. EXECUTE (Evaluar la operacion y ejecutarla en la ALU / Registros)
        // ---------------------------------------------------------------------
        switch (operationCode) {
            
            // Operacion READ: Lee un valor del teclado y lo guarda en memoria
            case READ: {
                int valorLeido = 0;
                printf("? ");
                scanf("%d", &valorLeido);
                
                // Verificamos que el valor ingresado este entre -9999 y +9999
                while (valorLeido < -9999 || valorLeido > 9999) {
                    printf("*** Valor fuera de rango (-9999 a 9999). Intente de nuevo.\n? ");
                    scanf("%d", &valorLeido);
                }
                
                memoria[operand] = valorLeido;
                break;
            }

            // Operacion WRITE: Muestra en pantalla el valor almacenado en la memoria
            case WRITE:
                // %+05d imprime el signo obligatorio (+ o -) y rellena a 4 cifras con ceros
                printf("%+05d\n", memoria[operand]);
                break;

            // Operacion LOAD: Pasa un valor de la memoria hacia el acumulador
            case LOAD:
                acumulador = memoria[operand];
                break;

            // Operacion STORE: Guarda el contenido del acumulador en una posicion de memoria
            case STORE:
                memoria[operand] = acumulador;
                break;

            // Operacion ADD: Suma al acumulador el numero que esta en memoria
            case ADD: {
                int resultadoSuma = acumulador + memoria[operand];
                // Checamos si la suma desborda la capacidad del acumulador (-9999 a +9999)
                if (resultadoSuma < -9999 || resultadoSuma > 9999) {
                    printf("\n*** Desbordamiento del acumulador ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    acumulador = resultadoSuma;
                }
                break;
            }

            // Operacion SUBTRACT: Resta al acumulador el numero que esta en memoria
            case SUBTRACT: {
                int resultadoResta = acumulador - memoria[operand];
                // Checamos si la resta desborda la capacidad del acumulador
                if (resultadoResta < -9999 || resultadoResta > 9999) {
                    printf("\n*** Desbordamiento del acumulador ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    acumulador = resultadoResta;
                }
                break;
            }

            // Operacion DIVIDE: Divide el acumulador entre el valor en memoria
            case DIVIDE:
                // Verificamos el caso critico de division entre cero
                if (memoria[operand] == 0) {
                    printf("\n*** Intento de dividir entre cero ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    acumulador = acumulador / memoria[operand];
                }
                break;

            // Operacion MULTIPLY: Multiplica el acumulador por el valor en memoria
            case MULTIPLY: {
                int resultadoMulti = acumulador * memoria[operand];
                // Checamos si la multiplicacion supera los 4 digitos permitidos
                if (resultadoMulti < -9999 || resultadoMulti > 9999) {
                    printf("\n*** Desbordamiento del acumulador ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    acumulador = resultadoMulti;
                }
                break;
            }

            // Operacion BRANCH: Salto incondicional a la direccion indicada por el operando
            case BRANCH:
                instructionCounter = operand; // Sobrescribimos el contador con la nueva direccion
                salto = 1;                    // Marcamos que salto para evitar el incremento automatico
                break;

            // Operacion BRANCHNEG: Salta solo si el acumulador tiene un valor negativo
            case BRANCHNEG:
                if (acumulador < 0) {
                    instructionCounter = operand;
                    salto = 1;
                }
                break;

            // Operacion BRANCHZERO: Salta solo si el acumulador es igual a 0
            case BRANCHZERO:
                if (acumulador == 0) {
                    instructionCounter = operand;
                    salto = 1;
                }
                break;

            // Operacion HALT: Termina la ejecucion exitosa del programa
            case HALT:
                printf("\n*** Termino la ejecucion de Simpletron ***\n");
                corriendo = 0; // Rompe el ciclo while principal
                break;

            // Si el codigo no coincide con ninguna de las 12 operaciones validas del set de instrucciones
            default:
                printf("\n*** Codigo de operacion no valido (%02d) ***\n", operationCode);
                printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                corriendo = 0;
                break;
        }

        // Si el programa sigue corriendo y no hubo un salto (branch), avanzamos a la siguiente instruccion secuencial
        if (salto == 0 && corriendo == 1) {
            instructionCounter = instructionCounter + 1;
        }
    }

    // =========================================================================
    // FASE 3: VACIADO DE MEMORIA (DUMP)
    // =========================================================================
    // Se ejecuta siempre al final, ya sea por terminacion normal (HALT) o error fatal
    dump(acumulador, instructionCounter, instructionRegister, operationCode, operand, memoria);

    return 0;
}

// Funcion encargada de imprimir el estado final de los registros y toda la memoria en formato tabular
void dump(int acumulador, int instructionCounter, int instructionRegister, 
          int operationCode, int operand, int memoria[]) {
    
    // Imprimimos los registros formateados con alineacion a la izquierda (%-)
    printf("\nREGISTROS:\n");
    printf("%-20s%+05d\n", "acumulador:", acumulador);
    printf("%-23s%02d\n", "instructionCounter:", instructionCounter);
    printf("%-20s%+05d\n", "instructionRegister:", instructionRegister);
    printf("%-23s%02d\n", "operationCode:", operationCode);
    printf("%-23s%02d\n\n", "operand:", operand);

    // Imprimimos la cabecera de las columnas de memoria (0 al 9)
    printf("MEMORIA:\n");
    printf("%8d%7d%7d%7d%7d%7d%7d%7d%7d%7d\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    // Con dos ciclos for recorremos e imprimimos la cuadricula de 10x10 de la memoria
    int fila, columna;
    for (fila = 0; fila < 10; fila++) {
        // Imprime el numero de fila a la izquierda (0, 10, 20, ..., 90)
        printf("%2d ", fila * 10);
        
        for (columna = 0; columna < 10; columna++) {
            // Imprime cada palabra con su signo obligatorio (+/-) y rellenando con ceros a 4 digitos
            printf("%+05d ", memoria[fila * 10 + columna]);
        }
        printf("\n"); // Salto de linea al terminar cada fila de 10 palabras
    }
}