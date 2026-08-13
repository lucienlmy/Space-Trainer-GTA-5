<img width="1920" height="1080" alt="37e3d82c-3fdb-41c6-b7e9-5b27ff19aaa3 (1)" src="https://github.com/user-attachments/assets/f42876af-da52-485b-9e09-f88bd6ffb6d3" />


# Space Trainer

Single-player trainer for **Grand Theft Auto V** (.asi).

- Open / close: **F5** (rebindable in Settings)
- Version: **3.5.3**
- Author: [xdigr](https://t.me/xdigr)
- License: GPL-3.0 (see `LICENSE.txt` / `COPYING.txt`)

## Space Trainer 3.5.3 — What's new

- **SPACE TRAINER** branding, smoother UI animations, selection slide, accent glow
- **Zombie Apocalypse** — gradual chaos infection: random pedestrians & drivers turn, corpses rise after a delay, optional armed survivors, cops can get infected
- **Professions** — Firefighter (`s_m_y_fireman_01` + firetruk) and Inkassator / cash collector (armored models + Stockade) with defend / heist jobs
- **Disasters** hub — Tornado, Flood, Meteor Shower
- **FreeCam fix** — player stays visible; camera flies independently
- Bodyguards enter vehicles more reliably (enter / warp + auto-follow)
- Film cameras & Top-Down cam fixes
- Tools Pack (cleanup, film cams, gangs, realistic traffic) — Quick Search console removed
- Compact hubs (Spawner / World / NPC / Functions)
- UTF-8 safe menu text (Russian no longer breaks mid-glyph)
- Default language: English · Russian supported

### Also in 3.5.2

- Contract Hits (5 mission types) — stealth until aimed / shot
- Tools Pack foundations: smart vehicle cleanup, film cams, gangs, realistic traffic
- Rebindable open-menu key
- Safer blackout & safer vehicle spawn
- PedList.xml and SpaceStuff data included

## Features

- Player options
- Bodyguards
- Weapons
- Vehicles
- Spawner
- Teleport
- World / Weather / Time
- NPC tools & animations
- Chaos modes, disasters, zombie outbreak, professions
- Contract Hits
- Object Spooner / creators
- Appearance, themes, settings
- Russian language support

## Install

1. Install **ScriptHookV** + **ASI Loader**
2. Disable **BattlEye** in Rockstar Launcher
3. Copy `Space.asi` and the **`SpaceStuff`** folder into your GTA V directory (same folder as `GTA5.exe`)
4. Launch **Story Mode** and press **F5**

## Requirements

- GTA V (PC)
- ScriptHookV
- ASI Loader
- Story Mode / Single Player only

**Do not use in GTA Online.**

## Build

Windows + VS 2022 (C++):

```
git clone --recursive https://github.com/networkgpts/Space-Trainer-GTA-5.git
cd Space-Trainer-GTA-5
build.bat
```

Output: `Solution/source/_Build/bin/Release/Space.asi`  
More notes in `INSTRUCTIONS.txt`.

## Links

- GitHub: https://github.com/networkgpts/Space-Trainer-GTA-5
- Releases: https://github.com/networkgpts/Space-Trainer-GTA-5/releases
- Latest release tag: https://github.com/networkgpts/Space-Trainer-GTA-5/releases/tag/v3.5.3
- Based on MenyooSP by MAFINS / itsjustcurtis: https://github.com/itsjustcurtis/MenyooSP
