#ifndef STAGES_6EG_ENVELOPE_H_
#define STAGES_6EG_ENVELOPE_H_

namespace stages {

enum EnvelopeStage {
  IDLE,
  DELAY,
  ATTACK,
  HOLD,
  DECAY,
  SUSTAIN,
  RELEASE
};

class Envelope {
  
  public:
    
    Envelope() { }
    ~Envelope() { }
    
    void Init();
    
    inline void SetDelayLength  (float f) { SetStageLength(f, &delayLength  ); };
    inline void SetAttackLength (float f) { SetStageLength(f, &attackLength ); };
    inline void SetHoldLength   (float f) { SetStageLength(f, &holdLength   ); };
    inline void SetDecayLength  (float f) { SetStageLength(f, &decayLength  ); };
    inline void SetSustainLevel (float f) { sustainLevel = f - 0.001f;         };
    inline void SetReleaseLength(float f) { SetStageLength(f, &releaseLength); };
    
    inline bool HasDelay  () { return HasStageLength(&delayLength  ); };
    inline bool HasAttack () { return HasStageLength(&attackLength ); };
    inline bool HasHold   () { return HasStageLength(&holdLength   ); };
    inline bool HasDecay  () { return HasStageLength(&decayLength  ); };
    inline bool HasSustain() { return sustainLevel > 0.001f;          };
    inline bool HasRelease() { return HasStageLength(&releaseLength); };
    
    inline EnvelopeStage CurrentStage() { return stage; }
    
    void Gate(bool high);
    
    float Value();
	
  private:
    
    void SetStage(EnvelopeStage stage);
    void SetStageLength(float f, long *field);
    bool HasStageLength(long *field);
    
    float Interpolate(float from, float to, long time, long length);
    
    EnvelopeStage stage;
    long stageTime;
    float stageStartValue;
    
    long delayLength;
    long attackLength;
    long holdLength;
    long decayLength;
    float sustainLevel;
    long releaseLength;
    
    bool gate;
    float value;
  
};

}

#endif  // STAGES_6EG_ENVELOPE_H_