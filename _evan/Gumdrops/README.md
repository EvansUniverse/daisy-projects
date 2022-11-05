# :candy: Gumdrops :candy:
Sequencer for the Electrosmith Daisy Field.

## Installation Instructions

<details>
<summary><b>If you aren't a developer, and you just want to install the damn program, click here for instructions.</b></summary>

Note: If this is your first time installing anything on your Daisy, I recommend that you start by installing blink [(official instructions)](https://forum.electro-smith.com/t/welcome-to-daisy-get-started-here/15) as a way to learn the Daisy web programmer.

From the [Daisy web programmer](https://electro-smith.github.io/Programmer/):
1. With the eurorack module's power turned off, connect Daisy Seed via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. Download [build/Gumdrops.bin](build/Gumdrops.bin)
4. In the web programmer, click "connect". Select "DFU in FS mode"
6. Click "choose file" and select `Gumdrops.bin`
7. Click "program"

I've tested this in Chrome, compatibility with other browsers may vary.

</details>

&nbsp; <!-- New line -->

If you are a developer, head on over to [BUILD_INSTRUCTIONS.md](../BUILD_INSTRUCTIONS.md) for detailed build instructions.

## Usage Instructions
### TL;DR
* Gumdrops is a basic step sequencer. Each page has 16 steps. There can be up to 8 pages. The current page is shown on the LEDs above the numbered knobs. 
* The LED buttons display the current page of steps. Press a button to enable that step.
* Hold a button to edit that step using the numbered knobs. You can hold multiple 
* There is an on-screen menu, you can edit it with the numbered knobs. The menu has multiple pages. Press `SW1` to cycle pages.
* Gumdrops can store 16 patterns. To change the current pattern, hold `SW2` and select one of the LED buttons.
* Hold `SW2` and press `SW1` to save the current pattern. It will persist when you restart the device.
* Gumdrops can live record notes using either gate/CV data from `GATE IN` and `CV IN 1` or note data from `MIDI IN`. Recording will overdub any current notes. Hold `SW1` and press `SW2` to start live recording. Press `SW1` when you're finished recording.
* Gumdrops can set its own clock in the menu, or receive external clock by sending 1/4 note pulses to `CV IN 4`
* The `Humanize` setting randomizes note times to make it sound less robotic.
* Notes are output to `CV OUT 1` and `GATE OUT 1` as well as the MIDI output jack.
* Each note has a CV setting that can be used to control things like velocity. It is output to `CV OUT 2`


### Controls
<!-- * **Gate in:** 
    * Gate input for live recording -->
* **Gate out:** Trigger out
* **CV in 4:** 
    * Clock pulse in (Gumdrops treats CV4 like a gate input. If the value is above ~3.5v, the gate is high.)
* **CV out 1:** Sequencer pitch out (v/oct) 
* **CV out 2:** Sequencer CV out
<!-- * **MIDI in:** 
    * Midi clock in
    * Note input for live recording -->
* **MIDI out:** Midi clock and note out
* **LEDs 1-8 (above knobs):** Shows the current sequencer page
* **Knobs 1-8:**
    * In sequencer mode: If step(s) are held down, edit parameters of those steps. Otherwise, edit the on-screen menu values.
    * In menu mode: knobs change the on-screen values
* **LED buttons:** 
    * In sequencer mode: show and edit steps
    * In pattern select mode: show and select patterns (access this by holding SW 2 in sequencer mode)
    * In menu mode: selects the menu page
* **SW 1:**
    * Saves pattern data
* **SW 2:**
    * Hold to access pattern select mode
* **Press SW1 while holding SW2**
    * Press to cycle menu pages
<!-- * **Press SW2 while holding SW1**
    * Start live recording.  While live recording, Gumdrops will record gate/CV data from _GATE IN_ and _CV IN 1_ or note data from _MIDI IN_. Recording data overrides any existing data. -->

<!-- * **Press SW 1 + SW 2 simultaneously:** Play/stop sequencer -->
<!--
* **Hold SW 1 + SW 2 simultaneously for 1 second:** Clear current pattern
-->
    

### Menu
Operation is divided into modes; pressing _SW 1_ will cycle the current mode.
#### Sequencer mode
LED buttons show/select sequencer steps. If step(s) are held down, knobs edit parameters of those steps. Otherwise, knobs edit the on-screen menu values.
Items marked with a '(p)' are persisted when the pattern is saved.
* **Page:** Select which page to display 
* **Clock Division:**  Select the amount of bars per-step
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
* **BPM:** (p): BPM of the internal clock (overriden by input to GATE IN 1)
    * 20-500

#### Menu mode
Knobs edit the on-screen values. LED buttons show and select the menu pages.
Items marked with a '(p)' are persisted when the device is powered off.
* **Save:** Save all banks
* **Clear Ptn:** Clear the current pattern
* **Clear All:** Clear all patterns and banks
* **Clock Md:** (p) Clock mode
    * BPM: Keep an internal clock. If clock is coming in through _GATE IN_, it will detect tempo via 1/4 note pulses. Otherwise BPM can be adjusted in the menu.
    * PerTrig: Don't use a clock, instead advance the sequencer by 1 step each time a trigger is revieved through _GATE IN_.
    * MIDI: Receive clock pulse from _MIDI IN_.
* **Ptn Sw Md:** (p) Pattern switch mode. Determines the behavior when a new pattern is selected.
    * End: Wait until the current pattern is finished to start playing the new pattern
    * Page: Wait until the current page is finished to start playing the new pattern
    * Jump: After the current step finishes, start playing the new pattern from the next step (e.g. if you switch patterns while on step 4, it will start playing your new pattern from step 5)
    * Now:  Immediately start playing the new pattern.
* **MIDI Rx Md:** (p) Midi receive mode
    * Clock: receive clock via midi
    * Rec: receive live recording (note data) via midi
    * Off: disable
* **MIDI Tx Ch:** (p) MIDI output channel
    * 0-16
    * If 0 is selected, send to all channels
* **MIDI Rx Ch:** (p) MIDI input channel
    * 0-16
    * If 0 is selected, receive from all channels

## TODOs
* change clock output to 16ths for jb

### Definitely planned for the future
* note micro-timing
* Notes 4, 8, 12, 16 all sound swung into the next note. I think it's bc they occur right before a pulse note and they end up subject to the catch-up algorithm. The other notes seem to sound in time so i dont think its a drifting out of time thing
* Add a reset trig in thru one of the audio ins
* Add a "reload" macro that reloads to the last save. 
* undo function for recording - when record is pressed, save a copy of the current pattern in ram. if undo is pressed before record mode is exited, load that copy back as the active pattern.
* pressing a key while step(s) are held sets their note and oct accordingly
* add a slide setting to notes that slides into next note
* add a "steps on last page" setting per-pattern (or some sort of offset) to enable other time signatures besides 4/4

### Maybe, if I have the time
* MIDI input recording
* Swing
* audio outputs metronome click to help recording (depends on how much binary space this takes up)
* banks, each contains 16 patterns. hold SW 2 in menu mode to switch banks, same way you'd switch patterns
* save/load state data to/from SD card
* user can name patterns/banks/projects
* Display functions of sw1/sw2 on bottom of screen as a footer (under a line.)
    * probably just add a footer, similar to the header, to fieldgui and put the values there.
* Holding a note for longer than a second or so should not delete it. buttons have some sort of "timeHeld" property
* copy/paste. While holding a step/pattern/bank, SW1=copy and SW2=paste

## Known Bugs
* The lowest note (low C) is out of tune. Although the program is telling the DAC the correct output value (0), the output is 0.04v instead of the expected 0v. All other notes, however, are in tune. Might disable low C to compensate, dunno. 
* when recording in a c maj, some sound out of tune
* should hear played note when recording
* BPM on startup should match the menu val
* CV2 callback fn is broken. Since menu relies on it being 0-100, have getDAc2 return the proper value 0-5v.
* long notes override subsequent notes that they overlap. I'd prefer that the new note started playing.
* tempo slows down while a knob (e.g. humanize knob) is continuously turned. I could fix this by adding a "catch-up" feature that detects if the system time elapsed doesn't jive with the steps index and skips forward/backward steps until it's correct. Note that Jellybeans doesn't have this problem, could potentially look there for a soln.
* screen flickers:  (solution: this is most likely caused by patch->display.Update(); being called more than once per event loop, or in more than one place.)
* upon startup, it plays a note (possibly just sends a trig?). Maybe this is an unavoidable hardware issue, I'll have to test

## possibly fixed bugs
* when you switch pages or boot up the device, menu params on the new page shouldnt get changed until you turn a knob. to fix this I'll probably have to move the whole "populate prevknobvals" code to its own fn and call that fn on startup and when a page is cycled.
* sometimes pattern switching causes crash


## Contributing
See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Contact me
See [CONTACT.md](../CONTACT_ME.md)

## License
Author: Evan Pernu

GNU GPL 3.0 - see [../LICENSE.md](../LICENSE.md).