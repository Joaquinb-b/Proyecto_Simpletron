#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MEM_SIZE 100
#define MAX_LINE_LENGTH 256

// Definición de tipos para la Tabla de Símbolos
// type: 'l' = número de línea, 'v' = variable, 'c' = constante
typedef struct TableEntry {
    int symbol;             // Número de línea, valor de constante o código ASCII de la variable (ej. 'x')
    char type;              // 'l', 'v', 'c'
    int location;           // Posición de memoria en SML (00 a 99)
    struct TableEntry* next;
} TableEntry;

// Estructura global para la Tabla de Símbolos
TableEntry* symbolTable = NULL;

// Memoria SML e Índice de instrucciones generadas (Instruction Counter)
int smlMemory[MEM_SIZE];
int instructionCounter = 0;

// Arreglo de banderas para referencias no resueltas (inicializado en -1)
int flags[MEM_SIZE];

// Puntero para la asignación de variables/constantes desde el final de la memoria hacia atrás (comienza en 99)
int variableMemoryCounter = MEM_SIZE - 1;

// --- FUNCIONES DE LA TABLA DE SÍMBOLOS ---

// Buscar un símbolo en la tabla
TableEntry* searchSymbol(int symbol, char type) {
    TableEntry* current = symbolTable;
    while (current != NULL) {
        if (current->symbol == symbol && current->type == type) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Insertar un símbolo en la tabla
void insertSymbol(int symbol, char type, int location) {
    TableEntry* newNode = (TableEntry*)malloc(sizeof(TableEntry));
    newNode->symbol = symbol;
    newNode->type = type;
    newNode->location = location;
    newNode->next = symbolTable;
    symbolTable = newNode;
}

// Obtener o asignar ubicación de memoria para variables o constantes
int getOrInsertDataLocation(int val, char type) {
    TableEntry* existing = searchSymbol(val, type);
    if (existing != NULL) {
        return existing->location;
    }
    // Si no existe, se asigna desde el final de la memoria hacia atrás (99, 98, 97...)
    int loc = variableMemoryCounter--;
    insertSymbol(val, type, loc);
    
    // NUEVO: Guardar el valor real en la memoria SML si es una constante
    if (type == 'c') {
        smlMemory[loc] = val;
    }
    
    return loc;
}


// --- PRIMERA PASADA ---
void primeraPasada(FILE* inputFile) {
    char line[MAX_LINE_LENGTH];

    // Inicializar memoria SML y banderas en -1
    for (int i = 0; i < MEM_SIZE; i++) {
        smlMemory[i] = 0;
        flags[i] = -1;
    }

    while (fgets(line, sizeof(line), inputFile) != NULL) {
        // Remover salto de línea final si existe
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        // Tokenizar la línea usando espacios
        char* token = strtok(line, " ");
        if (token == NULL) continue;

        // 1. El primer token siempre es el número de línea del programa Simple
        int lineNumber = atoi(token);
        insertSymbol(lineNumber, 'l', instructionCounter);

        // 2. Siguiente token: el comando (rem, input, print, goto, if, let, end)
        char* command = strtok(NULL, " ");
        if (command == NULL) continue;

        if (strcmp(command, "rem") == 0) {
            // Los comentarios se ignoran, no generan SML ni incrementan IC
            continue;
        }
        else if (strcmp(command, "input") == 0) {
            char* var = strtok(NULL, " ");
            int varLoc = getOrInsertDataLocation(var[0], 'v');
            
            // Generar instrucción SML: 10 (READ) + ubicación
            smlMemory[instructionCounter] = 1000 + varLoc;
            instructionCounter++;
        }
        else if (strcmp(command, "print") == 0) {
            char* var = strtok(NULL, " ");
            int varLoc = getOrInsertDataLocation(var[0], 'v');
            
            // Generar instrucción SML: 11 (WRITE) + ubicación
            smlMemory[instructionCounter] = 1100 + varLoc;
            instructionCounter++;
        }
        else if (strcmp(command, "goto") == 0) {
            char* targetLineStr = strtok(NULL, " ");
            int targetLine = atoi(targetLineStr);

            // Buscar si la línea destino ya fue procesada (hacia atrás) o registrar referencia adelantada
            TableEntry* targetEntry = searchSymbol(targetLine, 'l');
            if (targetEntry != NULL) {
                smlMemory[instructionCounter] = 4000 + targetEntry->location; // BRANCH incondicional
            } else {
                // Referencia adelantada: dejamos en 00 el operando y guardamos la línea destino en flags
                smlMemory[instructionCounter] = 4000; 
                flags[instructionCounter] = targetLine;
            }
            instructionCounter++;
        }
        else if (strcmp(command, "if") == 0) {
            // Ejemplo: if y == x goto 60
            char* var1 = strtok(NULL, " ");
            char* op = strtok(NULL, " "); // ==, <, >, etc.
            char* var2 = strtok(NULL, " ");
            strtok(NULL, " "); // saltar "goto"
            char* targetLineStr = strtok(NULL, " ");
            int targetLine = atoi(targetLineStr);

            int loc1 = getOrInsertDataLocation(var1[0], 'v');
            int loc2 = getOrInsertDataLocation(var2[0], 'v');

            // 1. Cargar var1
            smlMemory[instructionCounter++] = 2000 + loc1;
            // 2. Restar var2
            smlMemory[instructionCounter++] = 3100 + loc2;

            // 3. Evaluar salto condicional (si es igual, la resta da cero -> Branch Zero 42)
            TableEntry* targetEntry = searchSymbol(targetLine, 'l');
            if (targetEntry != NULL) {
                smlMemory[instructionCounter] = 4200 + targetEntry->location;
            } else {
                smlMemory[instructionCounter] = 4200; // Operando temporal 00
                flags[instructionCounter] = targetLine; // Referencia adelantada
            }
            instructionCounter++;
        }
        else if (strcmp(command, "let") == 0) {
            // Ejemplo simplificado para asignaciones directas o expresiones básicas (let y = y + 1)
            char* varDest = strtok(NULL, " ");
            strtok(NULL, " "); // saltar "="
            char* op1 = strtok(NULL, " ");
            char* operador = strtok(NULL, " ");
            char* op2 = strtok(NULL, " ");

            int destLoc = getOrInsertDataLocation(varDest[0], 'v');

            if (operador == NULL) {
                // Asignación simple: let x = 5 o let x = y
                int sourceVal = isdigit(op1[0]) ? atoi(op1) : op1[0];
                char sourceType = isdigit(op1[0]) ? 'c' : 'v';
                int sourceLoc = getOrInsertDataLocation(sourceVal, sourceType);

                smlMemory[instructionCounter++] = 2000 + sourceLoc; // LOAD
                smlMemory[instructionCounter++] = 2100 + destLoc;   // STORE
            } else {
                // Operación aritmética básica (ej: y + 1)
                int val1 = isdigit(op1[0]) ? atoi(op1) : op1[0];
                char type1 = isdigit(op1[0]) ? 'c' : 'v';
                int loc1 = getOrInsertDataLocation(val1, type1);

                int val2 = isdigit(op2[0]) ? atoi(op2) : op2[0];
                char type2 = isdigit(op2[0]) ? 'c' : 'v';
                int loc2 = getOrInsertDataLocation(val2, type2);

                smlMemory[instructionCounter++] = 2000 + loc1; // LOAD operando 1

                if (strcmp(operador, "+") == 0) {
                    smlMemory[instructionCounter++] = 3000 + loc2; // ADD
                } else if (strcmp(operador, "-") == 0) {
                    smlMemory[instructionCounter++] = 3100 + loc2; // SUBTRACT
                } else if (strcmp(operador, "*") == 0) {
                    smlMemory[instructionCounter++] = 3300 + loc2; // MULTIPLY
                } else if (strcmp(operador, "/") == 0) {
                    smlMemory[instructionCounter++] = 3200 + loc2; // DIVIDE
                }

                smlMemory[instructionCounter++] = 2100 + destLoc; // STORE resultado
            }
        }
        else if (strcmp(command, "end") == 0) {
            // Generar instrucción SML: 43 (HALT)
            smlMemory[instructionCounter] = 4300;
            instructionCounter++;
        }
    }
}


// --- SEGUNDA PASADA ---
void segundaPasada(const char* outputFilename) {
    // 1. Recorrer el arreglo flags buscando referencias no resueltas (!= -1)
    for (int i = 0; i < MEM_SIZE; i++) {
        if (flags[i] != -1) {
            int targetLineNumber = flags[i];
            
            // 2. Buscar en la tabla de símbolos la ubicación SML correspondiente a ese número de línea
            TableEntry* entry = searchSymbol(targetLineNumber, 'l');
            if (entry != NULL) {
                // 3. Insertar la posición de memoria (location) en el operando de la instrucción incompleta
                int baseOpCode = smlMemory[i] / 100; // Extraer código de operación (ej: 40 o 42)
                smlMemory[i] = (baseOpCode * 100) + entry->location;
            } else {
                printf("Error: Referencia a la línea no existente %d\n", targetLineNumber);
            }
        }
    }

    // 4. Guardar el código SML resultante en un archivo de disco (una instrucción por línea)
    FILE* outFile = fopen(outputFilename, "w");
    if (outFile == NULL) {
        printf("No se pudo crear el archivo de salida SML.\n");
        return;
    }

    // NUEVO: Cambiar instructionCounter por MEM_SIZE para imprimir las 100 palabras
    for (int i = 0; i < MEM_SIZE; i++) {
        fprintf(outFile, "%+05d\n", smlMemory[i]);
    }

    fclose(outFile);
    printf("\n*** Compilación exitosa. Código SML guardado en '%s' ***\n", outputFilename);
}


// --- FUNCIÓN PRINCIPAL ---
int main(void) {
    FILE* file = fopen("programa1.simple", "r");
    if (file == NULL) {
        printf("Error al abrir el archivo fuente programa1.simple\n");
        return 1;
    }

    printf("Iniciando Primera Pasada...\n");
    primeraPasada(file);
    fclose(file);

    printf("Iniciando Segunda Pasada y resolución de referencias...\n");
    segundaPasada("programa1.sml");

    return 0;
}