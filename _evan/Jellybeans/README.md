# :candy: Jellybeans :candy:

Diatonic arpeggio generator for Daisy Patch

## TODO
* boot screen (disable during debug mode?)
* persist settings upon startup
* rename src file to jellybeans.cpp
* split defs into a .h file
* rename order to pattern
* make maps const (this causes other bugs that need to be ironed out)
* use their instructions for setting up a new repo so that this isn't in a fork of their example repo
* implement behavior for when a note would be played that's out of the semitone register. Likely just transpose values <0 up an octave and values >0 down an octave
* fine tune for +/- 1 semitone?
* CV in and knobs can alter their respective fields
* Settings submenu
* Display settings (note names, semitones, scale degrees)
* Repo mirroring to github https://docs.gitlab.com/ee/user/project/repository/repository_mirroring.html
* derive menus from map keys
* Add some sort of "inversion+oct displacement" setting that can create, for example, shell chords from 7ths
* Add other piano-inspired patterns/features e.g. adding a low root+5th or low root or high root. Maybe add  a "reinforce" option that adds these things without having to have separate "7th+oct" voicings

## Bugs
* 5th note in kenny barron chords (both major and minor) resolve to octaves

## Features

* User selects a scale (root and quality) through the bmenu.
* Receives clock through _Gate in 1_ 
* Receives cv through _Ctrl 1_ which dictates the root note
* Generates a diatonic arpeggio and sends pitch through _CV out 1 _ and _Gate out 1_
* Arpeggiates these chord progressions and sends the output through _CV out 1_ and _Gate out 1_. 
* Arpeggio settings are highly configurable and performable
* Sends a bassline out through _CV out 2_


## Controls
* **Gate in 1:** Clock in
* **Gate in 2:**
* **Gate out 1:** Arpeggio gate out
* **Ctrl 1:** Root
* **Ctrl 2:** Note division
* **Ctrl 3:** Voicing 
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


## Menu

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


## Development Phases


### Phase -1



* Takes clock (step) in
* Outputs a fixed arpeggio with a configurable root


### Phase 0



* Takes clock and root in
* Outputs 1 type of arpeggio


### Phase 1



* Most arpeggiation features work


### Phase 2



* Most basslines features work


### Phase 3



* Add extra features like virus-style patterns
* Fix bugs, streamline


## Post-Daisy

I want to use the Daisy version for a while to figure out which parameters are useful, how it’s played, etc. to figure out the layout for the non-Daisy version. 

Current ideas:



* More dedicated controls for:
    * Order 
    * Rhythm
    * Octave (at the very least, 2 buttons for up+down)
    * Most bass controls (or, even an expander module for bass controls & output)
* Screen display for:
    * Current values
    * Menu items
    * Scale
    * Octave
* Maybe a piano key style display to indicate current chord and note (e.g. all notes in the current chord are lit blue, the current note is lit green)

