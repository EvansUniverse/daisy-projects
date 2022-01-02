# Changelog
## [1.0.0] - 12/28/2021
### Changed
* First stable release :)
## [1.1.0] - 12/30/2021
### Added
* BPM detection
* Internal clock
* Fractional clock values
* Set BPM via menu
* Set trim via menu for cv in, arp out, and bass out 
### Changed
* Split theory and gui out into separate libs
## [1.2.0] - 12/31/2021
### Fixed
* Bug that was limiting output notes to one octave
* Bug that was causing weird behavior in scales other than C
## [1.3.0] - 12/31/2021
### Changed
* Reworked octave modifiers to behave more like a user would expect
## [1.4.0] - 1/1/2022
### Added
* Added "quantizer only" mode, activated by settings the "Op Mode" menu item to "Quant". quantized output will be sent through both CV outs.
* The following settings are saved in persistent storage and recalled when Jellybeans is booted up:
    * Clock Div
    * Arp Oct
    * Root
    * Mode
    * Bass Oct
    * BPM
    * Clock Mode
    * Op Mode
    * In Tune
    * Arp Tune
    * Bass Tune
## [1.4.1] - 1/2/2022
* Change compiler optimization flag to reduce binary size