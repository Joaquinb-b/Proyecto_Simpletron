#include <stdio.h>
#include <stdlib.h> // Necesario para system("pause")

// Definimos el tamano de la memoria y el numero centinela para parar la carga
#define MEM_SIZE 100
#define SENTINEL 9999

// Definimos los codigos de las operaciones de Entrada y Salida
#define READ        10
#define WRITE       11

// Definimos los codigos para Cargar y Guardar datos en memoria
#define LOAD        20
#define STORE       21

// Definimos los codigos para las operaciones matematicas
#define ADD         30
#define SUBTRACT    31
#define DIVIDE      32
#define MULTIPLY    33

// Definimos los codigos para los saltos (bifurcaciones) y para detener el programa
#define BRANCH      40
#define BRANCHNEG   41
#define BRANCHZERO  42
#define HALT        43

// Prototipo de la funcion para imprimir el vaciado de memoria y registros
void dump(int accumulator, int instructionCounter, int instructionRegister, 
          int operationCode, int operand, int memory[]);

int main(void) {
    // Arreglo para simular la memoria de 100 palabras, inicializado todo en 0
    int memory[MEM_SIZE];
    int i;
    for (i = 0; i < MEM_SIZE; i++) {
        memory[i] = 0; // Llenamos la memoria de ceros al inicio
    }

    // Registros de la computadora Simpletron inicializados en 0
    int accumulator = 0;         // Guarda los resultados temporales y operaciones
    int instructionCounter = 0;  // Apunta a la posicion de la instruccion actual
    int instructionRegister = 0; // Guarda la instruccion completa de 4 digitos
    int operationCode = 0;       // Guarda los dos primeros digitos (codigo de operacion)
    int operand = 0;             // Guarda los dos ultimos digitos (direccion de memoria)

    // Variables de control para saber si el simulador sigue corriendo
    int corriendo = 1; // 1 significa verdadero (sigue corriendo), 0 significa falso (se detiene)
    int salto = 0;     // Bandera para saber si hubo un branch y no incrementar el contador

    // Imprimimos el mensaje de bienvenida tal como pide el manual
    printf("*** Bienvenido a Simpletron! ***\n");
    printf("*** Cargando programa automaticamente desde archivo SML ***\n\n");

    // FASE 1: CARGA DEL PROGRAMA DESDE EL ARCHIVO SML EN DISCO
    FILE *archivoSml = fopen("programa1.sml", "r");
    if (archivoSml == NULL) {
        printf("*** Error: No se pudo abrir el archivo programa1.sml ***\n");
        system("pause");
        return 1;
    }

    int posicionCarga = 0;
    int instruccionLeida = 0;

    // Leemos el archivo SML instrucción por instrucción
    while (fscanf(archivoSml, "%d", &instruccionLeida) == 1 && posicionCarga < MEM_SIZE) {
        memory[posicionCarga] = instruccionLeida;
        posicionCarga = posicionCarga + 1;
    }

    fclose(archivoSml);

    printf("*** Se termino de cargar el programa ***\n");
    printf("*** Comienza la ejecucion del programa ***\n\n");

    // FASE 2: CICLO DE EJECUCION (FETCH, DECODE, EXECUTE)

    while (corriendo == 1 && instructionCounter < MEM_SIZE) {
        
        // 1. FETCH (Traer la instruccion desde la memoria)
        instructionRegister = memory[instructionCounter];

        // 2. DECODE (Separar el codigo de operacion y el operando)
        // La division entera descarta los ultimos dos digitos
        operationCode = instructionRegister / 100;
        // El operador residuo obtiene unicamente los dos ultimos digitos
        operand = instructionRegister % 100;

        // Si el operando salio negativo por el signo de la instruccion, lo convertimos a positivo
        if (operand < 0) {
            operand = -operand;
        }

        // Validamos que la direccion a la que apunta el operando exista en la memoria
        if (operand >= MEM_SIZE) {
            printf("\n*** Direccion de memoria fuera de rango (%02d) ***\n", operand);
            printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
            corriendo = 0;
            break;
        }

        // Reiniciamos la bandera de salto antes de ejecutar la instruccion
        salto = 0;

        // 3. EXECUTE (Evaluar la operacion y ejecutarla)
        switch (operationCode) {
            
            // Operacion READ: Lee un valor del teclado y lo guarda en memoria
            case READ: {
                int valorLeido = 0;
                printf("? ");
                fflush(stdout); // Fuerza a mostrar el prompt inmediatamente en la consola
                scanf("%d", &valorLeido);
                
                // Verificamos que el valor ingresado este entre -9999 y +9999
                while (valorLeido < -9999 || valorLeido > 9999) {
                    printf("*** Valor fuera de rango (-9999 a 9999). Intente de nuevo.\n? ");
                    fflush(stdout);
                    scanf("%d", &valorLeido);
                }
                
                memory[operand] = valorLeido;
                break;
            }

            // Operacion WRITE: Muestra en pantalla el valor almacenado en la memoria
            case WRITE:
                printf("%+05d\n", memory[operand]);
                break;

            // Operacion LOAD: Pasa un valor de la memoria hacia el acumulador
            case LOAD:
                accumulator = memory[operand];
                break;

            // Operacion STORE: Guarda el contenido del acumulador en una posicion de memoria
            case STORE:
                memory[operand] = accumulator;
                break;

            // Operacion ADD: Suma al acumulador el numero que esta en memoria
            case ADD: {
                int resultadoSuma = accumulator + memory[operand];
                // Checamos si la suma desborda el acumulador
                if (resultadoSuma < -9999 || resultadoSuma > 9999) {
                    printf("\n*** Desbordamiento del acumulador ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    accumulator = resultadoSuma;
                }
                break;
            }

            // Operacion SUBTRACT: Resta al acumulador el numero que esta en memoria
            case SUBTRACT: {
                int resultadoResta = accumulator - memory[operand];
                // Checamos si la resta desborda el acumulador
                if (resultadoResta < -9999 || resultadoResta > 9999) {
                    printf("\n*** Desbordamiento del acumulador ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    accumulator = resultadoResta;
                }
                break;
            }

            // Operacion DIVIDE: Divide el acumulador entre el valor en memoria
            case DIVIDE:
                // Verificamos el caso de error de division entre cero
                if (memory[operand] == 0) {
                    printf("\n*** Intento de dividir entre cero ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    accumulator = accumulator / memory[operand];
                }
                break;

            // Operacion MULTIPLY: Multiplica el acumulador por el valor en memoria
            case MULTIPLY: {
                int resultadoMulti = accumulator * memory[operand];
                // Checamos si la multiplicacion desborda el acumulador
                if (resultadoMulti < -9999 || resultadoMulti > 9999) {
                    printf("\n*** Desbordamiento del acumulador ***\n");
                    printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                    corriendo = 0;
                } else {
                    accumulator = resultadoMulti;
                }
                break;
            }

            // Operacion BRANCH: Salto incondicional a la direccion indicada por el operando
            case BRANCH:
                instructionCounter = operand;
                salto = 1; // Marcamos que salto para no hacer el ++ al final
                break;

            // Operacion BRANCHNEG: Salta solo si el acumulador tiene un valor negativo
            case BRANCHNEG:
                if (accumulator < 0) {
                    instructionCounter = operand;
                    salto = 1;
                }
                break;

            // Operacion BRANCHZERO: Salta solo si el acumulador es igual a 0
            case BRANCHZERO:
                if (accumulator == 0) {
                    instructionCounter = operand;
                    salto = 1;
                }
                break;

            // Operacion HALT: Termina la ejecucion exitosa del programa
            case HALT:
                printf("\n*** Termino la ejecucion de Simpletron ***\n");
                corriendo = 0;
                break;

            // Si el codigo no coincide con ninguna de las 12 operaciones validas
            default:
                printf("\n*** Codigo de operacion no valido (%02d) ***\n", operationCode);
                printf("*** La ejecucion de Simpletron termino anormalmente ***\n");
                corriendo = 0;
                break;
        }

        // Si el programa sigue corriendo y no hubo un salto (branch), avanzamos a la siguiente instruccion
        if (salto == 0 && corriendo == 1) {
            instructionCounter = instructionCounter + 1;
        }
    }

    // FASE 3: VACIADO DE MEMORIA (DUMP)

    // Se ejecuta siempre al final, ya sea por terminacion normal o error fatal
    dump(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);

    // Pausa para evitar que la consola se cierre de golpe en Visual Studio
    system("pause");

    return 0;
}

// Funcion encargada de imprimir el estado final de los registros y toda la memoria
void dump(int accumulator, int instructionCounter, int instructionRegister, 
          int operationCode, int operand, int memory[]) {
    
    // Imprimimos los registros formateados
    printf("\nREGISTROS:\n");
    printf("%-20s%+05d\n", "accumulator:", accumulator);
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
            // Imprime cada palabra con su signo y rellenando con ceros a 4 digitos
            printf("%+05d ", memory[fila * 10 + columna]);
        }
        printf("\n"); // Salto de linea al terminar cada fila de 10 palabras
    }
}