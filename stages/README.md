Mutable Instruments Stages multi-mode firmware
==============================================

This is a unofficial firmware for Mutable Instruments Stages. It was originally created by joeSeggiola and started as a modification to let you enter and exit the "Ouroboros" mode (the **harmonic oscillator** easter egg) at runtime, while the module is powered on. Then, it evolved adding the ability to sequence harmonic ratios, enable slower free-running LFOs and providing a completely alternative mode that transforms the module into **six identical DAHDSR envelope generators**.

This fork further adds:

- **Polarity control for each segment type** (except non-looping ramp/green). Hold button and turn pot to control polarity (left = bipolar, right = unipolar). When bipolar, the segment will flash dull red about 1/sec.
- **Re-trigger control for ramp segments**. Hold button and turn pot to control re-trigger behavior. When re-trigger is disabled, the segment will flash dull red about 1/sec.
- **Independent LFO (clocked and free) range control for each segment**;
    - For free-running, ranges are the same as Tides: 2 min to 2hz at the slowest, 0.125hz to 32hz (default and Stages' original range), and 8hz to about 2khz at the fastest. As with the original Stages and Tides, this range is further expanded by CV.
    - For clocked, ranges are: 1/8 to 1 in low, 1/4 to 4 by default (as in original Stages), 1 to 8x in high.
    - Hold the segment's button and move its slider to change LFO range. LFO range is indicated by the speed of the mode indicator LED's cycle. Note: artifacts appear at high frequencies depending on wave shape. Frequency has been capped at 7khz (A8) as the module acts very strangely after that...
- **Arbitrarily slow clocked LFOs**. Previously, clocked LFOs in Stages had a reset timeout at about 5 seconds; now, the reset timeout adapts to the clock cycle, allowing for arbitrarily slow clocked LFOs (logic taken from Tides 2).
- **Track & hold support**. While you can [get track & hold with the original Stages](https://forum.mutable-instruments.net/t/stages-track-and-hold/16365/11), it takes 3 segments to do so. Now, a single, looping, gated step (orange) segments will track & hold. Single, non-looping, gated step segments still sample & hold.

For any settings that requires you hold the button to change, moving the slider or pot will disable loop mode changes or multi-mode changes, so you don't need to worry about holding th button for too long.
Also changes to those settings won't occur unless you move it's respective control; thus, you won't accidentally change the range on an LFO while changing it's polarity unless you move the slider.
If the slider/pot is already in the position of the setting you want, simply wiggle it to one side and then back into the desired setting while the button is held.
After you release the button, you can then use the pot/slider as normal.

⚠️ **Warning:** This firmware has **not** been tested on multiple [chained][1] modules. It could behave strangely if chained. Obviously I'm not responsible for any issue you might encounter.

[1]: https://mutable-instruments.net/modules/stages/manual/#chaining-modules


Download and installation
-------------------------

📦 Download the **[latest WAV file here][2]** and follow the [firmware update procedure here][3]. Source code is available [here][8]. joeSeggiola's original source code is available [here][9].

IMPORTANT: Installation will clear the module settings if coming from a different firmware. Right after updating from an earlier version of this fork, the stock Stages firmware or joeSeggiola's version, Stages may continuously cycle between green, orange, and red LEDs. Turning the module off and on again should restore functionality. This happens because this fork expands the amount of data stored for each segment, so will be incompatible with the settings stored from a different firmware. If you encounter problems, please let me know, either in a GitHub issue or otherwise.

[2]: https://github.com/qiemem/eurorack/releases/latest
[3]: https://mutable-instruments.net/modules/stages/manual/#firmware
[8]: https://github.com/qiemem/eurorack/tree/stages-multi/stages
[9]: https://github.com/joeSeggiola/eurorack/tree/stages-multi/stages

Core modification usage
---

In the first three modes, each segment type now has both a unipolar and bipolar mode.
To make a segment bipolar, hold down its button and turn its pot above the halfway point.
To make a segment unipolar, either press the button three times (changing type resets polarity) or hold down its button and turn its pot below the halfway point.
When in bipolar mode, the segment will a dim red color about once a second (this should be visible regardless of base color or looping mode).

A bipolar ramp mode doesn't really make sense since ramps slide between the values of their neighbors.
So, "bipolar" mode ramps instead prevents re-triggering if a trigger is received when that ramp segment is active.
This allows you to make standard Maths like clock dividers by setting the first ramp segment to "bipolar".
This also works on segments besides the first: for instance, set the D of an AD envelope to "bipolar" to prevent re-triggering on decay rather than attack.

Bipolar LFOs (single, looping green segments) output -5v to 5v. All other bipolar segment types output -8v to 8v.


Multi-mode usage
-----

Hold one of the six buttons for 5 seconds to change mode. This setting is persisted when the module reboots. From left to right:

1. [Segment generator](#segment-generator)
2. [Segment generator](#segment-generator)
3. Segment generator with [slower free-running LFOs](#slower-free-running-lfos)
4. [Six DAHDSR envelope generators](#six-dahdsr-envelope-generators)
5. [Harmonic oscillator](#harmonic-oscillator), aka Ouroboros mode
6. Harmonic oscillator with [alternate controls](#harmonic-oscillator-with-alternate-controls)

### Segment generator

This is the standard mode of the module, refer to the official [Stages manual][4]. This firmware is built on top of official [Stages 1.1][10] and [latest changes][11], therefore it includes **color-blind mode**, **S&H gate delay** and **LFO phase preservation**.

[4]: https://mutable-instruments.net/modules/stages/manual/
[10]: https://mutable-instruments.net/modules/stages/firmware/
[11]: https://github.com/pichenettes/eurorack/commits/master


### Slower free-running LFOs

In this mode, Stages behaves exactly like the standard segment generator mode, except free-running LFOs (i.e. single green looping segments) are [eight time slower][5].
This fork applies this 8x slowdown to each of the LFO ranges, so while the default is the same as in joeSeggiola's original, much slower LFOs may be achieved (16 minutes), while also mixing with faster LFOs.

Note: Since LFO range configuration has been integrated in as a segment configuration, this mode may be removed to make space for other things.

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

