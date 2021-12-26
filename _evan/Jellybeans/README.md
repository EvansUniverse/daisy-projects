# :candy: Jellybeans :candy:
Diatonic arpeggio generator for the Electrosmith Daisy Patch eurorack module

## Build Instructions
### Help! I'm not a programmer, I just want to put this on my Daisy Patch.
// TODO place instructions on using compiled binary with Daisy web programmer here
### Local build 
To build locally (for checking compilation, etc): `make`

### Using a dirrect connection
From this project's root directory:
1. With the eurorack module's power turned off, connect Daisy Seed via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. `make program-dfu`
4. Disconnect USB and power on the module.

### Using a debug probe
Follow the official electrosmith instructions for accomplishing this with VSCode tasks.

### Additional resources
https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment

## Usage
### Features
* User selects a scale (root and quality) through the bmenu.
* Receives clock through _Gate in 1_ 
* Receives cv through _Ctrl 1_ which dictates the root note
* Generates a diatonic arpeggio and sends pitch through _CV out 1 _ and _Gate out 1_
* Arpeggiates these chord progressions and sends the output through _CV out 1_ and _Gate out 1_. 
* Arpeggio settings are highly configurable and performable
* Sends a bassline out through _CV out 2_

### Controls
* **Gate in 1:** Clock in
* **Gate in 2:**
* **Gate out 1:** Arpeggio gate out
* **Ctrl 1:** Pattern
* **Ctrl 2:** Rhythm
* **Ctrl 3:** Time Division
* **Ctrl 4:** Inversion
* **Enc 1 (push):** Select alter/browse menu item 
* **Enc 1 (rotate):** Alter/browse menu item
* **Audio in 1:**
* **Audio in 2:**
* **Audio in 3:**
* **Audio in 4:**
* **Audio out 1:**
* **Audio out 2:**
* **Audio out 3:**
* **Audio out 4:**
* **MIDI in:** 
* **MIDI out:** 
* **CV out 1:** Arpeggio pitch
* **CV out 2:** Bass pitch


### Menu
Organized as a vertical scrollable menu. Rotating the encoder scrolls to the next menu item. Pushing encoder in highlights the currently selected item, in which case rotating it edits the value. (Items that are controlled by the ctrl knobs cannot be selected in this way, to prevent the knob position from mismatching with the value. Maybe.)
* Note division (_Ctrl 2)_
    * 4 bars
    * 2 bars
    * 1 bar
    * Half
    * 4th
    * 8th
    * 16th
    * 32nd
    * 64th (maaaybe)
* Chord voicing (_Ctrl 3)_
    * Standard (triad)
    * Power (root+5)
    * 7th
    * Sus2
    * Sus4
    * 6th
    * Add9
    * Add11
    * Add13
    * Kenny Barron
* Inversion (_Ctrl 4)_
    * Standard
    * Drop 2
    * Drop 3
    * Drop 4 (if there aren't enough notes, default to drop 3)
* Order
    * Up
    * Down
    * Up+down inc.
    * Up+down exc.
    * A number of fun virus-like patterns
* Rhythm
    * Straight
    * Dotted
    * Swing 25%
    * Swing 50%
    * Swing 75%
    * Swing 100%
    * A number of fun virus-like patterns
    * Trig in (Plays a note when something is sent to trig in. Changes notes for every new trigger)
* Arp octave range (-2 to +4)
* Octave (-4 to +4)
* Transpose (-12 to +12) (transposes bassline too)
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
    * 1/16

## TODOs
### Priority 1: Do asap

### Priority 2: Needed for initial release
* Repo mirroring to github https://docs.gitlab.com/ee/user/project/repository/repository_mirroring.html
* fractional timing
* varying rhythms
* inversions
* diatonic input - this will possibly require separate operation modes for diatonic/standalone
* persist certain state on shutdown e.g. mode of operation

### Priority 3: Nice to haves & maybes
* internal clock
* Deep settings submenu (would have to implement submenus in gui lib)
* Add some sort of "inversion+oct displacement" setting that can create, for example, shell chords from 7ths
* implement slides
* fine tune setting for +/- 1 semitone?
* Add other piano-inspired patterns/features e.g. adding a low root+5th or low root or high root. Maybe add  a "reinforce" option that adds these things without having to have separate "7th+oct" voicings
* Separate diagnostics page that can monitor perf data on-unit
* boot screen (disable during debug mode?), implemented in gui lib
* seprate menus implemented in gui lib - maybe double clicking the encoder cycles menus
* give bass more options (possibly implement as 2nd arp)

## Known Bugs
* 5th note in kenny barron chords (both major and minor) resolve to octaves
* Random order tends to bias root notes