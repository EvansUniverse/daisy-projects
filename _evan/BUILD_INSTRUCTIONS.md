# Build Instructions

<details>
<summary><b>"I'm not a developer, I just want to use the damn program!" (Daisy web programmer)</b></summary>

Note: If this is your first time installing anything on your Daisy, I recommend that you start by installing blink [(official instructions)](https://forum.electro-smith.com/t/welcome-to-daisy-get-started-here/15) as a way to learn the Daisy web programmer.

From the [Daisy web programmer](https://electro-smith.github.io/Programmer/):
1. With the eurorack module's power turned off, connect Daisy Seed via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. Download `build/project_name.bin`
4. In the web programmer, click "connect". Select "DFU in FS mode"
6. Click "choose file" and select `project_name.bin`
7. Click "program"

I've tested this in Chrome, compatibility with other browsers may vary.

</details>


<details>
<summary><b>Debug probe from vscode (recommended for devs)</b></summary>
This is the recommended process for devs because it allows for debugging capabilities and it doesn't require Daisy to be reconnected and reset each time you build.



From a vscode instance in the desired project's directory (for example, `_evan/Jellybeans` to build Jellybeans):
1. Connect the [ST LINK-V3 debug probe](https://www.electro-smith.com/daisy/stlink-v3mini) to Daisy
2. With the device's power turned on, connect the debug probe to your computer via USB.
3. Hit (ctrl+p on Windows, cmd+shift+p on Mac) to bring up the command palette 
4. Type `task build_and_program` and hit enter

</details>

<details>
<summary><b>Direct USB -> Daisy connection from command line</b></summary>

From a vscode instance in the desired project's directory (for example, `_evan/Jellybeans` to build Jellybeans):
1. With the device's power turned off, connect Daisy Seed to your computer via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. `make program-dfu`
4. Disconnect USB and power on the module.

</details>

<details>
<summary><b>Direct USB -> Daisy connection from vscode</b></summary>

From a vscode instance in the desired project's directory (for example, `_evan/Jellybeans` to build Jellybeans):
1. With the device's power turned off, connect Daisy Seed to your computer via USB. If you're re-building, you'll need to briefly disconnect and then reconnect to reset things.
2. Put Daisy into bootloader mode by holding the BOOT button down, and then pressing the RESET button. Once you release the RESET button, you can also let go of the BOOT button. 
3. Hit (ctrl+p on Windows, cmd+shift+p Mac) to bring up the command palette 
4. Type `task build_and_program_dfu` and hit enter
5. Disconnect USB and power on the module

</details>

<details>
<summary><b> Check compilation with no Daisy </b></summary>
From the desired project's root directory (for example, `_evan/Jellybeans` for Jellybeans): `make`

</details>

## Troubleshooting
* If you can't get the command palette stuff to work, they're really just macros for commands. Just go into `tasks.json` and manually copy/paste the corresponding commands.
* I had to manually install `arm-none-eabi-gcc` and restart my computer to get it to work on mac. I also had to manually run `rebuild_all.sh` from the repo root directory to build libdaisy.
* If the expected options don't come up in the command palette, you've probably opened vscode in the wrong directory. Make sure it was opened from the desired project's directory (for example, `_evan/Jellybeans` to build Jellybeans).
* You might have to run additional tasks to re-build the binary. For additional resources: [Official Daisy documentation](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment)
* If this is your first time building a daisy project, you'll need to setup your dev environment as per these [instructions](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment).
* **If this is a fresh clone of this repo or you have updated libdaisy/daisysp, you will need to first run `task build_all` (or `make clean;make`) in order to build libdaisy & daisysp.**
* If you get this error:
    ```
    Error: libusb_open() failed with LIBUSB_ERROR_ACCESS
    Error: open failed
    in procedure 'program'
    ** OpenOCD init failed **
    shutdown command invoked

    make: *** [../../libdaisy/core/Makefile:318: program] Error 1
    ```
    Unplug and re-plug every connection, reboot the daisy, and try again.
* Freezes on startup screen: Try rebooting and/or reinstalling. 