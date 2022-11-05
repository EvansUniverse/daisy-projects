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
### Changed
* Change compiler optimization flag to reduce binary size
## [1.5.0] - 9/25/2022
### Changed
* Updated codebase for compatibility with newest ev_theory and ev_gui changes
### Added
* New note triggers via GATE IN 2
### Fixed
* 1.4.1 had a compilation error
* Bug that interpreted note inputs > C5 as B4
* Arp is now correctly reset when a new note is received
## [1.5.1] - 11/4/2022
### Added
* Oct switch only mode
### Changed
* Clock input now needs to be 16th notes. This allows for greater tempo sync precision.
### Fixed
* Tempo should play much nicer with external clock sources now