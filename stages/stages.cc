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

#include <stm32f37x_conf.h>

#include "stmlib/dsp/dsp.h"

#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/units.h"
#include "stages/drivers/dac.h"
#include "stages/drivers/gate_inputs.h"
#include "stages/drivers/leds.h"
#include "stages/drivers/serial_link.h"
#include "stages/drivers/system.h"
#include "stages/cv_reader.h"
#include "stages/io_buffer.h"
#include "stages/resources.h"
#include "stages/settings.h"
#include "stages/ui.h"

using namespace stages;
using namespace std;
using namespace stmlib;

CvReader cv_reader;
Dac dac;
GateFlags no_gate[kBlockSize];
GateInputs gate_inputs;
IOBuffer io_buffer;
Settings settings;
Ui ui;

// Default interrupt handlers.
extern "C" {
  
  void NMI_Handler() { }
  void HardFault_Handler() { while (1); }
  void MemManage_Handler() { while (1); }
  void BusFault_Handler() { while (1); }
  void UsageFault_Handler() { while (1); }
  void SVC_Handler() { }
  void DebugMon_Handler() { }
  void PendSV_Handler() { }
  
}

// SysTick and 32kHz handles
extern "C" {

  void SysTick_Handler() {
    IWDG_ReloadCounter();
    ui.Poll();
  }

}

IOBuffer::Slice FillBuffer(size_t size) {
  IOBuffer::Slice s = io_buffer.NextSlice(size);
  gate_inputs.Read(s, size);
  if (io_buffer.new_block()) {
    cv_reader.Read(s.block);
    gate_inputs.ReadNormalization(s.block);
  }
  return s;
}

void ProcessTest(IOBuffer::Block* block, size_t size) {
  
  for (size_t channel = 0; channel < kNumChannels; channel++) {
    
    // Pot position affects LED color
    const float pot = block->pot[channel];
    ui.set_led(channel, pot > 0.5f ? LED_COLOR_GREEN : LED_COLOR_OFF);
    
    // Gete input and button turn the LED red
    bool gate = false;
    bool button = ui.switches().pressed(channel);
    if (block->input_patched[channel]) {
      for (size_t i = 0; i < size; i++) {
        gate = gate || (block->input[channel][i] & GATE_FLAG_HIGH);
      }
    }
    if (gate || button) {
      ui.set_led(channel, LED_COLOR_RED);
    }
    
    // Slider position (summed with input CV) affects output value
    const float output = (gate || button) ? 1.0f : block->cv_slider[channel];
    ui.set_slider_led(channel, output > 0.001f, 1);
    for (size_t i = 0; i < size; i++) {
      block->output[channel][i] = settings.dac_code(channel, output);
    }
    
  }
  
}

void Init() {
  System sys;
  sys.Init(true);
  dac.Init(int(kSampleRate), 2);
  gate_inputs.Init();
  io_buffer.Init();
  settings.Init();
  std::fill(&no_gate[0], &no_gate[kBlockSize], GATE_FLAG_LOW);
  cv_reader.Init(&settings);
  ui.Init(&settings);
  sys.StartTimers();
  dac.Start(&FillBuffer);
}

int main(void) {
  Init();
  while (1) {
    io_buffer.Process(&ProcessTest);
  }
}
