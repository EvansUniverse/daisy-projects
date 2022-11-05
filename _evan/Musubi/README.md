Oscillator processing utility & monosynth

Envelope generator, filter, and 3-band EQ that acts as a one-stop-shop for processing your oscillator. It can also supply its own oscillator and act as a monosynth.

Signal flow:

1. Oscillator (selectable waveform from the basic types sine, square, tri, saw. Or, if we wanna be fancy, smooth morphing between them. optimize for bass, probably 2 oscs. octave/pitch control)
2. Envelope generator (AR or ASR with A/R shaping (concave/convex))
3. VCA
4. Lowpass filter (possibly with selectable algorithm)
5. 3 band eq and/or static multimode filter (selectable between all the useful standard types libdaisy offers)


=================================================================================================================================================================
Notes from original doc:
=================================================================================================================================================================

Envelope + filter, because for some reason nobody’s ever built one 
Also has a 3-band EQ (post filter) because fuck you, sound shaping
Envelope can be routed to either volume or filter
Selectable filter type, HP/LP/4-pole, 8-pole, etc
Filter drive
Output is gain normalized (driving the filter and cranking the EQ does not result in much of a volume change, making it safe to fuck with in a live situation)
Selectable envelope contour, like quadrax
Might opt AHR env type instead of ADSR, less knobs and more practical for modular
Maybe add support for multiple channels, since daisy 
Daisy controls
Page 1: main, performance settings (heck, you won’t even have to leave this page for a lot of use cases)
Cv1 - A
Cv2 - H
Cv3 - R
Cv4 - Freq
Enc push - Toggle routing (volume/filter)
Page 2: “set and forget” settings
Cv1 - Env contour
Cv2 - Filter Resonance
Cv3 - Filter Type
LP 4-pole
LP 8-pole
Possibly different emulations (e.g. ladder, DSI) but for now we’ll just do digital
HP
Cv4 - Filter Drive
Enc push -
Page 3: EQ
Cv1 - Low
Cv2 - High
Cv3 - Low freq (also where mid begins)
Cv4 - Hi freq (also where mid ends)
Enc push - 
Page last: menu (maybe)
New ui idea: first few pages are cycled by knob. Once last (menu) page is scrolled to, it uses the classic evan ui controls and scrolling up past the top returns to the other menu pages.

Hardware controls
A fader/pot
H fader/pot
R fader/pot
Env routing switch (filter/env)
Env contour pot
Filter freq pot
Filter resonance pot
Filter type selector (or could be clever and use a pot like blades, or could use a button to cycle. Actually button to cycle with color coded labels on faceplate seems like a good option)
Filter drive pot
EQ low fader/pot
EQ hi fader/pot
EQ mid fader/pot
EQ low freq pot (also where mid begins)
EQ hi freq pot (also where mid ends)
Audio in level LED
Audio out level LED
Audio in jack (probably mono but consider stereo)
Audio out jack (probably mono but consider stereo, research osc modules to see)
Trigger in jack
Gate in jack (when used, H gets overridden)
Could add CV in jacks for other things like freq, drive, etc to modular-ify it more but tbh that defies the purpose of this module (a manually performed or set-and-forget one stop shop for osc processing) so I’ll probably just opt not too (which has the benefit of keeping cost down) and ignore the complaints

