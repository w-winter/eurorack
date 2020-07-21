Mutable Instruments Stages multi-mode firmware
==============================================

This is a unofficial firmware for Mutable Instruments Stages. It was originally created by joeSeggiola and started as a modification to let you enter and exit the "Ouroboros" mode (the **harmonic oscillator** easter egg) at runtime, while the module is powered on. Then, it evolved adding the ability to sequence harmonic ratios, enable slower free-running LFOs and providing a completely alternative mode that transforms the module into **six identical DAHDSR envelope generators**.
This fork further adds bipolar modes for each segment type, re-trigger control for ramp segments, and restores v/oct tracking on slow-LFO mode while expanding slider range.

⚠️ **Warning:** This firmware has **not** been tested on multiple [chained][1] modules. It could behave strangely if chained. Obviously I'm not responsible for any issue you might encounter.

[1]: https://mutable-instruments.net/modules/stages/manual/#chaining-modules


Download and installation
-------------------------

📦 Download the **[latest WAV file here][2]** and follow the [firmware update procedure here][3]. Source code is available [here][8]. joeSeggiola's original source code is available [here][9].

[2]: https://github.com/qiemem/eurorack/releases/latest
[3]: https://mutable-instruments.net/modules/stages/manual/#firmware
[8]: https://github.com/qiemem/eurorack/tree/stages-multi/stages
[9]: https://github.com/joeSeggiola/eurorack/tree/stages-multi/stages


Usage
-----

Hold one of the six buttons for 5 seconds to change mode. This setting is persisted when the module reboots. From left to right:

1. [Segment generator](#segment-generator)
2. [Segment generator](#segment-generator)
3. Segment generator with [slower free-running LFOs](#slower-free-running-lfos)
4. [Six DAHDSR envelope generators](#six-dahdsr-envelope-generators)
5. [Harmonic oscillator](#harmonic-oscillator), aka Ouroboros mode
6. Harmonic oscillator with [alternate controls](#harmonic-oscillator-with-alternate-controls)

### Polarity and re-trigger control

In the first three modes, each segment type now has both a unipolar and bipolar mode.
When on a particular segment type, simply press the mode button again (short press) to access the bipolar mode.
Bipolar mode is indicated by a dimmed LED.
Thus, pressing the mode button now advances through six states rather than three, for both looping and non-looping segments.

A bipolar ramp mode doesn't really make sense since ramps slide between the values of their neighbors.
So, "bipolar" mode ramps instead prevents re-triggering if a trigger is received when that ramp segment is active.
This allows you to make standard Maths like clock dividers by setting the first ramp segment to "bipolar".
This also works on segments besides the first: for instance, set the D of an AD envelope to "bipolar" to prevent re-triggering on decay rather than attack.

Normal modes:

1. Bright green: Unipolar ramp.
2. Dim green: Non-re-triggering ramp.
3. Bright orange: Unipolar step, 0v to 8v.
4. Dim orange: Bipolar step, -8v to 8v.
5. Bright red: Unipolar hold, 0v to 8v.
6. Dim red: Bipolar hold, -8v to 8v.

Loop modes:
1. Bright green: Unipolar LFO, 0v to 8v.
2. Dim green: Bipolar LFO, -5v to 5v.
3. Bright orange: Unipolar step/S&H, 0v to 8v.
4. Dim orange: Bipolar step/S&H, -8v to 8v.
5. Bright red: Unipolar sustain, 0v to 8v.
6. Dim red: Bipolar sustain, -8v to 8v.

Hot tip: apply a negative voltage using a single ramp or hold segment to an LFO segment to increase period to 13 minutes.

### Segment generator

This is the standard mode of the module, refer to the official [Stages manual][4]. This firmware is built on top of official [Stages 1.1][10] and [latest changes][11], therefore it includes **color-blind mode**, **S&H gate delay** and **LFO phase preservation**.

[4]: https://mutable-instruments.net/modules/stages/manual/
[10]: https://mutable-instruments.net/modules/stages/firmware/
[11]: https://github.com/pichenettes/eurorack/commits/master


### Slower free-running LFOs

Fork:

Stages behaves exactly like the standard segment generator mode, except that the slider range on free-running LFOs has been expanded to range from 8 minutes to C1 (the original high-end).
CV input still tracks v/oct.
Finally, ramp slider higher end has been increase from 32 seconds to about 58 seconds.

Original:

In this mode, Stages behaves exactly like the standard segment generator mode, except free-running LFOs (i.e. single green looping segments) are [eight time slower][5].

[5]: https://forum.mutable-instruments.net/t/stages/13643/54


### Six DAHDSR envelope generators

The module transforms into a generator of six identical envelopes. **Sliders** controls the duration (or level) of each stage of all envelopes. From left to right:

1. Duration of the **delay** phase
2. Duration of the **attack** phase
3. Duration of the **hold** phase
4. Duration of the **decay** phase
5. Level of the **sustain** phase
6. Duration of the **release** phase

Each duration goes from 0 to 10 seconds. Each value can be **modulated** using **TIME/LEVEL** inputs. The hold phase is always at maximum level (8V). Each stage can be "disabled" by setting the slider to the bottom; the LED on the slider will turn off to indicate that. For example, set sliders 1 and 3 to zero to get six standard ADSR envelopes.

**SHAPE/TIME** pots 2, 4 and 6 control the **shape** of the corresponding ramp stages, from accelerating through linear, to decelerating. Pots 1, 3 and 5 are unused.

**GATE** inputs are used to activate each of the six envelopes, which can be taken from the corresponding outputs on the bottom of the module. LEDs below pots show the current phase of each envelope: green for delay/attack/hold/decay, orange for sustain, red for release, off when idle. Pressing a **button** will trigger the corresponding envelope manually, like it's a gate signal.


### Harmonic oscillator

This mode was normally accessible on the non-modified firmware by [chaining][1] the module with itself (hence the name "Ouroboros" mode). This firmware simply adds the ability to switch at runtime, without using the cable on the back of the module. All credits goes to [Stages author][9], obviously.

[9]: https://github.com/pichenettes

The left-most column of the module acts a little different from the others:

- Slider is for **coarse tuning** of the main oscillator
- Pot is for **fine tuning**
- CV is a **1V/oct** input for the root pitch
- Output is an **audio mix** of all the harmonics

Each one of the next five columns controls a partial:

- Pot selects the **harmonic ratio** in relation to the root pitch
- Slider and CV input control its **volume** in the mix
- Gate is for **strumming**, i.e. temporarily increase the volume with a fast decay envelope

The **buttons** cycle through different waveform for each harmonic, including the root one. Selected waveform is shown using LEDs colors:

- Green: **sine**
- Orange: **triangle**
- Red: **square**
- Flashing green (long-press): **sawtooth**
- Flashing orange: square with **small pulse width**
- Flashing red: square with **smaller pulse width**


### Harmonic oscillator with alternate controls

Same as harmonic oscillator, but controls for each partial (columns 2 to 6) are swapped:

- Pot controls the **volume** in the mix
- Slider and CV input sets the **harmonic ratio** in relation to the root pitch

This way is possibile to modulate (and therefore sequence) the harmonics with external CV.


Changelog
---------

Fork:

- [**bipolar-v1.0.0**][104]: Add bipolar modes, trigger control, and expanded slow LFO mode.

[104]: https://github.com/qiemem/eurorack/releases/tag/1.0.0-beta


Original:

- [**v3**][103]: Fixed multi-mode switch to avoid unwanted segment loop toggle, independent permanent storage for segments configuration and harmonic oscillator waveform selection.
- [**v2**][102]: Alternate controls mode for harmonic oscillator.
- [**v1**][101]: Initial release, merging slower LFOs and Ouroboros toggle into a single firmware, together with a new 6xDAHDSR mode.

[101]: https://github.com/joeSeggiola/eurorack/releases/tag/stages-multi-v1
[102]: https://github.com/joeSeggiola/eurorack/releases/tag/stages-multi-v2
[103]: https://github.com/joeSeggiola/eurorack/releases/tag/stages-multi-v3

In the works
---
On muffwiggler, several users suggested adding controlled random modes to Stages, and I liked the idea.
The `turing-machine` branch adds a turing machine segment type.
In it, the segment contains an internal shift register which it advances on clock ticks.
The segment's slider/cv input controls the probability of flipping the copied bit.
The segment's potentiometer controls length of the shift register.
Currently, it only works as a standalone segment type, but will work similar to step segment with a turing machine module as input eventually.
Still deciding what looping turing machine mode should do: perhaps internal clock with potentiometer as frequency (and fixed to 16 steps)?
Or perhaps making it sustain based rather than hold so that it can used in envelopes without adding a step (so you'd get, e.g., ASR envelopes with a controlled random S)?
Open to suggestions.
Also not sure what to do with "bipolar" mode as turing machines aren't typically bipolar.
Again open to suggestions.

I do have a few concerns that might prevent the eventual integration of this mode:
- The bipolarity change already expands the number of modes to click through to 6.
    Adding turing machine segments would make it 8, which seems like a lot, but is the same as Plaits, so maybe not a big deal.
- Turing machines are great for controlled randomness, but I'm wondering if there's another approach that would have a little more play better with the other segment types.
    Right now, it gives the other modes random targets so can be used to add variety and evolution to envelopes or sequences, which is definitely useful.
    But I've also been thinking about something that adds "woggle" to envelopes.
    Open to ideas.

That said, interweaving 6 different turing machines each with different step lengths sounds pretty awesome.



Feedback
--------

Please [let me know][6] if you encounter [issues][7] with my firmware modifications, or if you have ideas for additional modes.

[6]: https://github.com/qiemem/eurorack/issues/new
[7]: https://github.com/qiemem/eurorack/issues

