# :plant: Harmonist :plant:
Teachable chord generation AI

## TODO
### Priority 1: Do asap
* gut out old jellybeans code
* design a new MenuItem that acts as a momentary push button (when hovered over, press encoder to "press" the button)
    * Create a child of this for toggle switch

### Priority 2: Needed for initial release
* rename to something garden related
* boot screen (disable during debug mode?)
* split defs into a .h file
* use their instructions for setting up a new repo so that this isn't in a fork of their example repo

### Priotiry 3: Nice to haves

## Known Bugs


## Features
* Learns chord progressions from the user
* Plays chord progressions based on a combination of what it’s learned, new things generated based off of what it's learned, intelligent permutations thereof
* Takes feedback from the user, and employs a neural network (or stochastic function, TBD), and uses it to develop a "personality" that determines further permutations and generations

## Under the hood
### Brainstorm
Data structure:
Mode: uint 
corresponds to a string name and []int set of tones via a static internal map, which I’ll make part of a standard library for my daisy stuff
Progression: [16]uint
Max of 16 values, can use 0 as a blank value. Can play with this number later but 16 seems good. As soon as the harmonist encounters a 0, it will consider that to be the end of progression.
E.g. [1, 5, 6, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
Rating: int
A rating based off of the user’s like/dislike data
Ranges from -128 to 128 (can change this value if needed)###
Generation:
Well, the stupid way would be to use a Markov chain that dictates the next chord based on the previous. But, that’s not musical enough.
Improving upon the Markov model, we could create a stochastic function that generates probabilities for each parameter of the next note based on previous data
This guy used fitness functions to improve a Markov chain for his text generator: https://medium.com/@G3Kappa/writing-a-weight-adjustable-markov-chain-based-text-generator-in-python-9bbde6437fb4
Could use some sort of not-markov function that takes all previous notes into account
Could use a neural network instead of a stochastic function
Lightweight C++ ML libraries: https://github.com/tiny-dnn/tiny-dnn/ (most promising) https://github.com/yixuan/MiniDNN
This would be fun for the “teach your own device” ethos


## Controls
* **Gate in 1:** Clock/step in
* **Gate in 2:**
* **Gate out 1:** Arpeggio gate out
* **Ctrl 1:** Ctrl 1: Progression vary
    * Before 12:00 - mutate the chord progression
    * 12:00 - do not vary
    * Past 12:00 - combination of playing the base chord progression and adding variations
* **Ctrl 2:** Pattern vary
    * Before 12:00 - mutate the pattern
    * 12:00 - do not vary
    * Past 12:00 - combination of playing the base pattern and adding variations
* **Ctrl 3:** Fill chords
    * Before 12:00 - mutate the fill chords
    * 12:00 - do not vary
    * Past 12:00 - combination of playing the base fill chords and adding variations
* **Ctrl 4:**
* **Enc 1 (push):** Select alter/browse menu item 
* **Enc 1 (rotate):** Alter/browse menu item
* **Audio in 1:** N/A
* **Audio in 2:** N/A
* **Audio in 3:** N/A
* **Audio in 4:** N/A
* **Audio out 1:** N/A
* **Audio out 2:** N/A
* **Audio out 3:** N/A
* **Audio out 4:** N/A
* **MIDI in:** Learn in
* **MIDI out:** Chord and tempo data
* **CV out 1:** Arpeggio pitch
* **CV out 2:** Root pitch?


## Menu

Organized as a vertical scrollable menu. Rotating the encoder scrolls to the next menu item. Pushing encoder in highlights the currently selected item, in which case rotating it edits the value. (Items that are controlled by the ctrl knobs cannot be selected in this way, to prevent the knob position from mismatching with the value. Maybe.)

* Home (push button): Sets the “home” state 
* Go home (push button): return to the home state 
* Stay (push button): keep doing what you’re doing. Sets the “home away from home”. 
* Vary (push button): stop that idea completely, try something new. A “shuffle” of sorts.
* Vary level: determines how dramatically the AI will vary from its home state. How crazy or different of things will it try and how frequently will it try them?
* Suggest (push button): The user plays something to the AI, which then attempts to emulate it and vary upon it. The AI will then remember this and add it to its catalog of baselines
* Learn (push button): The AI will add the current state to its catalog of baselines 
* Like (push button): This is the user saying “I like what you’re doing, make it part of your style”. The module will remember this idea and give it a positive bias in future variations. This is how the module gains its own taste/personality. A like of a previously disliked state will make the module neutral to it. 
* Dislike (push button): This is the user saying “I don’t like what you’re doing, do it less often”. The module will remember this idea  and give it a negative bias in future variations. A dislike of a previously liked state will make the module neutral to it.


## Development Phases


### Phase -2
* Can I actually jam a neural network on Daisy?
* Determine whether to use ML or a stochastic function
* Determine the ML library or the first draft stochastic function

### Phase -1
* Identifies input user chord progressions via midi
* Plays back the most recently recalled chord progression
* Uses an arp class inherited from the arp project
* Determines its own tempo (internal clock)


### Phase 0
* Stores data (learnings) locally
* Generates permutations and new progressions based off of data


### Phase 1


### Way later
* Uses a custom MIDI protocol to communicate state with other modules


## Post-Daisy
I want to use the Daisy version for a while to figure out which parameters are useful, how it’s played, etc. to figure out the layout for the non-Daisy version. 