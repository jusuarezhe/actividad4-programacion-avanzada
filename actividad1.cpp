#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cmath>
#include <algorithm>

using namespace std;

// estructua para representar el estado del tablero
// (Juan hizo esta parte)
struct Estado {
    vector<vector<int>> tablero;
    int x, y; // posision del espacio vacio
    int g, h; // costo y euristica
    string camino; // movimientos realizados
    
    int f() const { return g + h; }
    
    // operador para la cola de prioridad
    bool operator>(const Estado& otro) const {
        return f() > otro.f();
    }
};

// funcion para calcular la distancia manhattan (heuristica)
// Maria hizo esta funcion
int calcularHeuristica(const vector<vector<int>>& tablero, const vector<vector<int>>& objetivo) {
    int dist = 0;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(tablero[i][j] != 0) {
                // buscar posicion objetivo
                for(int x = 0; x < 3; x++) {
                    for(int y = 0; y < 3; y++) {
                        if(tablero[i][j] == objetivo[x][y]) {
                            dist += abs(i - x) + abs(j - y);
                        }
                    }
                }
            }
        }
    }
    return dist;
}

// funcion para convertir el estado a string (para el set)
string estadoAString(const vector<vector<int>>& tablero) {
    string s = "";
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            s += to_string(tablero[i][j]);
        }
    }
    return s;
}

// imprimir tablero
// Pedro hizo esto
void imprimirTablero(const vector<vector<int>>& tablero) {
    cout << "-------------" << endl;
    for(int i = 0; i < 3; i++) {
        cout << "| ";
        for(int j = 0; j < 3; j++) {
            if(tablero[i][j] == 0)
                cout << "  | ";
            else
                cout << tablero[i][j] << " | ";
        }
        cout << endl << "-------------" << endl;
    }
}

// contar inversiones para verificar solucionabilidad
// Pedro agrego esta validacion
int contarInversiones(const vector<vector<int>>& tablero) {
    vector<int> arr;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(tablero[i][j] != 0) {
                arr.push_back(tablero[i][j]);
            }
        }
    }
    
    int inversiones = 0;
    for(int i = 0; i < arr.size(); i++) {
        for(int j = i + 1; j < arr.size(); j++) {
            if(arr[i] > arr[j]) inversiones++;
        }
    }
    
    return inversiones;
}

bool puedenConectarse(const vector<vector<int>>& inicial, const vector<vector<int>>& objetivo) {
    int inv1 = contarInversiones(inicial);
    int inv2 = contarInversiones(objetivo);
    // ambas deben tener la misma paridad (ambas pares o ambas impares)
    return (inv1 % 2) == (inv2 % 2);
}

// verificar si dos tableros son iguales
bool sonIguales(const vector<vector<int>>& a, const vector<vector<int>>& b) {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(a[i][j] != b[i][j]) return false;
        }
    }
    return true;
}

// funcion para resolver el puzzle usando A*
// todos trabajamos en esta parte
vector<string> resolverPuzzle(vector<vector<int>>& inicial, vector<vector<int>>& objetivo) {
    
    priority_queue<Estado, vector<Estado>, greater<Estado>> pq;
    set<string> visitados;
    
    // encontrar posicion inicial del espacio vacio
    int x0, y0;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(inicial[i][j] == 0) {
                x0 = i;
                y0 = j;
            }
        }
    }
    
    Estado estadoInicial;
    estadoInicial.tablero = inicial;
    estadoInicial.x = x0;
    estadoInicial.y = y0;
    estadoInicial.g = 0;
    estadoInicial.h = calcularHeuristica(inicial, objetivo);
    estadoInicial.camino = "";
    
    pq.push(estadoInicial);
    
    // direcciones: arriba, abajo, izquierda, derecha
    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};
    string movimientos[] = {"ARRIBA", "ABAJO", "IZQUIERDA", "DERECHA"};
    
    int iteraciones = 0;
    int maxIteraciones = 200000;
    
    while(!pq.empty() && iteraciones < maxIteraciones) {
        iteraciones++;
        Estado actual = pq.top();
        pq.pop();
        
        // verificar si llegamos al objetivo
        if(sonIguales(actual.tablero, objetivo)) {
            vector<string> resultado;
            resultado.push_back(actual.camino);
            return resultado;
        }
        
        string key = estadoAString(actual.tablero);
        
        // si ya visitamos este estado, continuar
        if(visitados.count(key) > 0) continue;
        
        visitados.insert(key);
        
        // generar estados vecinos
        for(int i = 0; i < 4; i++) {
            int nx = actual.x + dx[i];
            int ny = actual.y + dy[i];
            
            // verificar limites
            if(nx >= 0 && nx < 3 && ny >= 0 && ny < 3) {
                Estado nuevo;
                nuevo.tablero = actual.tablero;
                swap(nuevo.tablero[actual.x][actual.y], nuevo.tablero[nx][ny]);
                nuevo.x = nx;
                nuevo.y = ny;
                nuevo.g = actual.g + 1;
                nuevo.h = calcularHeuristica(nuevo.tablero, objetivo);
                nuevo.camino = actual.camino + movimientos[i] + " -> ";
                
                string newKey = estadoAString(nuevo.tablero);
                if(visitados.count(newKey) == 0) {
                    pq.push(nuevo);
                }
            }
        }
    }
    
    vector<string> vacio;
    return vacio; // no se encontro solucion
}

int main() {
    cout << "=== SOLUCIONADOR DE 8-PUZZLE ===" << endl;
    cout << "Ingrese la configuracion inicial (use 0 para el espacio vacio):" << endl;
    
    vector<vector<int>> inicial(3, vector<int>(3));
    vector<vector<int>> objetivo(3, vector<int>(3));
    
    // leer configuracion inicial
    cout << "Ingrese los numeros fila por fila:" << endl;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            cin >> inicial[i][j];
        }
    }
    
    // configuracion objetivo fija (segun el problema)
    objetivo[0][0] = 1; objetivo[0][1] = 2; objetivo[0][2] = 3;
    objetivo[1][0] = 8; objetivo[1][1] = 0; objetivo[1][2] = 4;
    objetivo[2][0] = 7; objetivo[2][1] = 6; objetivo[2][2] = 5;
    
    cout << "\nEstado inicial:" << endl;
    imprimirTablero(inicial);
    
    cout << "\nEstado objetivo:" << endl;
    imprimirTablero(objetivo);
    
    // verificar si pueden conectarse (misma paridad de inversiones)
    int invInicial = contarInversiones(inicial);
    int invObjetivo = contarInversiones(objetivo);
    
    cout << "\n--- Analisis de Solucionabilidad ---" << endl;
    
    // DEBUG: mostrar secuencia sin el 0
    vector<int> seqInicial, seqObjetivo;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(inicial[i][j] != 0) seqInicial.push_back(inicial[i][j]);
            if(objetivo[i][j] != 0) seqObjetivo.push_back(objetivo[i][j]);
        }
    }
    
    cout << "Secuencia inicial (sin 0): ";
    for(int x : seqInicial) cout << x << " ";
    cout << endl;
    
    cout << "Secuencia objetivo (sin 0): ";
    for(int x : seqObjetivo) cout << x << " ";
    cout << endl;
    
    cout << "Inversiones en estado inicial: " << invInicial << " (" << (invInicial % 2 == 0 ? "PAR" : "IMPAR") << ")" << endl;
    cout << "Inversiones en estado objetivo: " << invObjetivo << " (" << (invObjetivo % 2 == 0 ? "PAR" : "IMPAR") << ")" << endl;
    
    if(!puedenConectarse(inicial, objetivo)) {
        cout << "\n=== IMPOSIBLE RESOLVER ===" << endl;
        cout << "Las configuraciones tienen diferente paridad de inversiones." << endl;
        cout << "Matematicamente NO existe solucion para este puzzle." << endl;
        return 0;
    }
    
    cout << "Las configuraciones son compatibles. Buscando solucion..." << endl;
    cout << "(Esto puede tardar unos segundos...)" << endl;
    
    vector<string> solucion = resolverPuzzle(inicial, objetivo);
    
    if(solucion.empty()) {
        cout << "\n=== NO SE ENCONTRO SOLUCION ===" << endl;
        cout << "No se pudo encontrar un camino despues de 200,000 iteraciones." << endl;
    } else {
        cout << "\n=== SOLUCION ENCONTRADA ===" << endl;
        
        // contar numero de movimientos
        int numMovimientos = 0;
        for(char c : solucion[0]) {
            if(c == '>') numMovimientos++;
        }
        
        cout << "Numero de movimientos: " << numMovimientos << endl;
        cout << "\nSecuencia de movimientos:" << endl;
        if(solucion[0].empty()) {
            cout << "¡Ya esta resuelto!" << endl;
        } else {
            cout << solucion[0] << "FIN" << endl;
        }
    }
    
    return 0;
}

// TODO: optimizar la funcion de heuristica (creo que se puede mejorar)
// NOTA: funciona bien pero tarda un poco con configuraciones dificiles