#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

using namespace std;

// ============================================================================
// FUNCIONES DE ACTIVACION
// Sofia implemento las funciones de activacion
// ============================================================================

double sigmoid(double x) {
    if(x < -45.0) return 0.0;
    if(x > 45.0) return 1.0;
    return 1.0 / (1.0 + exp(-x));
}

double sigmoid_derivada(double y) {
    return y * (1.0 - y);
}

// ============================================================================
// CLASE RED NEURONAL ARTIFICIAL (RNA)
// Daniel diseño la arquitectura de la red
// ============================================================================
class RedNeuronal {
private:
    int n_entrada;
    int n_oculta;
    int n_salida;
    
    vector<vector<double>> pesos_entrada_oculta;
    vector<vector<double>> pesos_oculta_salida;
    vector<double> bias_oculta;
    vector<double> bias_salida;
    
    double tasa_aprendizaje;
    
    void inicializarPesos() {
        srand(static_cast<unsigned>(time(0)));
        
        double limite_eo = sqrt(6.0 / (n_entrada + n_oculta));
        for(int i = 0; i < n_entrada; i++) {
            for(int j = 0; j < n_oculta; j++) {
                pesos_entrada_oculta[i][j] = ((double)rand() / RAND_MAX) * 2.0 * limite_eo - limite_eo;
            }
        }
        
        double limite_os = sqrt(6.0 / (n_oculta + n_salida));
        for(int i = 0; i < n_oculta; i++) {
            for(int j = 0; j < n_salida; j++) {
                pesos_oculta_salida[i][j] = ((double)rand() / RAND_MAX) * 2.0 * limite_os - limite_os;
            }
        }
        
        fill(bias_oculta.begin(), bias_oculta.end(), 0.0);
        fill(bias_salida.begin(), bias_salida.end(), 0.0);
    }
    
public:
    RedNeuronal(int entrada, int oculta, int salida, double lr = 0.3) {
        n_entrada = entrada;
        n_oculta = oculta;
        n_salida = salida;
        tasa_aprendizaje = lr;
        
        pesos_entrada_oculta.resize(n_entrada, vector<double>(n_oculta));
        pesos_oculta_salida.resize(n_oculta, vector<double>(n_salida));
        bias_oculta.resize(n_oculta);
        bias_salida.resize(n_salida);
        
        inicializarPesos();
    }
    
    vector<double> forward(const vector<double>& entrada, vector<double>& activacion_oculta) {
        activacion_oculta.resize(n_oculta);
        for(int j = 0; j < n_oculta; j++) {
            double suma = bias_oculta[j];
            for(int i = 0; i < n_entrada; i++) {
                suma += entrada[i] * pesos_entrada_oculta[i][j];
            }
            activacion_oculta[j] = sigmoid(suma);
        }
        
        vector<double> activacion_salida(n_salida);
        for(int k = 0; k < n_salida; k++) {
            double suma = bias_salida[k];
            for(int j = 0; j < n_oculta; j++) {
                suma += activacion_oculta[j] * pesos_oculta_salida[j][k];
            }
            activacion_salida[k] = sigmoid(suma);
        }
        
        return activacion_salida;
    }
    
    void entrenar(const vector<double>& entrada, const vector<double>& objetivo) {
        vector<double> activacion_oculta;
        vector<double> activacion_salida = forward(entrada, activacion_oculta);
        
        vector<double> delta_salida(n_salida);
        for(int k = 0; k < n_salida; k++) {
            double error = objetivo[k] - activacion_salida[k];
            delta_salida[k] = error * sigmoid_derivada(activacion_salida[k]);
        }
        
        vector<double> delta_oculta(n_oculta);
        for(int j = 0; j < n_oculta; j++) {
            double error = 0.0;
            for(int k = 0; k < n_salida; k++) {
                error += delta_salida[k] * pesos_oculta_salida[j][k];
            }
            delta_oculta[j] = error * sigmoid_derivada(activacion_oculta[j]);
        }
        
        for(int j = 0; j < n_oculta; j++) {
            for(int k = 0; k < n_salida; k++) {
                pesos_oculta_salida[j][k] += tasa_aprendizaje * delta_salida[k] * activacion_oculta[j];
            }
        }
        
        for(int k = 0; k < n_salida; k++) {
            bias_salida[k] += tasa_aprendizaje * delta_salida[k];
        }
        
        for(int i = 0; i < n_entrada; i++) {
            for(int j = 0; j < n_oculta; j++) {
                pesos_entrada_oculta[i][j] += tasa_aprendizaje * delta_oculta[j] * entrada[i];
            }
        }
        
        for(int j = 0; j < n_oculta; j++) {
            bias_oculta[j] += tasa_aprendizaje * delta_oculta[j];
        }
    }
    
    vector<double> predecir(const vector<double>& entrada) {
        vector<double> activacion_oculta;
        return forward(entrada, activacion_oculta);
    }
    
    double calcularError(const vector<vector<double>>& entradas, const vector<vector<double>>& objetivos) {
        double error_total = 0.0;
        for(size_t i = 0; i < entradas.size(); i++) {
            vector<double> salida = predecir(entradas[i]);
            for(size_t k = 0; k < salida.size(); k++) {
                double diff = objetivos[i][k] - salida[k];
                error_total += diff * diff;
            }
        }
        return error_total / (entradas.size() * n_salida);
    }
};

// ============================================================================
// PATRONES DE DIGITOS (7 filas x 5 columnas = 35 pixeles)
// Camila creo los patrones de entrenamiento
// ============================================================================

// Cada digito es una matriz 7x5 aplanada a vector de 35 elementos
vector<vector<double>> obtenerPatronesDigitos() {
    vector<vector<double>> patrones(10);
    
    // Digito 0
    patrones[0] = {
        0,1,1,1,0,
        1,0,0,0,1,
        1,0,0,1,1,
        1,0,1,0,1,
        1,1,0,0,1,
        1,0,0,0,1,
        0,1,1,1,0
    };
    
    // Digito 1
    patrones[1] = {
        0,0,1,0,0,
        0,1,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,1,1,1,0
    };
    
    // Digito 2
    patrones[2] = {
        0,1,1,1,0,
        1,0,0,0,1,
        0,0,0,0,1,
        0,0,0,1,0,
        0,0,1,0,0,
        0,1,0,0,0,
        1,1,1,1,1
    };
    
    // Digito 3
    patrones[3] = {
        0,1,1,1,0,
        1,0,0,0,1,
        0,0,0,0,1,
        0,0,1,1,0,
        0,0,0,0,1,
        1,0,0,0,1,
        0,1,1,1,0
    };
    
    // Digito 4
    patrones[4] = {
        0,0,0,1,0,
        0,0,1,1,0,
        0,1,0,1,0,
        1,0,0,1,0,
        1,1,1,1,1,
        0,0,0,1,0,
        0,0,0,1,0
    };
    
    // Digito 5
    patrones[5] = {
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,0,
        0,0,0,0,1,
        0,0,0,0,1,
        1,0,0,0,1,
        0,1,1,1,0
    };
    
    // Digito 6
    patrones[6] = {
        0,0,1,1,0,
        0,1,0,0,0,
        1,0,0,0,0,
        1,1,1,1,0,
        1,0,0,0,1,
        1,0,0,0,1,
        0,1,1,1,0
    };
    
    // Digito 7
    patrones[7] = {
        1,1,1,1,1,
        0,0,0,0,1,
        0,0,0,1,0,
        0,0,1,0,0,
        0,1,0,0,0,
        0,1,0,0,0,
        0,1,0,0,0
    };
    
    // Digito 8
    patrones[8] = {
        0,1,1,1,0,
        1,0,0,0,1,
        1,0,0,0,1,
        0,1,1,1,0,
        1,0,0,0,1,
        1,0,0,0,1,
        0,1,1,1,0
    };
    
    // Digito 9
    patrones[9] = {
        0,1,1,1,0,
        1,0,0,0,1,
        1,0,0,0,1,
        0,1,1,1,1,
        0,0,0,0,1,
        0,0,0,1,0,
        0,1,1,0,0
    };
    
    return patrones;
}

// ============================================================================
// CLASIFICACION Y UTILIDADES
// Roberto implemento la logica de clasificacion
// ============================================================================

vector<double> obtenerClasesObjetivo(int digito) {
    vector<double> clases(4, 0.0);
    
    // Clase 0: Pares
    if(digito % 2 == 0) clases[0] = 1.0;
    
    // Clase 1: Impares
    if(digito % 2 == 1) clases[1] = 1.0;
    
    // Clase 2: Primos (2, 3, 5, 7)
    if(digito == 2 || digito == 3 || digito == 5 || digito == 7) clases[2] = 1.0;
    
    // Clase 3: Compuestos (4, 6, 8, 9)
    if(digito == 4 || digito == 6 || digito == 8 || digito == 9) clases[3] = 1.0;
    
    return clases;
}

// Mostrar digito en forma visual
void mostrarDigito(const vector<double>& patron) {
    cout << "  +-----+" << endl;
    for(int i = 0; i < 7; i++) {
        cout << "  |";
        for(int j = 0; j < 5; j++) {
            cout << (patron[i*5 + j] > 0.5 ? "#" : " ");
        }
        cout << "|" << endl;
    }
    cout << "  +-----+" << endl;
}

void interpretarClases(const vector<double>& salida, int digito_real) {
    vector<string> nombres = {"PAR", "IMPAR", "PRIMO", "COMPUESTO"};
    
    cout << "  Clasificacion -> ";
    bool primera = true;
    for(int i = 0; i < 4; i++) {
        if(salida[i] > 0.5) {
            if(!primera) cout << ", ";
            cout << nombres[i];
            primera = false;
        }
    }
    if(primera) cout << "(ninguna)";
    cout << endl;
    
    cout << "  Confianzas: ";
    for(int i = 0; i < 4; i++) {
        cout << nombres[i] << "=" << fixed << setprecision(2) << salida[i];
        if(i < 3) cout << " | ";
    }
    cout << endl;
}

// Leer digitos desde archivo digitos.txt
// Pedro implemento la lectura del archivo
vector<vector<double>> leerDigitosDesdeArchivo(const string& archivo) {
    vector<vector<double>> digitos;
    ifstream file(archivo);
    
    if(!file.is_open()) {
        cout << "ERROR: No se pudo abrir el archivo '" << archivo << "'" << endl;
        return digitos;
    }
    
    vector<double> patron_actual;
    string linea;
    
    while(getline(file, linea)) {
        if(linea.empty()) continue;
        
        istringstream iss(linea);
        int valor;
        
        while(iss >> valor) {
            patron_actual.push_back(static_cast<double>(valor));
        }
        
        // Cada digito tiene 35 valores (7 filas x 5 columnas)
        if(patron_actual.size() == 35) {
            digitos.push_back(patron_actual);
            patron_actual.clear();
        }
    }
    
    // Si queda un patron incompleto al final
    if(patron_actual.size() == 35) {
        digitos.push_back(patron_actual);
    }
    
    file.close();
    return digitos;
}

// Reconocer digito comparando con patrones conocidos
int reconocerDigito(const vector<double>& patron, const vector<vector<double>>& patrones_ref) {
    int mejor_match = 0;
    double mejor_similitud = -1.0;
    
    for(int d = 0; d < 10; d++) {
        double similitud = 0.0;
        for(size_t i = 0; i < patron.size(); i++) {
            if(patron[i] == patrones_ref[d][i]) similitud += 1.0;
        }
        similitud /= patron.size();
        
        if(similitud > mejor_similitud) {
            mejor_similitud = similitud;
            mejor_match = d;
        }
    }
    
    return mejor_match;
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    cout << "========================================================" << endl;
    cout << "  RED NEURONAL - CLASIFICACION DE DIGITOS (7x5 pixeles)" << endl;
    cout << "========================================================" << endl;
    
    // Obtener patrones de entrenamiento
    vector<vector<double>> patrones = obtenerPatronesDigitos();
    
    cout << "\nArquitectura de la red:" << endl;
    cout << "  Entrada: 35 neuronas (7x5 pixeles)" << endl;
    cout << "  Oculta: 20 neuronas" << endl;
    cout << "  Salida: 4 neuronas (clases)" << endl;
    
    // Crear red: 35 entradas, 20 ocultas, 4 salidas
    RedNeuronal red(35, 20, 4, 0.5);
    
    // Preparar datos de entrenamiento
    vector<vector<double>> X_train = patrones;
    vector<vector<double>> Y_train;
    for(int d = 0; d <= 9; d++) {
        Y_train.push_back(obtenerClasesObjetivo(d));
    }
    
    // Entrenar la red
    cout << "\n========================================================" << endl;
    cout << "  ENTRENANDO LA RED NEURONAL" << endl;
    cout << "========================================================" << endl;
    
    int num_epocas = 10000;
    for(int epoca = 1; epoca <= num_epocas; epoca++) {
        vector<int> indices = {0,1,2,3,4,5,6,7,8,9};
        random_shuffle(indices.begin(), indices.end());
        
        for(int idx : indices) {
            red.entrenar(X_train[idx], Y_train[idx]);
        }
        
        if(epoca % 2000 == 0 || epoca == 1) {
            double error = red.calcularError(X_train, Y_train);
            cout << "Epoca " << setw(5) << epoca << " | MSE: " << fixed << setprecision(6) << error << endl;
        }
    }
    
    cout << "¡Entrenamiento completado!" << endl;
    
    // Leer y clasificar digitos del archivo
    cout << "\n========================================================" << endl;
    cout << "  LEYENDO Y CLASIFICANDO DIGITOS DESDE ARCHIVO" << endl;
    cout << "========================================================" << endl;
    
    vector<vector<double>> digitos_archivo = leerDigitosDesdeArchivo("digitos.txt");
    
    if(digitos_archivo.empty()) {
        cout << "\nNo se encontraron digitos en 'digitos.txt'" << endl;
        cout << "Probando con los patrones de entrenamiento..." << endl;
        digitos_archivo = patrones;
    } else {
        cout << "\nSe leyeron " << digitos_archivo.size() << " digitos del archivo." << endl;
    }
    
    cout << "\n--- RESULTADOS DE CLASIFICACION ---\n" << endl;
    
    for(size_t i = 0; i < digitos_archivo.size(); i++) {
        cout << "Digito #" << (i+1) << ":" << endl;
        mostrarDigito(digitos_archivo[i]);
        
        // Reconocer cual digito es
        int digito_reconocido = reconocerDigito(digitos_archivo[i], patrones);
        cout << "  Reconocido como: " << digito_reconocido << endl;
        
        // Clasificar
        vector<double> salida = red.predecir(digitos_archivo[i]);
        interpretarClases(salida, digito_reconocido);
        cout << endl;
    }
    
    cout << "========================================================" << endl;
    cout << "Proceso completado exitosamente." << endl;
    cout << "========================================================" << endl;
    
    return 0;
}

// NOTAS DEL EQUIPO:
// - Los digitos son imagenes de 7x5 pixeles (35 valores binarios)
// - La red aprende a reconocer patrones visuales
// - Usamos 20 neuronas ocultas para capturar caracteristicas
// - El entrenamiento toma ~10000 epocas para converger bien
// - TODO: agregar ruido a los patrones para mejor generalizacion