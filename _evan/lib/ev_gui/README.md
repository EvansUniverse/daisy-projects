# :desktop_computer: Patch GUI :desktop_computer:
UI Systems for Electrosmith Daisy Patch and Field.

See [patchGui.h](patchGui.h) and [fieldGui.h](fieldGui.h) for more documentation.

## Build Instructions
See [Jellybeans](../../Jellybeans/) and [Omakase](../../Omakase/) for implementation examples of `patchGui` and [Gumdrops](../../Gumdrops/) for an example of `fieldGui`.

## Known Bugs
* PatchGui crashes when attempting to render a page with only one item (possibly <4 items?)
* FieldGui screen flickers (solution: this is most likely caused by patch->display.Update(); being called more than once per event loop, or in more than one place.)

## TODO 
* make menuItemInt header only

## Contributing
See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Contact me
See [CONTACT.md](../CONTACT_ME.md)

## License
Author: Evan Pernu

GNU GPL 3.0 - see [../LICENSE.md](../LICENSE.md).