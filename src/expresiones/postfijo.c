// stdio.h (Standard Input/Output): Proporciona funciones de entrada y salida,
// como printf() para mostrar datos en la consola.
#include <stdio.h>

// stdlib.h (Standard Library): Proporciona funciones de utilidad general del sistema,
// como system("pause") para pausar la consola y exit() para terminar el programa.
#include <stdlib.h>

// ctype.h (Character Handling): Proporciona funciones para clasificar caracteres;
// se usa isdigit() para verificar si un char es un número ('0'-'9').
#include <ctype.h>

// math.h: Proporciona operaciones matemáticas avanzadas, como pow() para potencias.
#include <math.h>

// Define el tamaño máximo de la pila (capacidad de elementos)
#define TAM 100

// Arreglo estático donde se almacenan los números temporales de la expresión
float pila[TAM];

// Índice que rastrea la cima de la pila; -1 significa que la pila está vacía
int tope = -1;

/*
 * Función: push
 * ----------------
 * Inserta un nuevo número float en la parte superior de la pila.
 * Primero incrementa 'tope' (++tope) y luego guarda el valor en esa posición.
 */
void push(float valor) {
    pila[++tope] = valor;
}

/*
 * Función: pop
 * ----------------
 * Extrae y retorna el número que se encuentra en la cima de la pila.
 * Toma el valor en la posición 'tope' y luego decrementa 'tope' (tope--).
 */
float pop() {
    return pila[tope--];
}

/*
 * Función: evaluar
 * ----------------
 * Recibe una cadena de caracteres con la expresión en postfijo y retorna el
 * resultado numérico final como float siguiendo el algoritmo del pseudocódigo.
 */
float evaluar(char expresion[]) {
    // symb: almacena el caracter actual que se está analizando
    char symb;

    // i: índice contador para recorrer la cadena desde el inicio
    int i = 0;

    // opnd1, opnd2: variables temporales para guardar los operandos extraídos
    float opnd1, opnd2;

    // value: variable para guardar el resultado parcial de cada operación
    float value;

    // 2. Mientras no hayamos llegado al final de la cadena ('\0' es el fin de cadena en C)
    while (expresion[i] != '\0') {
        // 2.1. Copiar el siguiente caracter en symb
        symb = expresion[i];

        // Si el caracter es un espacio en blanco, lo ignoramos y avanzamos
        if (symb == ' ') {
            i++;
            continue;
        }

        // 2.2. Si symb es un operando (dígito numérico)
        if (isdigit(symb)) {
            // Convertimos el char a su valor numérico real restando el código ASCII de '0'
            // Ejemplo: '6' - '0' = 54 - 48 = 6.0
            push((float)(symb - '0'));
        } 
        // 2.3. Si no (es un operador: +, -, *, /, ^)
        else {
            // 2.3.1. Extraer el segundo operando (el que entró al final)
            opnd2 = pop();

            // 2.3.2. Extraer el primer operando (el que entró primero)
            opnd1 = pop();

            // 2.3.3. value = resultado de aplicar la operación (opnd1 symb opnd2)
            switch (symb) {
                case '+':
                    value = opnd1 + opnd2;
                    break;
                case '-':
                    value = opnd1 - opnd2;
                    break;
                case '*':
                    value = opnd1 * opnd2;
                    break;
                case '/':
                    value = opnd1 / opnd2;
                    break;
                case '^':
                    // En C el caracter '^' no eleva potencia, por eso usamos pow()
                    value = pow(opnd1, opnd2);
                    break;
                default:
                    // Manejo de error si aparece un caracter desconocido
                    printf("Error: Operador no reconocido '%c'\n", symb);
                    system("pause");
                    exit(1);
            }

            // 2.3.4. Guardar el resultado parcial de nuevo en la pila
            push(value);
        }

        // Avanzamos al siguiente caracter de la cadena
        i++;
    }

    // 3. El resultado final de la expresión queda en la cima de la pila
    return pop();
}

int main() {
    // Declaramos un arreglo de caracteres para almacenar la expresión ingresada por el usuario
    char expresion[100];
    // Solicitamos al usuario que ingrese la expresión en notación postfija
    printf("Ingrese la expresion en postfijo: ");
    // Leemos la cadena de entrada y la almacenamos en 'expresion'
    scanf("%s", expresion);

    // Llamamos a la función evaluar() para calcular el resultado de la expresión
    float resultado = evaluar(expresion);

    // Mostramos el resultado final en la consola con formato de número entero (sin decimales)
    printf("Resultado de la evaluacion: %.0f\n\n", resultado);

    // Pausa del sistema para mantener abierta la ventana del ejecutable .exe
    system("pause");

    return 0;
}