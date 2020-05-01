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
// Settings storage.

#ifndef STAGES_SETTINGS_H_
#define STAGES_SETTINGS_H_

#include "stmlib/stmlib.h"
#include "stmlib/system/storage.h"

#include "stages/io_buffer.h"

namespace stages {

enum MultiMode {
  MULTI_MODE_STAGES = 0,
  MULTI_MODE_STAGES_SLOW_LFO = 2,
  MULTI_MODE_SIX_EG = 3,
  MULTI_MODE_OUROBOROS = 1,
  MULTI_MODE_OUROBOROS_ALTERNATE = 4
};

struct ChannelCalibrationData {
  float adc_offset;
  float adc_scale;
  float dac_offset;
  float dac_scale;
  
  inline uint16_t dac_code(float level) const {
    int32_t value = level * dac_scale + dac_offset;
    CONSTRAIN(value, 0, 65531);
    return static_cast<uint16_t>(value);
  }
};

struct PersistentData {
  ChannelCalibrationData channel_calibration_data[kNumChannels];
  uint8_t padding[16];
  
  enum { tag = 0x494C4143 };  // CALI
};

// Segment configuration is 8 bits:
//  - b00000011 (0x03) -> segment type bits
//  - b00000100 (0x04) -> segment loop bit
//  - b01110000 (0x70) -> ouroboros waveshape (8 values)

struct State {
  uint8_t segment_configuration[kNumChannels];
  uint8_t color_blind;
  uint8_t multimode;
  enum { tag = 0x54415453 };  // STAT
};

class Settings {
 public:
  Settings() { }
  ~Settings() { }
  
  bool Init();

  void SavePersistentData();
  void SaveState();
  
  inline ChannelCalibrationData* mutable_calibration_data(int channel) {
    return &persistent_data_.channel_calibration_data[channel];
  }
  
  inline const ChannelCalibrationData& calibration_data(int channel) const {
    return persistent_data_.channel_calibration_data[channel];
  }
  
  inline State* mutable_state() {
    return &state_;
  }
  
  inline const State& state() const {
    return state_;
  }

  inline uint16_t dac_code(int index, float level) const {
    return calibration_data(index).dac_code(level);
  }

 private:
  PersistentData persistent_data_;
  State state_;
  
  stmlib::ChunkStorage<
      0x08004000,
      0x08008000,
      PersistentData,
      State> chunk_storage_;
  
  DISALLOW_COPY_AND_ASSIGN(Settings);
};

}  // namespace stages

#endif  // STAGES_SETTINGS_H_
