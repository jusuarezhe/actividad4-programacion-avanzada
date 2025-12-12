import pandas as pd
import matplotlib.pyplot as plt

# Leer datos
datos = pd.read_csv('resultados_hil_Rampa.txt')

# Crear graficas
fig, axs = plt.subplots(2, 1, figsize=(12, 8))

# Grafica 1: Referencia y seguimiento
axs[0].plot(datos['Tiempo'], datos['Referencia'], 'b--', label='Referencia', linewidth=2)
axs[0].plot(datos['Tiempo'], datos['z0_Seguimiento'], 'r-', label='z0 (Seguimiento)', linewidth=1.5)
axs[0].plot(datos['Tiempo'], datos['Salida_Planta'], 'g-', label='Salida Planta', linewidth=1.5)
axs[0].set_xlabel('Tiempo (s)')
axs[0].set_ylabel('Amplitud')
axs[0].set_title('Seguimiento de Referencia')
axs[0].legend()
axs[0].grid(True)

# Grafica 2: Derivadas
axs[1].plot(datos['Tiempo'], datos['z1_Derivada1'], label='z1 (1ra derivada)')
axs[1].plot(datos['Tiempo'], datos['z2_Derivada2'], label='z2 (2da derivada)')
axs[1].plot(datos['Tiempo'], datos['z3_Derivada3'], label='z3 (3ra derivada)')
axs[1].set_xlabel('Tiempo (s)')
axs[1].set_ylabel('Derivadas')
axs[1].set_title('Derivadas de Orden Superior')
axs[1].legend()
axs[1].grid(True)

plt.tight_layout()
plt.savefig('resultados_hil.png', dpi=300)
plt.show()
