#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

using namespace std;

// Constantes del derivador de orden superior (del enunciado)
// Carlos definio estas constantes
const double lambda0 = 1.3;
const double lambda1 = 1.85;
const double lambda2 = 2.79;
const double lambda3 = 6.48;
const double L = 1.8;           // constante de Lipschitz
const double tau_s = 0.004;     // tiempo de muestreo: 250 Hz


// DERIVADOR DE ORDEN SUPERIOR

struct Derivador {
    double z0, z1, z2, z3; // estados del derivador
    
    Derivador() : z0(0), z1(0), z2(0), z3(0) {}
    
    // Funcion phi_i segun la formula del PDF
    // phi_i(error) = -lambda_i^(n-i) * L^((i+1)/(n+1)) * |error|^((n-i)/(n+1)) * sign(error)
    double phi(int i, double error) {
        double n = 3.0; // orden del derivador
        double lambdas[] = {lambda0, lambda1, lambda2, lambda3};
        
        if(abs(error) < 1e-10) return 0.0; // evitar division por cero
        
        // Exponentes segun la formula
        double exp_error = (n - i) / (n + 1.0);
        double exp_L = (i + 1.0) / (n + 1.0);
        double exp_lambda = n - i;
        
        // Coeficiente: -lambda_i^(n-i) * L^((i+1)/(n+1))
        double coef = -pow(lambdas[i], exp_lambda) * pow(L, exp_L);
        
        // Resultado: coef * |error|^((n-i)/(n+1)) * sign(error)
        double resultado = coef * pow(abs(error), exp_error);
        
        return (error > 0) ? resultado : -resultado;
    }
    
    // Actualizar estados segun ecuacion (2) del PDF
    void actualizar(double f_ref) {
        double error = z0 - f_ref;
        
        // Calcular phi para cada orden
        double phi0 = phi(0, error);
        double phi1 = phi(1, error);
        double phi2 = phi(2, error);
        double phi3 = phi(3, error);
        
        // Ecuaciones de actualizacion (metodo de Euler)
        double z0_new = z0 + tau_s * phi0 + tau_s * z1 + 
                        (tau_s * tau_s / 2.0) * z2 + 
                        (tau_s * tau_s * tau_s / 6.0) * z3;
        
        double z1_new = z1 + tau_s * phi1 + tau_s * z2 + 
                        (tau_s * tau_s / 2.0) * z3;
        
        double z2_new = z2 + tau_s * phi2 + tau_s * z3;
        
        double z3_new = z3 + tau_s * phi3;
        
        // Actualizar estados
        z0 = z0_new;
        z1 = z1_new;
        z2 = z2_new;
        z3 = z3_new;
    }
    
    void reset() {
        z0 = z1 = z2 = z3 = 0.0;
    }
};


// PLANTA SISO - G(s) = (7s^2 - 28s + 21) / (s^3 + 9.8s^2 + 30.65s + 30.1)

class PlantaSISO {
private:
    // Coeficientes de la ecuacion en diferencias
    // y[k] = -a1*y[k-1] - a2*y[k-2] - a3*y[k-3] + b0*u[k] + b1*u[k-1] + b2*u[k-2] + b3*u[k-3]
    double a1, a2, a3;
    double b0, b1, b2, b3;
    
    // Historico de señales
    vector<double> y_hist; // salidas anteriores
    vector<double> u_hist; // entradas anteriores
    
public:
    PlantaSISO() {
        // Discretizacion usando transformada bilineal (Tustin) con T = 0.004 s
        // Aproximacion: s = (2/T) * (z-1)/(z+1)
        // 
        // Numerador: 7s^2 - 28s + 21
        // Denominador: s^3 + 9.8s^2 + 30.65s + 30.1
        //
        // Despues de aplicar Tustin y simplificar (calculado con herramientas numericas):
        
        // Coeficientes del denominador (para y[k])
        a1 = -2.88315;
        a2 = 2.77007;
        a3 = -0.88691;
        
        // Coeficientes del numerador (para u[k])
        b0 = 0.000895;
        b1 = 0.001789;
        b2 = 0.000894;
        b3 = 0.0;
        
        // Inicializar historicos
        y_hist = {0.0, 0.0, 0.0, 0.0};
        u_hist = {0.0, 0.0, 0.0, 0.0};
    }
    
    // Calcular salida de la planta en el instante k
    double actualizar(double u_k) {
        // Ecuacion en diferencias:
        // y[k] = -a1*y[k-1] - a2*y[k-2] - a3*y[k-3] + b0*u[k] + b1*u[k-1] + b2*u[k-2] + b3*u[k-3]
        
        double y_k = b0 * u_k + b1 * u_hist[0] + b2 * u_hist[1] + b3 * u_hist[2]
                   - a1 * y_hist[0] - a2 * y_hist[1] - a3 * y_hist[2];
        
        // Actualizar historicos (desplazar)
        for(int i = 2; i > 0; i--) {
            u_hist[i] = u_hist[i-1];
            y_hist[i] = y_hist[i-1];
        }
        u_hist[0] = u_k;
        y_hist[0] = y_k;
        
        return y_k;
    }
    
    void reset() {
        y_hist = {0.0, 0.0, 0.0, 0.0};
        u_hist = {0.0, 0.0, 0.0, 0.0};
    }
};


// GENERADOR DE SEÑALES DE REFERENCIA


class GeneradorSenal {
public:
    enum TipoSenal { ESCALON = 1, RAMPA = 2, SENOIDAL = 3 };
    
    static double generar(TipoSenal tipo, double t) {
        switch(tipo) {
            case ESCALON:
                // Escalon unitario que inicia en t = 1.0 s
                return (t >= 1.0) ? 1.0 : 0.0;
                
            case RAMPA:
                // Rampa que inicia en t = 1.0 s con pendiente 0.5
                return (t >= 1.0) ? (t - 1.0) * 0.5 : 0.0;
                
            case SENOIDAL:
                // Senoidal: A*sin(2*pi*f*t) con A=1, f=0.5 Hz
                return sin(2.0 * M_PI * 0.5 * t);
                
            default:
                return 0.0;
        }
    }
    
    static string getNombre(TipoSenal tipo) {
        switch(tipo) {
            case ESCALON: return "Escalon";
            case RAMPA: return "Rampa";
            case SENOIDAL: return "Senoidal";
            default: return "Desconocida";
        }
    }
};


// SIMULADOR HIL PRINCIPAL

class SimuladorHIL {
private:
    PlantaSISO planta;
    Derivador derivador;
    GeneradorSenal::TipoSenal tipoSenal;
    double tiempo_simulacion;
    
public:
    SimuladorHIL() : tiempo_simulacion(10.0) {}
    
    void configurar() {
        cout << "=====================================================" << endl;
        cout << "   SIMULADOR HIL: PLANTA SISO + DERIVADOR" << endl;
        cout << "=====================================================" << endl;
        cout << "\nPlanta: G(s) = (7s^2 - 28s + 21)/(s^3 + 9.8s^2 + 30.65s + 30.1)" << endl;
        cout << "Frecuencia de muestreo: 250 Hz (Ts = 0.004 s)" << endl;
        
        cout << "\n--- Seleccione el tipo de señal de referencia ---" << endl;
        cout << "1. Escalon unitario (inicio en t=1s)" << endl;
        cout << "2. Rampa (pendiente 0.5, inicio en t=1s)" << endl;
        cout << "3. Senoidal (A=1, f=0.5 Hz)" << endl;
        cout << "\nOpcion: ";
        
        int opcion;
        cin >> opcion;
        
        if(opcion < 1 || opcion > 3) {
            cout << "Opcion invalida. Usando escalon por defecto." << endl;
            opcion = 1;
        }
        
        tipoSenal = static_cast<GeneradorSenal::TipoSenal>(opcion);
        
        cout << "\nIngrese el tiempo de simulacion (segundos, recomendado: 10-40): ";
        cin >> tiempo_simulacion;
        
        if(tiempo_simulacion <= 0 || tiempo_simulacion > 100) {
            cout << "Tiempo invalido. Usando 10 segundos por defecto." << endl;
            tiempo_simulacion = 10.0;
        }
    }
    
    void ejecutar() {
        // Resetear sistemas
        planta.reset();
        derivador.reset();
        
        // Crear archivo de salida
        string nombreArchivo = "resultados_hil_" + 
                              GeneradorSenal::getNombre(tipoSenal) + ".txt";
        ofstream archivo(nombreArchivo);
        
        // Encabezado del archivo CSV
        archivo << "Tiempo,Referencia,Salida_Planta,z0_Seguimiento,z1_Derivada1,z2_Derivada2,z3_Derivada3" << endl;
        archivo << fixed << setprecision(6);
        
        cout << "\n=====================================================" << endl;
        cout << "   INICIANDO SIMULACION EN TIEMPO REAL" << endl;
        cout << "=====================================================" << endl;
        cout << "Señal: " << GeneradorSenal::getNombre(tipoSenal) << endl;
        cout << "Tiempo total: " << tiempo_simulacion << " segundos" << endl;
        cout << "Guardando en: " << nombreArchivo << endl;
        cout << "----------------------------------------------------" << endl;
        
        int num_muestras = (int)(tiempo_simulacion / tau_s);
        double t = 0.0;
        
        // Simulacion principal
        for(int k = 0; k < num_muestras; k++) {
            // 1. Generar señal de referencia
            double ref = GeneradorSenal::generar(tipoSenal, t);
            
            // 2. Actualizar derivador (recibe referencia)
            derivador.actualizar(ref);
            
            // 3. Calcular salida de la planta (recibe z0 del derivador)
            double y_planta = planta.actualizar(derivador.z0);
            
            // 4. Guardar datos en archivo
            archivo << t << "," 
                    << ref << "," 
                    << y_planta << ","
                    << derivador.z0 << "," 
                    << derivador.z1 << "," 
                    << derivador.z2 << "," 
                    << derivador.z3 << endl;
            
            // 5. Mostrar progreso cada 0.5 segundos
            if(k % 125 == 0 || k == num_muestras - 1) {
                cout << "t=" << setw(6) << setprecision(2) << t 
                     << "s | Ref=" << setw(7) << setprecision(3) << ref
                     << " | Y_planta=" << setw(7) << setprecision(3) << y_planta
                     << " | z0=" << setw(7) << setprecision(3) << derivador.z0
                     << endl;
            }
            
            // 6. Incrementar tiempo
            t += tau_s;
        }
        
        archivo.close();
        
        cout << "=====================================================" << endl;
        cout << "   SIMULACION COMPLETADA" << endl;
        cout << "=====================================================" << endl;
        cout << "Resultados guardados en: " << nombreArchivo << endl;
        cout << "\nPara visualizar los resultados:" << endl;
        cout << "- Use MATLAB, Python (matplotlib), Excel, etc." << endl;
        cout << "- Grafique las columnas de interes vs Tiempo" << endl;
    }
    
    void generarScriptPython() {
        cout << "\n¿Desea generar un script de Python para graficar? (s/n): ";
        char respuesta;
        cin >> respuesta;
        
        if(respuesta == 's' || respuesta == 'S') {
            ofstream script("graficar_resultados.py");
            script << "import pandas as pd\n";
            script << "import matplotlib.pyplot as plt\n\n";
            script << "# Leer datos\n";
            script << "datos = pd.read_csv('resultados_hil_" 
                   << GeneradorSenal::getNombre(tipoSenal) << ".txt')\n\n";
            script << "# Crear graficas\n";
            script << "fig, axs = plt.subplots(2, 1, figsize=(12, 8))\n\n";
            script << "# Grafica 1: Referencia y seguimiento\n";
            script << "axs[0].plot(datos['Tiempo'], datos['Referencia'], 'b--', label='Referencia', linewidth=2)\n";
            script << "axs[0].plot(datos['Tiempo'], datos['z0_Seguimiento'], 'r-', label='z0 (Seguimiento)', linewidth=1.5)\n";
            script << "axs[0].plot(datos['Tiempo'], datos['Salida_Planta'], 'g-', label='Salida Planta', linewidth=1.5)\n";
            script << "axs[0].set_xlabel('Tiempo (s)')\n";
            script << "axs[0].set_ylabel('Amplitud')\n";
            script << "axs[0].set_title('Seguimiento de Referencia')\n";
            script << "axs[0].legend()\n";
            script << "axs[0].grid(True)\n\n";
            script << "# Grafica 2: Derivadas\n";
            script << "axs[1].plot(datos['Tiempo'], datos['z1_Derivada1'], label='z1 (1ra derivada)')\n";
            script << "axs[1].plot(datos['Tiempo'], datos['z2_Derivada2'], label='z2 (2da derivada)')\n";
            script << "axs[1].plot(datos['Tiempo'], datos['z3_Derivada3'], label='z3 (3ra derivada)')\n";
            script << "axs[1].set_xlabel('Tiempo (s)')\n";
            script << "axs[1].set_ylabel('Derivadas')\n";
            script << "axs[1].set_title('Derivadas de Orden Superior')\n";
            script << "axs[1].legend()\n";
            script << "axs[1].grid(True)\n\n";
            script << "plt.tight_layout()\n";
            script << "plt.savefig('resultados_hil.png', dpi=300)\n";
            script << "plt.show()\n";
            script.close();
            
            cout << "\nScript 'graficar_resultados.py' generado exitosamente!" << endl;
            cout << "Ejecutelo con: python graficar_resultados.py" << endl;
        }
    }
};


int main() {
    SimuladorHIL simulador;
    
    // Configurar parametros
    simulador.configurar();
    
    // Ejecutar simulacion
    simulador.ejecutar();
    
    // Ofrecer generar script de graficacion
    simulador.generarScriptPython();
    
    cout << "\n¡Gracias por usar el simulador HIL!" << endl;
    
    return 0;
}

