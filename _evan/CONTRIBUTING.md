# Contributing
Thank you for your interest in contributing :) Pull requests are welcome.

As you can probably tell, I'm pretty new to C++, embedded, and Daisy. I would greatly appreciate any constructive criticism or advice on my code.

### Before merging
* Update `_evan/<project_name>/CHANGELOG.md` according to [Keep a Changelog's format](https://keepachangelog.com/en/1.0.0/). 
* Update the `VERSION` variable in `_evan/<project_name>/<project name>.cpp`.
* Add a copy of `_evan/<project_name>/build/<project name>.bin` to `old_builds` and rename it like so: `<project name>_1-0-0.bin`.

### Semver
This project adheres to [semantic versioning](https://semver.org/). Here are my definitions of the different change types:
* Minor change (0.0.x): Doesn't change controls or behavior of the module. Bug fixes, cosmetic improvements, non-behavior-altering features, etc. If the user created a patch with the previous firmware, they should be able to flash the new firmware and have their patch behave exactly the same way.
* Major change (0.x.0): Changes controls or behavior of the module.
* Breaking change (x.0.0): Either a complete and utter overhaul of the software, or a change to the hardware.