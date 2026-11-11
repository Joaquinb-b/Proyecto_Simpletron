#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MEM_SIZE 100
#define MAX_LINE_LENGTH 256

// Nodo para la tabla de simbolos (guarda variables, constantes y numeros de linea)
typedef struct TableEntry {
    int symbol;             // El numero, valor o letra de la variable
    char type;              // 'l' = linea, 'v' = variable, 'c' = constante
    int location;           // Posicion en la memoria SML (00 al 99)
    struct TableEntry* next;// Apuntador al siguiente nodo
} TableEntry;

TableEntry* symbolTable = NULL;

int smlMemory[MEM_SIZE];
int instructionCounter = 0;

// Arreglo para guardar los saltos que faltan por resolver
int flags[MEM_SIZE];

// Empieza a asignar memoria desde el final hacia atras (99, 98, 97...)
int variableMemoryCounter = MEM_SIZE - 1;

// Busca si un simbolo ya esta guardado en la tabla
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

// Inserta un nuevo simbolo al inicio de la lista
void insertSymbol(int symbol, char type, int location) {
    TableEntry* newNode = (TableEntry*)malloc(sizeof(TableEntry));
    newNode->symbol = symbol;
    newNode->type = type;
    newNode->location = location;
    newNode->next = symbolTable;
    symbolTable = newNode;
}

// Revisa si ya existe la variable/constante, si no, le asigna su celda atras
int getOrInsertDataLocation(int val, char type) {
    TableEntry* existing = searchSymbol(val, type);
    if (existing != NULL) {
        return existing->location;
    }
    
    int loc = variableMemoryCounter--;
    insertSymbol(val, type, loc);
    
    // Si es una constante, la guardamos de una vez en la memoria SML
    if (type == 'c') {
        smlMemory[loc] = val;
    }
    
    return loc;
}

// PRIMERA PASADA: Lee el codigo y genera las instrucciones basicas
void primeraPasada(FILE* inputFile) {
    char line[MAX_LINE_LENGTH];

    // Limpiamos memoria y flags
    for (int i = 0; i < MEM_SIZE; i++) {
        smlMemory[i] = 0;
        flags[i] = -1;
    }

    // Leemos el archivo linea por linea
    while (fgets(line, sizeof(line), inputFile) != NULL) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;

        char* token = strtok(line, " ");
        if (token == NULL) continue;

        // 1. Guardamos el numero de linea de Simple
        int lineNumber = atoi(token);
        insertSymbol(lineNumber, 'l', instructionCounter);

        // 2. Leemos el comando (input, print, let, goto, if, end)
        char* command = strtok(NULL, " ");
        if (command == NULL) continue;

        if (strcmp(command, "rem") == 0) {
            continue; // Ignoramos comentarios
        }
        else if (strcmp(command, "input") == 0) {
            char* var = strtok(NULL, " ");
            int varLoc = getOrInsertDataLocation(var[0], 'v');
            
            smlMemory[instructionCounter] = 1000 + varLoc; // 10 es READ
            instructionCounter++;
        }
        else if (strcmp(command, "print") == 0) {
            char* var = strtok(NULL, " ");
            int varLoc = getOrInsertDataLocation(var[0], 'v');
            
            smlMemory[instructionCounter] = 1100 + varLoc; // 11 es WRITE
            instructionCounter++;
        }
        else if (strcmp(command, "goto") == 0) {
            char* targetLineStr = strtok(NULL, " ");
            int targetLine = atoi(targetLineStr);

            TableEntry* targetEntry = searchSymbol(targetLine, 'l');
            if (targetEntry != NULL) {
                smlMemory[instructionCounter] = 4000 + targetEntry->location; // Salto incondicional
            } else {
                smlMemory[instructionCounter] = 4000; // Se queda en 00 y guardamos pendiente
                flags[instructionCounter] = targetLine;
            }
            instructionCounter++;
        }
        else if (strcmp(command, "if") == 0) {
            char* var1 = strtok(NULL, " ");
            char* op = strtok(NULL, " ");
            char* var2 = strtok(NULL, " ");
            strtok(NULL, " "); // saltamos "goto"
            char* targetLineStr = strtok(NULL, " ");
            int targetLine = atoi(targetLineStr);

            int loc1 = getOrInsertDataLocation(var1[0], 'v');
            int loc2 = getOrInsertDataLocation(var2[0], 'v');

            // Restamos para comparar si son iguales
            smlMemory[instructionCounter++] = 2000 + loc1; // LOAD
            smlMemory[instructionCounter++] = 3100 + loc2; // SUBTRACT

            TableEntry* targetEntry = searchSymbol(targetLine, 'l');
            if (targetEntry != NULL) {
                smlMemory[instructionCounter] = 4200 + targetEntry->location; // Branch zero
            } else {
                smlMemory[instructionCounter] = 4200; 
                flags[instructionCounter] = targetLine; 
            }
            instructionCounter++;
        }
        else if (strcmp(command, "let") == 0) {
            char* varDest = strtok(NULL, " ");
            strtok(NULL, " "); // saltamos "="
            char* op1 = strtok(NULL, " ");
            char* operador = strtok(NULL, " ");
            char* op2 = strtok(NULL, " ");

            int destLoc = getOrInsertDataLocation(varDest[0], 'v');

            if (operador == NULL) {
                // Asignacion simple (ej. x = 5)
                int sourceVal = isdigit(op1[0]) ? atoi(op1) : op1[0];
                char sourceType = isdigit(op1[0]) ? 'c' : 'v';
                int sourceLoc = getOrInsertDataLocation(sourceVal, sourceType);

                smlMemory[instructionCounter++] = 2000 + sourceLoc;
                smlMemory[instructionCounter++] = 2100 + destLoc;
            } else {
                // Operacion con dos numeros o variables (ej. x + y)
                int val1 = isdigit(op1[0]) ? atoi(op1) : op1[0];
                char type1 = isdigit(op1[0]) ? 'c' : 'v';
                int loc1 = getOrInsertDataLocation(val1, type1);

                int val2 = isdigit(op2[0]) ? atoi(op2) : op2[0];
                char type2 = isdigit(op2[0]) ? 'c' : 'v';
                int loc2 = getOrInsertDataLocation(val2, type2);

                smlMemory[instructionCounter++] = 2000 + loc1;

                if (strcmp(operador, "+") == 0) {
                    smlMemory[instructionCounter++] = 3000 + loc2;
                } else if (strcmp(operador, "-") == 0) {
                    smlMemory[instructionCounter++] = 3100 + loc2;
                } else if (strcmp(operador, "*") == 0) {
                    smlMemory[instructionCounter++] = 3300 + loc2;
                } else if (strcmp(operador, "/") == 0) {
                    smlMemory[instructionCounter++] = 3200 + loc2;
                }

                smlMemory[instructionCounter++] = 2100 + destLoc;
            }
        }
        else if (strcmp(command, "end") == 0) {
            smlMemory[instructionCounter] = 4300; // 43 es HALT
            instructionCounter++;
        }
    }
}

// SEGUNDA PASADA: Resuelve saltos y guarda el archivo dentro de la carpeta sml/
void segundaPasada(const char* inputFilename) {
    // Buscamos si quedaron saltos pendientes y los rellenamos
    for (int i = 0; i < MEM_SIZE; i++) {
        if (flags[i] != -1) {
            int targetLineNumber = flags[i];
            
            TableEntry* entry = searchSymbol(targetLineNumber, 'l');
            if (entry != NULL) {
                int baseOpCode = smlMemory[i] / 100; 
                smlMemory[i] = (baseOpCode * 100) + entry->location;
            } else {
                printf("Error: la linea %d no existe.\n", targetLineNumber);
            }
        }
    }

    // Extraemos solo el nombre del archivo sin la ruta previa (ej. programa1.simple)
    const char *filename = strrchr(inputFilename, '/');
    if (filename == NULL) filename = strrchr(inputFilename, '\\');
    if (filename == NULL) {
        filename = inputFilename;
    } else {
        filename++; // Saltamos el slash para quedarnos solo con el nombre
    }

    // Construimos la ruta de salida apuntando a la carpeta sml/
    char outputFilename[256];
    snprintf(outputFilename, sizeof(outputFilename), "sml/%s", filename);

    // Cambiamos automaticamente la extension .simple por .sml
    char *dot = strrchr(outputFilename, '.');
    if (dot != NULL) {
        strcpy(dot, ".sml");
    } else {
        strcat(outputFilename, ".sml");
    }

    // Guardamos todo en el archivo de salida dentro de la carpeta sml/
    FILE* outFile = fopen(outputFilename, "w");
    if (outFile == NULL) {
        printf("Error: No se pudo crear el archivo en la carpeta 'sml/'. Asegurate de que la carpeta exista.\n");
        return;
    }

    for (int i = 0; i < MEM_SIZE; i++) {
        fprintf(outFile, "%+05d\n", smlMemory[i]);
    }

    fclose(outFile);
    printf("\n*** Compilacion lista. Archivo SML generado: '%s' ***\n", outputFilename);
}

// FUNCION PRINCIPAL: Recibe cualquier archivo por argumentos
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso correcto: %s <ruta_o_nombre_archivo.simple>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error: No se pudo abrir el archivo '%s'\n", argv[1]);
        return 1;
    }

    printf("-> Ejecutando primera pasada para: %s\n", argv[1]);
    primeraPasada(file);
    fclose(file);

    printf("-> Ejecutando segunda pasada...\n");
    segundaPasada(argv[1]);

    return 0;
}