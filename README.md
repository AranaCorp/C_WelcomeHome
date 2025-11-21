<a id="readme-top"></a>

<!-- PROJECT LOGO -->
# C WelcomeHome

<!-- OVERVIEW -->
## Overview
### Descritpion
This project is a tutorial on plain C programming. It gives an example and basic knowledge on key features of the language, interfaces and common librairies

### Main features:
- **Discrete State/Output** Emulate a simple state or output (On/Off)
- **Discrete Input** Emulate discrete input such as a button (integer)
- **Analog Input** Emulate analog input such as a sensor (float)
- **EEPROM interface** Emulated data storage (file,bdd,ROM,etc.)
- **Serial communication** Emulate serial communication (USB, UDP, BLE.)
- **RTC management** Emulated real time (timestamp, alarms)

### System Architecture:
<table>
    <tbody>
        <tr>
            <th>INPUTS </th>
            <th>MEMORY</th>
            <td>file.bin</td>
            <th>OUTPUTS</th>
        </tr>
        <tr>
            <td rowspan=3> 
                Discrete (btn)<br>     
                Analog (sensor)  
            </td>
            <th colspan=2 rowspan=2>CPU</th>
            <td rowspan=3>
                Discrete state <br>
                PWM output 
            </td>
        <tr>
        <tr>
            <td colspan=2 rowspan=4>
                Computer <br>
                plain C  <br>
                gcc 
            </td>
        </tr>
        <tr>
            <th>RTC</th>
            <th>INTERFACES</th>
        </tr>
        <tr>
            <td rowspan=2>computer </td>
            <td  rowspan=2>Serial (socket)</td>
        </tr>
        <tr>
        </tr>
    </tbody>
</table>

---

<!-- GETTING STARTED -->
## Getting Started

### Prerequisites
To be able to use this tutorial you'll need gcc intalled
* MSYS2
* GCC compiler


### Setup
1. Clone the repo
   ```sh
   git clone https://github.com/github_username/repo_name.git
   ```
2. Compile the main code welcomehome.c
   ```sh
   gcc welcomehome.c eeprom.c rtc.c serial.c -o welcomehome.exe -lws2_32
   ```
3. Compile the command code commande.c
   ```sh
   gcc commande.c -o commande.exe -lws2_32
   ```
4. run main code first
   ```sh
   ./welcomehome.exe
   ```
4. run commande code second
   ```sh
   ./welcomehome.exe
   ```

### Usage
You can send commands via the serial interface:

| Command              | Description |
|----------------------|-------------|
| `btn in`             | Emulate button pressed |
| `btn out`            | Emulate button released |
| `cmd on`             | Activate device (if safety conditions met) |
| `cmd off`            | Deactivate device |
| `set_delay <int>`    | Set safety delay (ms), saved to EEPROM |
| `status`             | Print current device state (btn, battery, delay, etc.) |
| `sync <timestamp>`   | Synchronize RTC (not fully implemented) |
---

<!-- ROADMAP -->
## Roadmap
1. [ ] Implement RTC alarms and slot management
2. [ ] Enhance eeprom usage
3. [ ] Enhance battery/charge monitoring
4. [ ] Improve error handling
5. [ ] ~Add BLE communication~
---

<!-- LICENSE -->
## License

<p align="right">(<a href="#readme-top">back to top</a>)</p>