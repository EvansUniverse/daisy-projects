# Contributing
Thank you for your interest in contributing :)

Pull requests are welcome. Please run drastic changes by [me](CONTACT_ME.md) before starting on your PR. If you need a place to start, there are TODO items listed in the README.md of each project.

As you can probably tell, I'm relatively new to C++, embedded, and Daisy. I would greatly appreciate any constructive criticism, advice, or code reviews.

### Before merging
* Update `_evan/<project_name>/CHANGELOG.md` according to [Keep a Changelog's format](https://keepachangelog.com/en/1.0.0/). 
* For projects not in [lib/](lib/), update the `VERSION` variable in `_evan/<project_name>/<project name>.cpp`.
* For projects not in [lib/](lib/), add a copy of `_evan/<project_name>/build/<project name>.bin` to `old_builds` and rename it like so: `<project name>_1-0-0.bin`.

### Semver
This project adheres to [semantic versioning](https://semver.org/). Here are my definitions of the different change types:
* Minor change (0.0.x): Doesn't change controls or behavior of the module. Bug fixes, cosmetic improvements, non-behavior-altering features, etc. If the user created a patch with the previous firmware, they should be able to flash the new firmware and have their patch behave exactly the same way.
* Major change (0.x.0): Changes controls or behavior of the module.
* Breaking change (x.0.0): Either a complete and utter overhaul of the software, or a change to the hardware.