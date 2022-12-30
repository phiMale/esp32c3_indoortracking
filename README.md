# ESP32C3 Indoortracking

## How to set up a project with **PlatformIO** and the **Espressif IDF**:

**Create a new project first.** This is done like any other project setup with a small caviat: your project path must **not** include any whitespaces!
* Be sure you have **Python** and **pip** on your computer
* Create a new **PIO** project with the **ESP32-C3** and **IDF** as framework
* Wait as loading may take some time
* Change `platform` in `platformio.ini` to `platform = espressif32@4.4.0` as the latest version is not compatible yet (this information took 8 unpaid hours to find, thank you very much)

**Prepare your configurations.** To edit the IDF build configurations within PIO or even within a standard terminal, you will need the PIO CLI:
* Get the PIO **CLI** by opening a new terminal and typing `$ pip install -U platformio`
* Test the install by typing `$ pio --version`
* Open the menuconfig for IDF with `$ pio run -t menuconfig`
> Warning: If you open the menuconfig inside a VSCode terminal, the keys (as in keyboard) will be swapped. To scroll up and down use letters `j` and `k` instead. It is recommended to start the menuconfig with a different terminal like powershell or gitbash anyways.

**Add/edit IDF components.** Run any example project you like by activating the neccessary components:
* Scroll to `Component config`
* Open any component you want
* Activate it or specify a value
* Hit `s` and save the config to your `root` folder. This will create a file `sdkconfig.<your-board-name>`.

**Build project and start coding**. The IDF is big and complex, so intellisense needs a bit of help:
* Build the project **before** you add anything to `src/main.c`
* Include some libs
* Start coding. If intellisense does not see your includes, you configured them wrong or you need to restart VSCode to fully update it
