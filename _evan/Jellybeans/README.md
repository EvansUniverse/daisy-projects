# :candy: Jellybeans :candy:
Quantizing diatonic arpeggiator for the Electrosmith Daisy Patch eurorack module.

[![Click here for the demo video](img.youtube.com/vi/7cRScZM9cCY/0.jpg)](https://www.youtube.com/watch?v=7cRScZM9cCY&ab_channel=Evan%27sUniverse)

### What does it do?
Jellybeans is an arpeggiator with special features. The user configures a scale and sends in v/oct root input, then Jellybeans quantizes and arpeggiates the diatonic chord with that root. This is great for approximating chord progressions with eurorack's chiefly monophonic sound sources.

### Features
* Arpeggiator
* Configurable scale (root and quality)
* Quantizes v/oct input
* Lots of arpeggator options (chord voicings, inversions, clock divisions)
* Sends a bass note (root) out through _CV out 2_
* Inspired by piano chord voicings, WMD Arpitecht, and the Access Virus TI2's arpeggiator

## Build Instructions

<details>
<summary><b>"I'm not a developer, I just want to use the darn program!" (Daisy web programmer)</b></summary>

From the [Daisy web programmer](https://electro-smith.github.io/Programmer/):
1. With the eurorack module's power turned off, connect Daisy Seed via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. Download [build/Jellybeans.bin](build/Jellybeans.bin)
4. In the web programmer, click "connect". Select "DFU in FS mode"
6. Click "choose file" and select `Jellybeans.bin`
7. Click "program"

Check out [this post](https://forum.electro-smith.com/t/welcome-to-daisy-get-started-here/15) for troubleshooting and more info. I've tested this in Chrome compatibility with other browsers may vary.

</details>


<details>
<summary><b>Debug probe from vscode (recommended for devs)</b></summary>

From a vscode instance in this project's root directory:
1. Connect the [ST LINK-V3 debug probe](https://www.electro-smith.com/daisy/stlink-v3mini) to Daisy
2. With the eurorack module's power turned on, connect Daisy Seed via USB.
3. Hit ctrl+p to bring up the command palette 
4. Type `task build_and_program` and hit enter

</details>

<details>
<summary><b>Direct USB -> Daisy connection from command line</b></summary>

From this project's root directory:
1. With the eurorack module's power turned off, connect Daisy Seed via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. `make program-dfu`
4. Disconnect USB and power on the module.

</details>

<details>
<summary><b>Direct USB -> Daisy connection from vscode</b></summary>

From a vscode instance in this project's root directory:
From this project's root directory:
1. With the eurorack module's power turned off, connect Daisy Seed via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. Hit ctrl+p to bring up the command palette 
4. Type `task build_and_program_dfu` and hit enter
5. Disconnect USB and power on the module

</details>

<details>
<summary><b> Check compilation with no Daisy </b></summary>
From this project's root directory: `make`

</details>

Note that you might have to run additional tasks to re-build the binary. For additional resources: [Official Daisy documentation](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment)

## Usage Instructions
### TL;DR
* Use `ENC 1` to navigate the menu (push down to toggle between scrolling and editing)
* Set the note in with the menu or send v/oct to `CTRL 4`
* Set BPM in the menu or send quarter note tempo to `GATE IN 1`
* The arpeggiator will send v/oct to `OUT 1`, triggers to `GATE OUT 1` and bass v/oct to `OUT 2`

### Controls
* **Gate in 1:** Trigger in 
<!--* **Gate in 2:**-->
* **Gate out 1:** Trigger out
* **Ctrl 1:** Pattern
* **Ctrl 2:** Voicing
* **Ctrl 3:** Inversion
* **Ctrl 4:** v/oct
* **Enc 1 (push):** Select alter/browse menu item 
* **Enc 1 (rotate):** Alter/browse menu item
<!--* **Audio in 1:**
* **Audio in 2:**
* **Audio in 3:**
* **Audio in 4:**
* **Audio out 1:**
* **Audio out 2:**
* **Audio out 3:**
* **Audio out 4:**
* **MIDI in:** 
* **MIDI out:** -->
* **CV out 1:** Arpeggio v/oct
* **CV out 2:** Bass v/oct

### Menu
Organized as a vertical scrollable menu. Rotating the encoder scrolls to the next menu item. Pushing encoder in highlights the currently selected item, in which case rotating it edits the value. (Items that are controlled by the ctrl knobs cannot be selected in this way, to prevent the knob position from mismatching with the value. Maybe.)

* **Pattern** (_Ctrl 1)_: Pattern the arp will play notes in
    * Up
    * Down
    * Up + down inclusive (plays the last note on the way up and the way down e.g. C->E->G->G->E->C)
    * Up + down exclusive (play the last note only on the way up e.g. C->E->G->E->C)
    * Random
* **Voicing** (_Ctrl 2)_
    * Triad
    * Triad+ (triad + root)
    * 7th
    * Sus2
    * Sus4
    * Power (root+5)
    * Shell 1
    * Shell 2
    <!--* Kenny Barron-->
* **Inversion** (_Ctrl 3)_
    * None
    * 1st
    * 2nd
    * 3rd
* **Click Division**: Division that the arp will play notes at
    * 1/128
    * 1/64
    * 1/32
    * 1/16
    * 1/8
    * 1/4
    * 1/2
    * 1
    * 2
    * 3
    * 4
    * 5
    * 6
    * 7
    * 8
    * 16
    * 32
    * 64
    * 128
    <!--* 4 bars
    * 2 bars
    * 1 bar
    * Half
    * 4th
    * 8th
    * 16th
    * 32nd
    * 64th (maaaybe) -->
<!--* Rhythm
    * Straight
    * Dotted
    * Swing 25%
    * Swing 50%
    * Swing 75%
    * Swing 100%
    * A number of fun virus-like patterns
    * Trig in (Plays a note when something is sent to trig in. Changes notes for every new trigger)-->
* **Root**: Root note of the quantizer's scale
    * Any note
* **Mode**: Mode of the quantizer's scale
    * Major
    * Dorian
    * Phyrgian
    * Lydian
    * Mixolydian
    * Minor
    * Locrian
* **Octave**: Transposes the arp by x octaves
    * 0 to +2
* **Bass octave**: Transposes the bass by x octaves
    * 0 to +3
* **BPM**: BPM of the internal clock (overriden by input to GATE IN 1)
    * 20-500
* **In Tune**: alters incoming notes by a semitone
    * -12 to 12
* **In Tune**: alters incoming notes by a semitone
    * -12 to 12
* **Note in** (_Ctrl 4)_: The note being fed to the quantizer
    * Any note in a 5 octave register C0-B4
    * Can select a value or supply volt/octave

<!--
* Arp octave range (-2 to +4)
* Bass note division
    * Hold (this will play 1 sustained note until the next chord change)
    * 4 bars
    * 2 bars
    * 1 bar
    * Half
    * 4th
    * 8th
    * 16th
    * 32nd
* Bass Rhythm
    * Straight
    * Dotted
    * Swing 25%
    * Swing 50%
    * Swing 75%
    * Swing 100%
    * A number of fun virus-like patterns designed specifically for bass grooves
    * Trig in (Plays a note when something is sent to trig in. Changes notes for every new trigger)
* Bass Order
    * Up
    * Down
    * Up+down inc.
    * Up+down exc.
    * A number of fun virus-like patterns designed specifically for bass
* Bass Voicing
    * Root only
    * Melody (same chord type as arp)
    * Power (root+5)
    * Walk 1 (root+2)
    * Walk 2 (root+3)
    * Standard (triad)
    * Might include the same options as melody but maybe not. Issue would be option amount on a physical knob
* Clock in rate
    * 1
    * ½
    * ¼
    * ⅛
    * 1/16 -->

## TODOs
### High priority
* octave changes according to note in. currently it only quantizes one octave.

### Definitely planned for the future
* varying rhythms, similar to the Access Virus TI2's arp (implemented as a StepSequencer class)
* persist certain state on shutdown e.g. mode of operation
* user can select which cv input maps to which parameter
* Add alternate modes of operation
    * "quantizer only" mode where arp is completetly disabled and it acts as a pure quantizer

### Maybe, if I have the time
* multi-octave arpeggiation
* Add some sort of "inversion+oct displacement" setting
* slides (per-step portamento)
* Add other piano-inspired patterns/features e.g. adding a low root+5th or low root or high root. Maybe add  a "reinforce" option that adds these things without having to have separate "7th+oct" voicings
* Separate diagnostics page that can monitor perf data on-unit
* seprate menus implemented in gui lib - maybe double clicking the encoder cycles menus
* give bass more options (possibly implement as 2nd arp)
* automated semver
* changing clock divs maintains timing
* add a "pick up where you left off" option to the arp so that it doesn't restart at the root change voicing, etc
* varying fonts on startup screen. add a drawWithFont() function to gui lib
    * maybe not, since flash space is at a premium and importing a font is large. ill have to test this and see how much flash it costs.
* minecraft-esque fun message on startup screen
    * again, barring space concerns. maybe could read these from a file
* figure out how to make the binary smaller so i can fit more features
* quantizer + offset mode https://forum.electro-smith.com/t/jellybeans-diatonic-quantizing-arpeggiator-for-daisy-patch/2178/2?u=evan

## Known Bugs
* CV values jitter when knob is stuck between 2 values. Need to implement hysteresis
* Doesn't properly detect chords for some scales other than C major

## Contributing
See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Contact me
See [CONTACT.md](../CONTACT_ME.md)

## License
Author: Evan Pernu
GNU GPL 3.0 - see [../LICENSE.md](../LICENSE.md).