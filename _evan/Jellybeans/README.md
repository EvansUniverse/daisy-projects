# :candy: Jellybeans :candy:
Diatonic arpeggio generator for the Electrosmith Daisy Patch eurorack module

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

## Usage

### Features
* User selects a scale (root and quality) through the bmenu.
* Receives clock through _Gate in 1_ 
* Receives cv through _Ctrl 1_ which dictates the root note
* Arpeggio settings are highly configurable and performable
* Sends a bass note out through _CV out 2_

### Controls
* **Gate in 1:** Clock in 
<!--* **Gate in 2:**-->
* **Gate out 1:** Arpeggio gate out
* **Ctrl 1:** Pattern
* **Ctrl 2:** Rhythm
* **Ctrl 3:** Time Division
* **Ctrl 4:** Inversion
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
* **CV out 1:** Arpeggio pitch
* **CV out 2:** Bass pitch

### Menu
Organized as a vertical scrollable menu. Rotating the encoder scrolls to the next menu item. Pushing encoder in highlights the currently selected item, in which case rotating it edits the value. (Items that are controlled by the ctrl knobs cannot be selected in this way, to prevent the knob position from mismatching with the value. Maybe.)

<details>
<summary><b> Full menu </b></summary>
From this project's root directory: `make`

* Pattern (_Ctrl 1)_
    * Up
    * Down
    * Up + down inclusive
    * Up + down exclusive
    * Random
* Voicing (_Ctrl 2)_
    * Triad
    * Triad+ (triad + root)
    * 7th
    * Sus2
    * Sus4
    * Power (root+5)
    * Shell 1
    * Shell 2
    <!--* Kenny Barron-->
* Inversion (_Ctrl 3)_
    * None
    * 1st
    * 2nd
    * 3rd
* PPN (Pulse per note)
    * 1
    * 2
    * 3
    * 4
    * 8
    * 16
* Volt / octave (_Ctrl 4)_
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
* Root (any note)
* Mode
    * Major
    * Dorian
    * Phyrgian
    * Lydian
    * Mixolydian
    * Minor
    * Locrian
* Arp octave range (-2 to +4)
* Octave (0 to +3)
* Bass octave (0 to +3)
<!--* Clock PPQ (pulses per quarter)
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

</details>

## TODOs
### Planned features
* internal clock source
* fractional timing
* varying rhythms
* persist certain state on shutdown e.g. mode of operation
* boot screen (disable during debug mode?), implemented in gui lib
* +/- 1 semitone trim

### Nice to haves & maybes
* Deep settings submenu (would have to implement submenus in gui lib)
* Add some sort of "inversion+oct displacement" setting that can create, for example, shell chords from 7ths
* implement slides
* fine tune setting for +/- 1 semitone?
* Add other piano-inspired patterns/features e.g. adding a low root+5th or low root or high root. Maybe add  a "reinforce" option that adds these things without having to have separate "7th+oct" voicings
* Separate diagnostics page that can monitor perf data on-unit
* seprate menus implemented in gui lib - maybe double clicking the encoder cycles menus
* give bass more options (possibly implement as 2nd arp)

## Known Bugs
* CTRL 4 can't set root note to C0, lowest is C#0. This is due to a hack in Jellybeans.cpp::updateControls()
