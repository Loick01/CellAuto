# CellAuto (WIP)

<table align="center">
  <tr>
    <td align="center">
      <img src="./screenshot/elementary.gif" height="330"/>
      <br>
      <strong>Elementary cellular automata</strong>
    </td>
    <td align="center">
      <img src="./screenshot/gol.gif" height="330"/>
      <br>
      <strong>Conway's Game of Life</strong>
    </td>
    <td align="center">
      <img src="./screenshot/langton.gif" height="330"/>
      <br>
      <strong>Langton's Ant</strong>
    </td>
  </tr>
</table>

### How to compile

```bash
# Run these commands from the root directory
mkdir build
cd build
cmake ..
make
./cellauto
```

### Controls
*Keyboard and mouse events are not processed if the mouse is hovering over the ImGui window*

+ **Spacebar :** Pause/Resume
+ **Middle click :** Move the camera
+ **Left click :** Make cells alive/dead
+ **Mouse wheel :** Zoom in/out
+ **Escape :** Close the window

### Demo videos

<table align="center">
  <tr>
    <td align="center">
      <a href="https://www.youtube.com/watch?v=l4J-b-8SOkM">
        <img src="https://img.youtube.com/vi/l4J-b-8SOkM/maxresdefault.jpg" height="200">
      </a>
      <br>
      <strong>Game of Life, Langton's Ant and 1D Automata</strong>
    </td>
    <td align="center">
      <a href="https://www.youtube.com/watch?v=bSjD-3EoghI">
        <img src="https://img.youtube.com/vi/bSjD-3EoghI/maxresdefault.jpg" height="200">
      </a>
      <br>
      <strong>Life-like Automata</strong>
    </td>
  </tr>
</table>

### Automata settings

<table align="center">
  <tr>
    <td align="center">
      <img src="./screenshot/grid_setting.png" height="200"/>
      <br>
      <strong>General settings</strong>
    </td>
    <td align="center">
      <img src="./screenshot/color_setting.png" height="200"/>
      <br>
      <strong>Color settings</strong>
    </td>
  </tr>
</table>

<br>
<p align="center">
<em>The available automata can be selected from the drop-down list in the Automata tab. You can choose the neighborhood type (Moore or Von Neumann). In addition, some have also specific parameters (as shown below)</em>
</p>

<div style="display: flex; flex-direction: column; gap: 15px;">

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/elementary_setting.png" height="200">
<div>
<strong>Elementary cellular automata</strong><br>
<strong>State :</strong> 2<br>
<strong>Parameters :</strong> Rule [0-255]<br>
One-dimensional cellular automata (the rendered grid is obtained by stacking the generations). The new state of a cell is determined with the state (from the previous generation) of the same cell and its left and right neighbors. Each of the 8 possible three-cell neighborhood configurations is mapped to either 0 or 1 by a fixed 8-bit rule.
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/gol_setting.png" height="200">
<div>
<strong>Game of Life</strong><br>
<strong>State :</strong> 2<br>
<strong>Parameters :</strong> Birth (numbers of alive neighbors a dead cell needs to become alive), Survive (numbers of alive neighbors a living cell needs to stay alive)<br>
At each iteration, a dead cell becomes alive if the number of living neighbors is in Birth, and a living cell survives if the number of living neighbors is in Survive. All other cells die or remain dead.
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/langton_setting.png" height="200">
<div>
<strong>Langton's Ant</strong><br>
<strong>State :</strong> 2<br>
<strong>Parameters :</strong> None<br>
This automaton represents an ant that moves on a grid. At each step, if the cell is alive, the ant turns 90° clockwise, makes the cell dead, and moves forward one cell. If the cell is dead, the ant turns 90° counterclockwise, makes the cell alive, and moves forward one cell.
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/greenberg_setting.png" height="200">
<div>
<strong>Greenberg-Hastings</strong><br>
<strong>State :</strong> 3<br>
<strong>Parameters :</strong> None<br>
TODO
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/forestfire_setting.png" height="200">
<div>
<strong>Forest-fire model</strong><br>
<strong>State :</strong> 3<br>
<strong>Parameters :</strong> P [0., 1.], F [0., 1.]<br>
TODO
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/cyclic_setting.png" height="200">
<div>
<strong>Cyclic automata</strong><br>
<strong>State :</strong> Between 1 and 25<br>
<strong>Parameters :</strong> Threshold [0, 8]<br>
TODO
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/abelian_setting.png" height="200">
<div>
<strong>Abelian sandpile model</strong><br>
<strong>State :</strong> 4<br>
<strong>Parameters :</strong> Adding position (random or grid center)<br>
TODO
</div>
</div>

<div style="display: flex; align-items: center; gap: 15px;">
<img src="./screenshot/wireworld_setting.png" height="200">
<div>
<strong>Wireworld</strong><br>
<strong>State :</strong> 4<br>
<strong>Parameters :</strong> None<br>
TODO
</div>
</div>

</div>