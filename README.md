# Centipede (SFML / C++)

A simpler home-grown remake of Atari’s 1981 classic **Centipede**, built in C++ using SFML.

## 🎮 Gameplay

- You control a **fighter** at the bottom of the screen.  
- A **centipede** of multiple segments crawls down from the top through a field of mushrooms.  
- **Move** with the arrow keys (one cell at a time), and **shoot** with the space bar.  
- Hitting a segment splits the centipede; killing heads awards extra points.  
- Mushrooms block/redirect the centipede and take two shots to destroy.  
- Avoid poisonous mushrooms and enemy creatures (spiders, scorpions, fleas).  
- Each new level speeds up the centipede and adds more mushrooms and fast-moving “head” fragments.  

---

## ⚙️ Features

- **Player movement & shooting** (arrow keys + Space).  
- **Mushrooms** that block, regenerate, and turn poisonous.  
- **Centipede AI** that descends and bounces off mushrooms or screen edges.  
- **Multiple enemy types** (spiders, scorpions, fleas) with unique behaviors.  
- **Level progression**: speed increase, extra mushrooms, shrinking centipede.  
- **Scoring**:  
  - Body segment: 10 pts  
  - Head: 20 pts  
  - Mushroom destroy: 1 pt  
  - Spider: up to 900 pts  
  - Scorpion: 1000 pts  
  - Flea spawn points, regenerating mushrooms, extra lives at score milestones.  
- **Lives**: start with 3, earn extras per 10 000 pts (max 6).  

---

## 🚀 Getting Started

### Prerequisites

- C++17-compatible compiler (MSVC, g++, clang++)  
- [SFML 2.5+](https://www.sfml-dev.org/download.php)

### Build Instructions

```bash
git clone https://github.com/YourUser/Centipede.git
cd Centipede

mkdir build && cd build
cmake ..
cmake --build .      # or: make
```
### Build Instructions
```bash
./Centipede          # or Centipede.exe on Windows
