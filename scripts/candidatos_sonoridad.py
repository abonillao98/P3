import numpy as np
import matplotlib.pyplot as plt

# === Cargar datos ===
f_pitch = "prueba_wavesurfer-pitch-estimator.txt"
f_pot = "prueba_pot.txt"
f_r1norm = "prueba_r1norm.txt"
f_rmaxnorm = "prueba_rmaxnorm.txt"

# Leer pitch (columna 0)
pitch_data = np.loadtxt(f_pitch, usecols=0)

# Leer características
pot = np.loadtxt(f_pot)
r1norm = np.loadtxt(f_r1norm)
rmaxnorm = np.loadtxt(f_rmaxnorm)

# Asegurar que todos tienen el mismo número de frames
n_frames = min(len(pitch_data), len(pot), len(r1norm), len(rmaxnorm))
frames = np.arange(n_frames)

pitch_data = pitch_data[:n_frames]
pot = pot[:n_frames]
r1norm = r1norm[:n_frames]
rmaxnorm = rmaxnorm[:n_frames]

# === Dibujar ===
fig, axs = plt.subplots(4, 1, figsize=(12, 10), sharex=True)

# 1. Pitch
axs[0].plot(frames, pitch_data, label='Pitch (Hz)', color='purple')
axs[0].set_ylabel("Pitch [Hz]")
axs[0].set_title("Contorno de pitch estimado con WaveSurfer")
axs[0].grid(True)
axs[0].legend()

# 2. Potencia
axs[1].plot(frames, pot, label='Potencia (r[0]) [dB]', color='black')
axs[1].set_ylabel("r[0] [dB]")
axs[1].set_title("Nivel de potencia de la señal")
axs[1].grid(True)
axs[1].legend()

# 3. r1norm
axs[2].plot(frames, r1norm, label='r[1]/r[0]', color='blue')
axs[2].set_ylabel("r1norm")
axs[2].set_title("Autocorrelación normalizada de uno (r1norm)")
axs[2].grid(True)
axs[2].legend()

# 4. rmaxnorm
axs[3].plot(frames, rmaxnorm, label='r[lag]/r[0]', color='green')
axs[3].set_ylabel("rmaxnorm")
axs[3].set_title("Autocorrelación en su máximo secundario (rmaxnorm)")
axs[3].set_xlabel("Número de trama")
axs[3].grid(True)
axs[3].legend()

plt.tight_layout()
plt.show()
