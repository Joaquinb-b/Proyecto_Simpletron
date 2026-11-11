#include <stdio.h>   // Incluir la biblioteca estÃ¡ndar de entrada y salida
#include <stdbool.h> // Incluir la biblioteca para usar el tipo de dato booleano
#include <string.h>  // Incluir la biblioteca para manipulacion de cadenas

#define MAX 100 // TamaÃ±o maximo de la cadena

// --- Estructura de la Pila ---
char P[MAX];   // Arreglo para almacenar los simbolos
int tope = -1; // Indice del tope de la pila

bool isEmpty() { // Funcion para verificar si la pila esta vacia
    return tope == -1; // Verdadero si la pila esta vacia
}

void push(char symb) { // Funcion para agregar un simbolo a la pila
    if (tope < MAX - 1) { // Verificar si hay espacio en la pila
        tope++;           // Incrementar el indice del tope
        P[tope] = symb;   // Agregar el simbolo al tope de la pila
    }
}

char pop() { // Funcion para sacar un simbolo de la pila
    if (!isEmpty()) { // Verificar si la pila no esta vacia
        char temp = P[tope]; // Guardar el simbolo del tope
        tope--;              // Decrementar el indice del tope
        return temp;         // Retornar el simbolo sacado de la pila
    }
    return '\0'; // Retornar un caracter nulo si la pila esta vacia
}

// Funcion auxiliar para verificar si el cierre corresponde a la apertura
bool esPareja(char apertura, char cierre) { 
    if (apertura == '(' && cierre == ')') return true; // Verificar si es una pareja de parentesis
    if (apertura == '{' && cierre == '}') return true; // Verificar si es una pareja de llaves
    if (apertura == '[' && cierre == ']') return true; // Verificar si es una pareja de corchetes
    return false; // Retornar falso si no es una pareja valida
}

// --- Algoritmo principal ---
int verificarAnidamiento(char cadena[]) { // Funcion para verificar el anidamiento de la cadena
    tope = -1; // Reiniciar la pila 

    for (int i = 0; cadena[i] != '\0'; i++) { // Mientras no se llegue al final de la cadena
        char symb = cadena[i]; // Guardar el caracter actual

        if (symb == '(' || symb == '{' || symb == '[') { // Verificar si es un simbolo de apertura
            push(symb); // Agregar el simbolo a la pila
        }
        else if (symb == ')' || symb == '}' || symb == ']') { // Verificar si es un simbolo de cierre
            if (isEmpty()) { // Verificar si la pila esta vacia
                return 0; // Falso, ya que no hay apertura correspondiente
            } else { 
                char temp = pop(); // Guardar el simbolo de apertura correspondiente
                
                if (!esPareja(temp, symb)) { // Verificar si la apertura y cierre no corresponden
                    return 0; // Falso
                }
            }
        }
    }

    if (!isEmpty()) { // Verificar si la pila no esta vacía despuès de recorrer toda la cadena
        return 0; // Falso
    }

    // Si todo cerró correctamente
    return 1; // Verdadero
}

int main() {
    char cadena[MAX]; // Arreglo para almacenar la cadena ingresada por el usuario

    // Solicitar la cadena al usuario
    printf("Ingrese la expresion a evaluar: ");
    if (fgets(cadena, sizeof(cadena), stdin) != NULL) { 
        // Eliminar el salto de línea '\n' que fgets suele incluir al final
        cadena[strcspn(cadena, "\n")] = '\0'; 

        // Evaluar la cadena
        int resultado = verificarAnidamiento(cadena);

        // Mostrar salida según el pseudocódigo: 1 (Verdadero) o 0 (Falso)
        printf("\nResultado: %d\n", resultado);
        if (resultado == 1) { // Si el resultado es 1, la expresión es correcta
            printf("Estado: Expresion correcta (Verdadero)\n");
        } else { // Si el resultado es 0, la expresión es incorrecta
            printf("Estado: Expresion incorrecta (Falso)\n");
        }
    }

    // --- PAUSA PARA QUE NO SE CIERRE LA CONSOLA ---
    printf("\nPresione ENTER para salir...");
    getchar(); // Espera a que el usuario presione una tecla/Enter

    return 0; // Retornar 0 para indicar que el programa finalizó correctamente
}