
#include "stages/envelope.h"

#include <algorithm>

const float kMinStageLength = 0.001f;
const float timeScale = 10000.0f; // 4000 is 1 second

namespace stages {
  
  void Envelope::Init() {
    
    stage = IDLE;
    stageTime = 0L;
    stageStartValue = 0.0f;
    
    delayLength = 0L;
    attackLength = 0L;
    holdLength = 0L;
    decayLength = 0L;
    sustainLevel = 0.0f;
    releaseLength = 0L;
    
    gate = false;
    value = 0.0f;
    
  }
  
  void Envelope::Gate(bool high) {
    
    // Rising
    if (!gate && high) {
      SetStage(HasDelay() ? DELAY : ATTACK);
    }
    
    // Falling
    if (gate && !high) {
      switch (stage) {
        
        // Didn't start yet, back to idle
        case IDLE:
        case DELAY:
          SetStage(IDLE);
          break;
          
        // Else, skip to release stage
        default:
          SetStage(RELEASE);
          break;
          
      }
    }
    
    // Update
    gate = high;
    
  }
  
  float Envelope::Value() {
    
    // Compute stage transitions (cascading)
    if (stage == DELAY   && stageTime >= delayLength  ) SetStage(ATTACK );
    if (stage == ATTACK  && stageTime >= attackLength ) SetStage(HOLD   );
    if (stage == HOLD    && stageTime >= holdLength   ) SetStage(DECAY  );
    if (stage == DECAY   && stageTime >= decayLength  ) SetStage(SUSTAIN);
    if (stage == RELEASE && stageTime >= releaseLength) SetStage(IDLE   );
    
    // Increase elapsed time
    if (stage != IDLE) stageTime++;
    
    // Compute new value
    switch (stage) {
      
      case ATTACK:
        value = Interpolate(stageStartValue, 1.0f, stageTime, attackLength);
        break;
      
      case HOLD:
        value = 1.0f;
        break;
      
      case DECAY:
        value = Interpolate(1.0f, sustainLevel, stageTime, decayLength);
        break;
      
      case SUSTAIN:
        value = sustainLevel;
        break;
      
      case RELEASE:
        value = Interpolate(stageStartValue, 0.0f, stageTime, releaseLength);
        break;
        
      default:
        value = 0.0f;
        break;
      
    }
    
    return value;
    
  }
  
  void Envelope::SetStage(EnvelopeStage s) {
    
    // Set stage (if different than current) and restart the stage timer
    if (stage != s) {
      stage = s;
      stageTime = 0L;
      stageStartValue = value;
    }
    
  }
  
  void Envelope::SetStageLength(float f, long *field) {
    
    // If factor is above threshold, set the length in time units, according to time scale
    if (f >= kMinStageLength) {
      *field = std::max(0L, (long)((f - kMinStageLength) * timeScale));
    } else {
      *field = 0L;
    }
    
  }
  
  bool Envelope::HasStageLength(long *field) {
    
    // Return if it has a length bigger than zero, used for skipping stages and for slider LEDs
    return *field > 0L;
    
  }
  
  float Envelope::Interpolate(float from, float to, long time, long length) {
    
    // Linear interpolation
    // TODO: Interpolate with exp/lin/log continuos function depending on param f=[0..1]
    float t = (float)time / length;
    return from + (to - from) * t;
    
  }
  
}