import numpy as np
import matplotlib.pyplot as plt

# === Cargar pitch de ambos sistemas ===
f_pitch_own = "prueba.f0"
f_pitch_ws = "prueba_wavesurfer-pitch-estimator.txt"

# Cargar datos
pitch_own = np.loadtxt(f_pitch_own)
pitch_ws = np.loadtxt(f_pitch_ws, usecols=0)

# Asegurar que tienen el mismo número de frames
n = min(len(pitch_own), len(pitch_ws))
frames = np.arange(n)

pitch_own = pitch_own[:n]
pitch_ws = pitch_ws[:n]

# === Dibujar ===
plt.figure(figsize=(12, 5))
plt.plot(frames, pitch_own, label="Estimador propio (get_pitch)", color='blue')
plt.plot(frames, pitch_ws, label="Estimador WaveSurfer", color='orange', linestyle='--')
plt.title("Comparación entre estimador 'get_pitch' y WaveSurfer")
plt.xlabel("Número de trama")
plt.ylabel("Pitch [Hz]")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
