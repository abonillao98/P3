/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <cmath>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    --window=<W>   Window type: RECT or HAMMING [default: RECT]
    --pot-th=<T>   Power threshold [default: 10.0]
    --r1-th=<T>   r1/r0 threshold [default: 0.4]
    --rmax-th=<T>   r[lag]/r0 threshold [default: 0.6]

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  std::string window_type = args["--window"].asString();
  float pot_threshold = std::stof(args["--pot-th"].asString());
  float r1_threshold = std::stof(args["--r1-th"].asString());
  float rmax_threshold = std::stof(args["--rmax-th"].asString());

  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  // Define analyzer
  PitchAnalyzer::Window win = (window_type == "HAMMING") ?
    PitchAnalyzer::HAMMING : PitchAnalyzer::RECT;

  //Chivato ventanas
  #if 1
  std::cout << "La ventana es: "
          << (win == PitchAnalyzer::HAMMING ? "HAMMING" : "RECT")
          << std::endl;
  #endif

  PitchAnalyzer analyzer(n_len, rate, win, 50, 500);

  //Chivato threhsolds:
  #if 1
  std::cout << "Umbrales configurados:\n";
    std::cout << "  POT_THRESHOLD = " << pot_threshold << '\n';
    std::cout << "  R1NORM_THRESHOLD = " << r1_threshold << '\n';
    std::cout << "  RMAXNORM_THRESHOLD = " << rmax_threshold << '\n';
  #endif

  analyzer.set_thresholds(pot_threshold, r1_threshold, rmax_threshold);
  
/*
  // Center clipping: recorte a largo plazo
float max_abs = 0.0F;
for (float v : x)
    if (std::fabs(v) > max_abs) max_abs = fabs(v);

// Umbral de clipping: 30% del máximo absoluto
float clip_threshold = 0.05F * max_abs;

// Aplicar center clipping
for (float &v : x) {
    if (v > clip_threshold)
        v -= clip_threshold;
    else if (v < -clip_threshold)
        v += clip_threshold;
    else
        v = 0.0F;
}*/

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.  
  // Iterate for each frame and save values in f0 vector
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len);
    f0.push_back(f);
    
    
    // Postprocesado: filtro de mediana de longitud 3
    vector<float> f0_filtered(f0.size());

    for (size_t i = 0; i < f0.size(); ++i) {
        if (i == 0 || i == f0.size() - 1) {
            f0_filtered[i] = f0[i]; // no se filtra primer ni último
        } else {
            // Obtener vecindad
            float a = f0[i - 1];
            float b = f0[i];
            float c = f0[i + 1];

            // Calcular mediana directamente
            if ((a <= b && b <= c) || (c <= b && b <= a)) f0_filtered[i] = b;
            else if ((b <= a && a <= c) || (c <= a && a <= b)) f0_filtered[i] = a;
            else f0_filtered[i] = c;
        }
    }

    // Sustituir f0 original por la filtrada
    f0 = f0_filtered;
    
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
