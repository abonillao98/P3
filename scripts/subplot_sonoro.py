import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile as wav

# === Parámetros ===
filename = 'prueba.wav'
frame_duration = 0.030  # 30 ms
frame_start_time = 0.770  # Es donde empieza una parte de señal sonoro (analizado con wavesurfer)

# === Leer WAV ===
rate, data = wav.read(filename)
if data.ndim > 1:
    data = data[:, 0]  # convertir a mono si es estéreo

# === Extraer segmento de 30 ms ===
start_sample = int(frame_start_time * rate)
frame_len = int(frame_duration * rate)
frame = data[start_sample:start_sample + frame_len]
frame = frame.astype(np.float32)
frame = frame - np.mean(frame)  # quitar DC

# === Calcular autocorrelación sesgada ===
def autocorrelation_sesgada(x):
    N = len(x)
    r = np.array([
        np.sum(x[:N - l] * x[l:N]) / N
        for l in range(N)
    ])
    return r

r = autocorrelation_sesgada(frame)

# === Buscar máximo secundario (lag != 0) ===
r[0] = 1e-10  # evitar que se detecte el máximo en el origen
lag_min = int(rate / 500)  # pitch máximo: 500 Hz
lag_max = int(rate / 50)   # pitch mínimo: 50 Hz

lag_range = r[lag_min:lag_max]
lag = np.argmax(lag_range) + lag_min
pitch_period = lag
pitch_freq = rate / pitch_period

# === Plot ===
t = np.arange(frame_len) / rate * 1000  # eje temporal en ms
lags = np.arange(len(r))

plt.figure(figsize=(10, 6))

# Subplot 1: señal temporal
plt.subplot(2, 1, 1)
plt.plot(t, frame)
plt.title(f"Señal temporal (30 ms) extraída del fichero {filename} — Pitch ≈ {pitch_freq:.2f} Hz")
plt.xlabel("Tiempo [ms]")
plt.ylabel("Amplitud")
plt.axvline(x=pitch_period / rate * 1000, color='r', linestyle='--', label='Periodo de pitch')
plt.legend()

# Subplot 2: autocorrelación
plt.subplot(2, 1, 2)
plt.plot(lags, r)
plt.title("Autocorrelación sesgada")
plt.xlabel("Lag [muestras]")
plt.ylabel("r[lag]")
plt.axvline(x=lag, color='r', linestyle='--', label='Primer máximo secundario')
plt.legend()

plt.tight_layout()
plt.show()