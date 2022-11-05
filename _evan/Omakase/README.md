# :sushi: Omakase :sushi:
4-channel stereo performance mixer for Electrosmith Daisy Patch, inspired by DJ gear.

## Installation Instructions
Head over to [BUILD_INSTRUCTIONS.md](../BUILD_INSTRUCTIONS.md) for detailed build instructions.

## Usage Instructions
### Tips and  Tricks
* **IMPORTANT:** For any channels you aren't using, either make sure the gain setting is turned down or plug a dummy cable into that input (otherwise Patch will [normal the inputs](https://forum.electro-smith.com/t/disable-normaled-audio-inputs-in-daisy-patch/3085))
* Use the save/load functionality during a live performance! Save your state, use effects like reverb and filter to create a build, then reload your state right as the beat drops.
* Having separate gain and level for each channel allows your to safely mess with volumes during a live performance. `Gain` is intended to be used to adjust the volume of incoming signals, set it and forget it. `Level`  is meant to be used to fade the volume of a channel in and out during a live performance.
* The master distortion can function as a psuedo limiter, try increasing drive just to the point where things barely distort, it will add a bit of warmth and loudness.

### Signal flow
```
Channel 1 in --> distortion --> filter --> pan
Channel 2 in --> distortion --> filter --> pan
Channel 3 in --> distortion --> filter --> pan
Channel 4 in --> distortion --> filter --> pan 
 |   |            |             |
 |   V            V             |
 | Delay send   Reverb send     |
 |   |            |             |--------------------------> Exteral send out L/R
 |___V____________V
 |
 |
 |--> Delay --> Reverb --> Noise --> Distortion --> Compressor --> Master out L/R
```
### Controls

* **Ctrl 1:** On-screen parameter 1
* **Ctrl 2:** On-screen parameter 2
* **Ctrl 3:** On-screen parameter 3
* **Ctrl 4:** On-screen parameter 4
* **Enc 1 (quickly press):** Save the current state
* **Enc 1 (briefly hold, then release):** Load the last saved state
* **Enc 1 (hold for a while):** Enter/exit the settings menu
* **Enc 1 (rotate):** Change page
* **Audio in 1:** Channel 1 in
* **Audio in 2:** Channel 2 in 
* **Audio in 3:** Channel 3 in
* **Audio in 4:** Channel 4 in
* **Audio out 1:** Master out L
* **Audio out 2:** Master out R
* **Audio out 3:** External send out L or mono send out 1
* **Audio out 4:** External send out R or mono send out 2

### Menu
**Main menu**
* **Level:** Per-channel Post-FX level
* **DJ Filter:** Per-channel DJ style filter. Turn clockwise for highpass, counterclockwise for lowpass.
* **Distortion:** Per-channel Distortion drive
* **Delay:** Per-channel Delay send level
* **Reverb:** Per-channel Reverb send level
* **Ext. Send:** Per-channel external send level
* **Ext. Send 2:** If mono sends are enabled, determines the per-channel external send levels for send 2
* **Pan:** Per-channel stereo panning
* **Gain:** Per-channel Pre-FX gain level
* **Transition FX:** Master effects to create builds and drops or transition between songs
    * **Delay:** Delay wet/dry & intensity
    * **Reverb:** Reverb wet/dry * intensity
    * **Noise:** White noise sweep. Turn clockwise for highpass, counterclockwise for lowpass. Turn fully in either direction to mute.
    * **Filter:** DJ style filter

**Settings menu**
* **Distortion:**
    * **Type:** Change the type of distortion algorithm
    * **Tone:** Alters the sound of some of the distortion algorithms. Currently only affects bitcrush (determines the bit depth).
    <!--* **Output:** Lower the output of audio porportionately to drive level to compensate for the volume increase caused by distortion.-->
* **Delay:**
    * **Fdbk:** Feedback (total duration of delay)
    * **Time:** Delay tempo
    * **HPF:** Highpass filter for the delay wet channel
* **Reverb:**
    * **Fdbk:** Feedback (total duration of reverb)
    * **Damp:** Dampening (lowpass like effect)
    * **HPF:** Highpass filter for the delay wet channel
* **Compressor:**
    * **Attack:** 
    * **Release:** 
    * **Thresh:** Threshold
    * **Ratio:**
* **Noise:**
    * **Output:** Volume level
* **Master:**
    * **Gain:** Master gain, before distortion and compressor
    * **Type:** Master distortion algorithm
    * **Drive:** Master distortion drive
    * **Output:** Final master volume output level
* **System:**
    * **Reset:** If set to yes, the system will reset when you save state and reboot the unit. Use this to return to default settings, or recover from a bad/buggy state.
    * **Sends:**
        * **2 mono:** Audio outs 3/4 will function as 2 separate external mono sends
        * **1 stereo:** Audio outs 3/4 will function as L/R for one external stereo send

## TODO
### Main roadmap (required for 1.0 release)
* fix jellybeans
* shoot a vid, bag it and tag it

### Definitely planned for the future
* decide if limiter is needed
* better ui/graphics
* I think echo would sound better than delay. Build a clocked echo library, or use the one here: https://github.com/AdamFulford/Veno-Echo/blob/main/Veno-Echo/
* add bitcrush distortion algorithm
* Consider adding more noise algorithms & a setttings page to adjust this. NOTE: adding dust to noise adds 3% to bin size, might not be feasible 
* add lofi distortion algorithm
* add saturation distortion algorithm
* add an info page on startup - displays controls etc. push encoder to go to next page. add a setting in settings to disable it.
* Add sidechain compression. maybe just fix the channels for this, e.g. 1 to 2

### Maybe, if I have the time
* volume reduction porportionate to drive
* more reverb algorithms (shimmer, hall, plate)
* more delay algorithms (reverse, pingpong, echo)
* decide whether or not end limiter is needed
* fancy transition fx (I have a prototype commented in the page code of Omakase.cpp)
* 3-band EQ (for the master chain, note that Stimming's mastering unit uses 60hz low shelf (sub) and 12khz high shelf (air), which are good value for mastering)
* decide whether or not wraparound is a good idea for omakase
* consider adding a second reverb, one for space one for ambience
* find a good resonance value for filter, make it adjust intelligently if needed. or add it to settings
* fancy ui/graphics
* multi fx (could maybe just add 1 multi effect, as a send, with wet/dry)
* make save/load popup messages box overlay messages, it would look nicer
* make distortion gain safe (rms auto matching)
* See if JUCE has anything cool to import. Maybe the limiter? https://github.com/juce-framework/JUCE/tree/2b16c1b94c90d0db3072f6dc9da481a9484d0435/modules/juce_dsp/widgets 
* Import CloudSeed reverb algorithm: https://github.com/erwincoumans/DaisyCloudSeed/tree/master/patch/CloudSeed
* Chain mode (primary/secondary connection with MIDI that keeps both units on the same page, disables master FX chain on secondary, etc)
* Looper Component: Later down the road, might add a looper. It would be implemented as another page. Would be hard to do with daisy controls but we’ll see. Could be good segue into looping mixer
* once clocked echo is added, add a "pumping" lfo effect ala LFOTool
    * also a beat repeat

## Known Bugs
* None :D
    * ...For now

## Contributing
See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Contact me
See [CONTACT.md](../CONTACT_ME.md)

## License
Author: Evan Pernu

GNU GPL 3.0 - see [../LICENSE.md](../LICENSE.md).