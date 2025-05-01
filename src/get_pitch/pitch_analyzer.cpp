/// @file

#include <iostream>
#include <math.h>
#include "pitch_analyzer.h"

using namespace std;

/// Name space of UPC
namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
    /// \TODO Compute the autocorrelation r[l] 
      /// Para cada TODO en el codigo, añadir comando
      /// \FET Hemos hecho la autocorrelacion sesgada
      /// \f[
      /// r_{xx}[m]=\frac{1}{N} \sum_{n=0}^{N-m} x[n] x[n+m]
      /// \f]
    
    unsigned int N = x.size();  // Número de muestras en la ventana
    
    for (unsigned int l = 0; l < r.size(); ++l) {
        float sum = 0.0F;
        
        for (unsigned int n = 0; n < N - l; ++n) {
            sum += x[n] * x[n + l];
        }
        
        r[l] = sum / (float)N;  // Dividimos por N para autocorrelación sesgada
    }

    if (r[0] == 0.0F) // Para evitar problemas con divisiones por cero o log()
        r[0] = 1e-10;
}

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      /// \TODO Implement the Hamming window
      /// \FET Implementación de la ventana de Hamming
    for (unsigned int n = 0; n < frameLen; ++n) {
      window[n] = 0.54F - 0.46F * cos(2.0F * M_PI * n / (frameLen - 1));
    }
    
      break;
    case RECT:
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  void PitchAnalyzer::set_thresholds(float pot_th, float r1_th, float rmax_th) {
    potThreshold = pot_th;
    r1normThreshold = r1_th;
    rmaxnormThreshold = rmax_th;
  }

  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm) const {
    /// \FET Hemos vinculado los thresholds a los parámetros configurables por línea de comandos

    int passed = 0;

    if (pot > potThreshold) passed++;
    if (r1norm > r1normThreshold) passed++;
    if (rmaxnorm > rmaxnormThreshold) passed++;

    return (passed < 2);
}

  float PitchAnalyzer::compute_pitch(vector<float> & x) const {
    if (x.size() != frameLen)
      return -1.0F;

    //Window input frame
    for (unsigned int i=0; i<x.size(); ++i)
      x[i] *= window[i];

    vector<float> r(npitch_max);

    //Compute correlation
    autocorrelation(x, r);

    vector<float>::const_iterator iR = r.begin();

    /// \TODO
	/// Find the lag of the maximum value of the autocorrelation away from the origin.<br>
	/// Choices to set the minimum value of the lag are:
	///    - The first negative value of the autocorrelation.
	///    - The lag corresponding to the maximum value of the pitch.
    ///	   .
	/// In either case, the lag should not exceed that of the minimum value of the pitch.
      /// \FET Hemos implementado la búsqueda del lag máximo usando iteradores
      vector<float>::const_iterator iRStart = r.begin() + npitch_min;
      vector<float>::const_iterator iREnd = r.end();
      
      vector<float>::const_iterator iRMax = max_element(iRStart, iREnd);

    unsigned int lag = iRMax - r.begin();

    float pot = 10 * log10(r[0]);

    //You can print these (and other) features, look at them using wavesurfer
    //Based on that, implement a rule for unvoiced
    //change to #if 1 and compile
#if 0
    if (r[0] > 0.0F)
      cout << pot << '\t' << r[1]/r[0] << '\t' << r[lag]/r[0] << endl;
#endif
    
    if (unvoiced(pot, r[1]/r[0], r[lag]/r[0]))
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }

  
}
