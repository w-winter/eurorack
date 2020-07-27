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

#include "stages/cv_reader.h"

#include <algorithm>

#include "stages/chain_state.h"
#include "stmlib/dsp/dsp.h"

#include "stages/settings.h"

namespace stages {

using namespace std;
using namespace stmlib;

void CvReader::Init(Settings* settings, ChainState* chain_state) {
  chain_state_ = chain_state;
  settings_ = settings;
  pots_adc_.Init();
  cv_adc_.Init();

  STATIC_ASSERT(kNumCvAdcChannels == kNumChannels, CV_ADC_CHANNEL_MISMATCH);
  STATIC_ASSERT(kNumAdcChannels == kNumChannels, POTS_ADC_CHANNEL_MISMATCH);

  fill(&lp_pot_[0], &lp_pot_[kNumChannels], 0.0f);
  fill(&lp_slider_[0], &lp_slider_[kNumChannels], 0.0f);
  fill(&lp_cv_[0], &lp_cv_[kNumChannels], 0.0f);
  fill(&lp_cv_2_[0], &lp_cv_2_[kNumChannels], 0.0f);
}

void CvReader::Read(IOBuffer::Block* block) {
  uint8_t pot_index = pots_adc_.pot_index();
  if (pot_index != 0xff) {
    ONE_POLE(
        lp_pot_[pot_index],
        pots_adc_.float_value(ADC_GROUP_POT, pot_index),
        0.1f);
  }

  for (size_t i = 0; i < kNumChannels; ++i) {
    const ChannelCalibrationData& c = settings_->calibration_data(i);
    ONE_POLE(lp_cv_[i], cv_adc_.float_value(i), 0.7f);
    ONE_POLE(lp_cv_2_[i], lp_cv_[i], 0.7f);

    float value = lp_cv_2_[i] * c.adc_scale + c.adc_offset;

    ONE_POLE(lp_slider_[i],
        pots_adc_.float_value(ADC_GROUP_SLIDER, i),
        0.025f);


    float slider = lp_slider_[i];
    if (settings_->state().multimode == MULTI_MODE_STAGES_SLOW_LFO) {
      const ChainState::ChannelState *state = chain_state_->local_channel(i);
      if (state->configuration().type == segment::TYPE_RAMP) {
        if (chain_state_->loop_status(i) == ChainState::LOOP_STATUS_SELF) {
          if (!state->input_patched()) {
            // Free running LFO; input is frequency
            // Base freq is 2.0439497; semitones are relative to that
            // -120 semitones is thus about 8 minutes and 120 semitones is about 2093hz=C7
            // -128 is lowest and 127 is highest
            // Original goes from -48 (~6 seconds) to 48 (C1)
            const float slider_max = 48.0f / 96.0f + 0.5; // C1
            const float slider_min = -84.0f / 96.0f + 0.5; // 1 minutes
            slider = (slider_max - slider_min) * slider + slider_min;
          }
          // Leave tap LFO the same
        } else {
          // ramp; input is time; negative values don't make sense
          // 1.0f -> 32sec
          // 2.0f -> ~19min; these seemed waaay to sensitive in practice
          // 1.25 -> ~58sec, which is about what a Maths linear stage is
          // slider = 1.25f * slider; // Felt too sensitive.
        }
      }
    }
    uint16_t seg_config = settings_->state().segment_configuration[i];
    if (is_bipolar(seg_config) && (seg_config & 0x03) != 0) {
      slider = 2.0f * slider - 1.0f;
    }
    float combined_value = value + slider;
    CONSTRAIN(combined_value, -1.0f, 1.999995f);

    block->pot[i] = lp_pot_[i];
    block->cv_slider[i] = combined_value;
  }

  pots_adc_.Convert();
  cv_adc_.Convert();
}

}  // namespace stages
