// Copyright 2017 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// CV reader.

#ifndef STAGES_CV_READER_H_
#define STAGES_CV_READER_H_

#include "stages/chain_state.h"
#include "stmlib/stmlib.h"

#include "stages/drivers/pots_adc.h"
#include "stages/drivers/cv_adc.h"
#include "stages/io_buffer.h"
#include <cmath>

namespace stages {

using namespace std;

class Settings;

class CvReader {
 public:
  CvReader() { }
  ~CvReader() { }

  void Init(Settings* settings, ChainState* chain_state);
  void Read(IOBuffer::Block* block);
  void Lock(int i);
  void Unlock(int i);

  inline uint8_t raw_cv(int i) const {
    return (int32_t(cv_adc_.value(i)) + 32768) >> 8;
  }

  inline uint8_t raw_pot(int i) const {
    return pots_adc_.value(ADC_GROUP_POT, i) >> 8;
  }

  inline uint8_t raw_slider(int i) const {
    return pots_adc_.value(ADC_GROUP_SLIDER, i) >> 8;
  }

  inline float lp_cv(int i) const {
    return lp_cv_2_[i];
  }

  inline float lp_slider(int i) const {
    return lp_slider_[i];
  }

  inline float lp_pot(int i) const {
    return lp_pot_[i];
  }

  inline float locked_slider(int i) const {
    return locked_slider_[i];
  }

  inline float locked_pot(int i) const {
    return locked_pot_[i];
  }

  inline bool is_locked(int i) const {
    return locked_ >> i & 1;
  }

  inline bool pot_in_limbo(int i) const {
    return (pot_limbo_ >> i & 1);
  }

  inline bool slider_in_limbo(int i) const {
    return (slider_limbo_ >> i & 1);
  }

  inline bool update_pot_limbo(int i) {
    bool in_limbo = pot_in_limbo(i)
      && (fabs(locked_pot_[i] - lp_pot_[i]) > 0.01f);
    pot_limbo_ &= ~(!in_limbo << i);
    locked_pot_[i] += in_limbo
      * 2.0f * ((locked_pot_[i] < lp_pot_[i]) - 0.5f)
      // constant is how many seconds to take in extreme
      * kBlockSize / (1.0f * kSampleRate);
    return in_limbo;
  }

  inline bool update_slider_limbo(int i) {
    bool in_limbo = slider_in_limbo(i)
      && (fabs(locked_slider_[i] - lp_slider_[i]) > 0.01f);
    slider_limbo_ &= ~(!in_limbo << i);
    locked_slider_[i] += in_limbo
      * 2.0f * ((locked_slider_[i] < lp_slider_[i]) - 0.5f)
      // constant is how many seconds to take in extreme
      * kBlockSize / (1.0f * kSampleRate);
    return in_limbo;
  }

 private:
  Settings* settings_;
  ChainState* chain_state_;
  CvAdc cv_adc_;
  PotsAdc pots_adc_;

  float lp_cv_[kNumChannels];
  float lp_cv_2_[kNumChannels];
  float lp_slider_[kNumChannels];
  float lp_pot_[kNumChannels];

  uint8_t locked_;
  uint8_t pot_limbo_; // waiting for slider/pot to get back to true value
  uint8_t slider_limbo_;
  float locked_slider_[kNumChannels];
  float locked_pot_[kNumChannels];


  DISALLOW_COPY_AND_ASSIGN(CvReader);
};

}  // namespace stages

#endif  // STAGES_CV_READER_H_
