# Networked Raytracer
This program endeavors to enable offloading rendering from a less powerful client to a more capable server, since the work a ray tracer does is highly device-dependent. The client describes a scene through a small command shell; the server builds the scene, renders it, and sends the finished image back. 

## Features
- TCP connection between client and server
- Command-line interface on the client
- JSON-based protocol
- Real-time (*ish*) updates
- Image saving

## Structure of the Program
### Client
The client set-up is quite simple, consisting of only two files: `interpreter.py` & `shell.py`. `interpreter.py` provides a shell that not only takes in user input, but also packages it into a dictionary that is structured according to the agreed protocol between the two devices.

`shell.py` is the actual program that runs the shell and interacts with the server. It opens the TCP connection, prompts for input in a loop, sends each command to the server, and handles the response, which could be status messages, JSON, or raw bytes.

### Server
The server program is enormous, and contains all the code for ray-tracing, but the relevant portions would be the `JsonParser` class and `network_tracer.cpp`. 

`network_tracer.cpp` is the director, as it were, of the server program. It starts by getting the actual image generation program set up, and also listening for a connection. When the connection is made, it transitions into a loop that: receives input from the client, interprets it, & responds to client accordingly. 

`JsonParser` interprets input and holds the scene state, maintaining dictionaries of the necessary information contained in JSON objects. It then also builds the actual scene by parsing those dictionaries. 
## How to Run
### Setting up Client
Simply place the two in a directory on the client computer. The client requires Python 3 and the Pillow library.

### Setting up Server
The server is a bit trickier to set up, and is quite a bit larger. It also requires WSL (or Linux) & CLion

- Load the `src` & `test` directories, and the `CMakeLists.txt` file, into a CLion project
- To build the program, within the console, cd into `cmake-build-debug`, and run `make network_tracer`.

### Establishing a Connection
#### If Server is running on Windows
Open windows powershell as admin, and run `wsl hostname -I`, and see what address it returns. Using that address, enter this:
```powershell
netsh interface portproxy add v4tov4 listenaddress=0.0.0.0 listenport=[desired_port] connectaddress=[addr] connectport=[desired_port]
New-NetFirewallRule -DisplayName "[name]" -Direction Inbound -Protocol TCP -LocalPort [desired_port] -Action Allow
```
This is because the program runs using Linux, and thus will use the subsystem's network, and it needs to connect the host's actual network to the subsystem's. Then, when still in powershell, run `ipconfig` and it should show several addresses, use the one under `Wireless LAN adapter Wi-Fi 2` called `IPv4 Address`.


With that set up, first run the server program, putting into the console `./cmake-build-debug/test/network_tracer 0.0.0.0 [desired_port]`. Then the client by running `shell.py`, which will then ask for the ipaddr and port. The ipaddr to enter will be the one found by running `ipconfig`, and the port will be whatever port you chose for the server. Complete these, and you should be connected.

## Defaults
 
---

The **Defaults** and **Inputs Reference** section were made with *Claude*, because they would have been frankly long, verbose, and tedious to write otherwise.

---

The server starts with a small set of pre-loaded definitions so that you can render a scene without having to define every piece yourself.
 
### Default textures
 
| Name    | Type  | Color           |
| ------- | ----- | --------------- |
| `white` | solid | `(1.0 1.0 1.0)` |
| `light` | solid | `(4.0 4.0 4.0)` |
| `green` | solid | `(0.0 1.0 0.0)` |
| `red`   | solid | `(1.0 0.0 0.0)` |
 
### Default materials
 
| Name    | Type       | Texture | Emitting |
| ------- | ---------- | ------- | -------- |
| `light` | lambertian | `light` | true     |
| `white` | lambertian | `white` | false    |
| `green` | lambertian | `green` | false    |
| `red`   | lambertian | `red`   | false    |
 
### Default object
 
| Name            | Center                | Radius   | Material |
| --------------- | --------------------- | -------- | -------- |
| `default_light` | `(100.0 100.0 1100.0)` | `1000.0` | `light`  |
 
### Default camera
 
| Field           | Value             |
| --------------- | ----------------- |
| `position`      | `(0.0 14.0 6.0)`  |
| `target_point`  | `(0.0 0.0 4.0)`   |
| `up`            | `(0.0 0.0 1.0)`   |
| `fov`           | `90.0`            |
 
### Default render settings
 
| Field         | Value          |
| ------------- | -------------- |
| `ray_depth`   | `10`           |
| `ray_samples` | `20`           |
| `output name` | `default.png`  |


## Inputs Reference
Every command the shell accepts begins with one of six **actions**: `ADD`, `EDIT`, `DELETE`, `SET`, `GET`, or `QUIT`. Vectors are written in parentheses with three numbers separated by spaces, e.g. `(1.0 0.5 0.0)`. Booleans accept any of `true`/`t`/`yes`/`y`/`1` or `false`/`f`/`no`/`n`/`0`.
 
### ADD and EDIT
 
Both follow the same form. `ADD` introduces a new texture, material, or object; `EDIT` overwrites an existing one of the same name. The first argument after the action is the **type**, then the **name**, then the type-specific fields.
 
```
ADD  <type> <name> <fields...>
EDIT <type> <name> <fields...>
```
 
Three types are supported:
 
#### `texture`
 
The fields depend on the `texture_type`:
 
| `texture_type` | Additional fields                                                |
| -------------- | ---------------------------------------------------------------- |
| `solid`        | `color` (vector)                                                 |
| `gradient`     | `color1` (vector), `color2` (vector), `vertical` (bool)          |
| `checkerboard` | `color1` (vector), `color2` (vector)                             |
| `swirl`        | `color1`, `color2`, `color3`, `color4` (all vectors)             |
| `normal`       | none                                                             |
| `marble`       | none                                                             |
 
#### `material`
 
| `material_type` | Fields                                                                       |
| --------------- | ---------------------------------------------------------------------------- |
| `lambertian`    | `texture_name` (string), `emitting` (bool)                                   |
| `metallic`      | `texture_name` (string), `emitting` (bool), `fuzz` (float)                   |
 
#### `object`
 
Every object is a sphere and takes the same fields:
 
| Field           | Type    |
| --------------- | ------- |
| `center`        | vector  |
| `radius`        | float   |
| `material_name` | string  |
| `angle`         | vector  |
 
### DELETE
 
Removes a single named texture, material, or object.
 
```
DELETE <type> <name>
```
 
`<type>` is one of `texture`, `material`, or `object`.
 
### SET
 
Updates one of the miscellaneous scene-level settings. The first argument is the **target**, followed by that target's fields.
 
```
SET <target> <fields...>
```
 
| Target   | Fields                                                                                       |
| -------- | -------------------------------------------------------------------------------------------- |
| `camera` | `position` (vector), `target_point` (vector), `up` (vector), `fov` (float)                   |
| `rays`   | `ray_depth` (int), `ray_samples` (int)                                                       |
| `output` | `name` (string — the output filename)                                                        |

### About ADD, EDIT, DELETE, SET
After completing a render, the shell will ask if you want to save it. Entering yes will have it ask you for a name, which should then save it.
 
### GET
 
Asks the server to send back the current state of a scene element. Unlike the other actions, `GET` does not trigger a render; it simply returns JSON describing what was requested.
 
```
GET <target> [<name>]
```
 
The target controls whether a name is required:
 
| Target                          | Name required | Returns                                       |
| ------------------------------- | ------------- | --------------------------------------------- |
| `texture`, `material`, `object` | yes           | the fields of that single named item          |
| `textures`                      | no            | a list of every texture currently defined     |
| `materials`                     | no            | a list of every material currently defined    |
| `objects`                       | no            | a list of every object currently defined      |
| `camera`                        | no            | the current camera settings                   |
| `rays`                          | no            | the current ray-depth and ray-sample settings |
| `output`                        | no            | the current output filename                   |
 
### QUIT
 
Closes the connection and exits the client. You can also press **Ctrl+D** to send `QUIT` automatically.
 
```
QUIT
```
## Code Examples
 
A short session that adds a blue metallic sphere to the default scene and renders it:
 
```
> ADD texture blue (0 0 1)
> ADD material blue_metal metallic blue false 0.05
> ADD object circ (-3 2 1) 2 blue_metal (0 0 0)
```
 
After the third command, the server renders the scene and the client opens the resulting image.
 
Adjusting the camera and tightening up the render settings:
 
```
> SET camera (0 8 6) (0 0 4) (0 0 1) 90
> SET rays 20 50
```
 
Inspecting the scene without rendering:
 
```
> GET objects
> GET material blue_metal
> GET camera
```
 
Removing an object:
 
```
> DELETE object circ
```
 
Ending the session:
 
```
> QUIT
```
