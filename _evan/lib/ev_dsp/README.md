# :musical_score: DSP :musical_score:
Various sound processing functionalities for the Electrosmith Daisy platform.
* [audioeffect](audioeffect.h) - Simple audio effect framework and a collection of effects with musically sensible parameters
    * [delay](delay.h) - Delay effect using DaisySP's DelayLine
    * [distortion](distortion.h) - Distortion with multiple algorithms
    * [djfilter](djfilter.h) - DJ style filter
    * [pan](pan.h) - Stereo panning
    * [reverb](reverb.h) - Reverb effect using DaisySP's ReverbSc
    * [compressor](compressor.h) - AudioEffect wrapper for DaisySP::Compressor
    * [limiter](compressor.h) - AudioEffect wrapper for DaisySP::Limiter
* [channel](channel.h) - Mixer style channel strip with multiple send destinations
* [noise](noise.h) - Noise generator with a filter sweep

For more documentation, read the `.h` files.

## Usage
See [Omakase](../../Omakase/) for an implementation example

## Known bugs
* None... for now

## TODO
* make djfilter an audioeffect

## Contributing
See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Contact me
See [CONTACT.md](../CONTACT_ME.md)

## License
Author: Evan Pernu

GNU GPL 3.0 - see [../LICENSE.md](../LICENSE.md).