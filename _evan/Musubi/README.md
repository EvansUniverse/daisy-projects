# :rice_ball: Musubi :rice_ball:
A one-stop-shop for all of your oscillator processing needs.
- Envelope generator
- VCA
- VCF
- Multi-FX (reverb, delay, distortion, DJ filter)

## Installation Instructions
Head over to [BUILD_INSTRUCTIONS.md](../BUILD_INSTRUCTIONS.md) for detailed build instructions.

## Usage Instructions
### Controls
* `CTRL 1-4`: Controls the corresponding on-screen menu options
* `ENC 1`
    - Rotate to change menu page
    - Press to load saved settings & state
    - Briefy hold to save current settings & state
    - Hold until the on-screen loading bar completes to change between the main menu and the settings menus
* `IN 1-4`: Oscillator inputs
* `OUT 1`, `OUT 2`: Stereo master output
* `OUT 3`, `OUT 4`: Stereo master output (identical)
* `GATE IN 1`: Trigger/gate input
* `CV OUT 1`: Envelope output #1 (0-5v CV for the envelope, can be attenuated in settings)
* `CV OUT 2`: Envelope output #2

### Signal flow
1. Oscillator inputs
2. Mixer ("Levels")
3. Panning
4. VCA (amplitude/volume envelope)
5. Distortion (if distortion is set to pre-filter)
6. VCF (filter envelope)
7. Distortion (if distortion is set to post-filter)
8. DJ filter 
7. Delay
8. Reverb

### Menu
**Main menu Pages**
1. Env 
    1. Attack
    2. Hold
    3. Release
    4. Cutoff frequency (having access to it here allows this to be the default "performance page")
3. LPF
    1. Drive
    2. Filter envelope amount
    3. Resonance
    4. Cutoff frequency
4. FX
    1. DJ filter
    2. Reverb amount (feedback + mix)
    3. Delay amount (feedback + mix)
    4. Delay time
4. Env
    1. Attack shape
    2. Release shape
    3. Toggle cascade
    4. Toggle VCA

**Settings menu pages**
1. Volume
    1. Ch1
    2. Ch2
    3. Ch3
    4. Ch4
2. Panning
    1. Ch1
    2. Ch2
    3. Ch3
    4. Ch4
3. Misc
    1. Gate/trig toggle
        - Gate: Envelope will hold for as long as the gate input is high
        - Trig: Envelope will only hold for as long as the hold setting
    2. Filter type
        - Normal: A digital filter from the DaisySP library (svf)
        - Moog: A moog ladder filter emulation
    3. Env out 1 level (attenuates the envelope output to `GATE OUT 1`)
    4. Env out 2 level
4. Reverb
    1. HPF level
    2. LPF level
    3. 
    4. Predelay
5. Delay
    1. HPF level
    2. LPF level
    3. 
    4. Toggle ping pong
6. Distortion
    1. Type (algorithm)
        - Bypass
        - Tanh
        - Atan
        - HardClip
        - SoftClip
        - Saturate
        - Bitcrush
6. System
    1. 
    <!-- Output routing options, concerning Patches 4 audio outputs.  Entries prefixed with 'mx' will have the selected inputs bypass the envelope generator and fx. This allows musubi to be simultaneously used as a mixer and an EG. It's a bit of a wierd function but we might as well make use of Patches many IO jacks. If enabled (not "None"), outputs 3/4 will instead be  
        1. stereo  2 stereo outs (1+2, 3+4)
        2. mono    4 mono outs (1, 2, 3, 4)
        3. mx 1    channel 1 bypasses EG. 2 stereo outs: 1+2 is EG output, 3+4 is mixer output
        4. mx 1-2
        5. mx 1-3
        6. mx 1-4 -->
    2. 
    3. 
    4. Factory reset: Set this to "Yes", save the settings by briefly holding `ENC`, then reboot. This will reset all settings to their factory defaults.


## TODO
### Need
* bitcrush needs work
* add a downsammpling distortion option, maybe merge it with bitcrush

### Want
* each distortion algo should lower output volume as drive increases to prevent earbleed
* output 1s2m by default
* reset pop up is too short
* consider end-of-chain limiter to prevent earbleed, particularly when drive is high
* pick a consistent name for shape/curve/contour/whatever and rename all vars to match it

### If I can get side loading to work (currently not enough binary space for these)
* if no external oscs are supplied, musubi should act as a monosynth. Could also add 1 osc per channel (up to 4)
* Chorus & phaser, for them dubtastic stabs (either will add a 2nd fx page or just hide them away in the "more settings" menu)
* Add more filter types
* import clouds reverb
* clockable delay
* audio meters page in settings, shows 1 bar for each input/output channel. Maybe display average of the past 1ms of audio or something like that
* add this disto: https://dsp.stackexchange.com/questions/13142/digital-distortion-effect-algorithm

## Known Bugs
* If you play with resonance too much it crashes (might be fixed? havent seen it in a while)
* Sometimes crashes when playing with the delay time

## Contributing
See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Contact me
See [CONTACT.md](../CONTACT_ME.md)

## License
Author: Evan Pernu

GNU GPL 3.0 - see [../LICENSE.md](../LICENSE.md).
