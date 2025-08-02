/******************************************************************************

Copyright (C) 2025 Neil Richardson (nrich@neiltopia.com)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, version 3.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.

******************************************************************************/

#ifndef GAME_H
#define GAME_H

enum class State {
    Title,

    World,
    Map,
    Inventory,
    Dead,
    Laugh,

    // 01.MAP
    CrashedPlaneEntry,      // room3
    CrashedPlaneLeft,       // room2
    CrashedPlaneCockpit,    // room1
    CrashedPlaneRight,      // room4
    CrashedPlaneExit,       // room5

    // 02.MAP
    BunkerEntry,            // bunkerbg
    BunkerExit,             // bunkerin
    BunkerLeft,             // bunkerleft
    BunkerRight             // bunkert
};

enum class Collision {
    Pass,
    Block,
    Touch,
};

enum class Action {
    Look,
    Get,
    Talk,
    Use,
    Inventory,
    Help,
};

enum Input : uint64_t {
    PrimaryAction   = 1 << 0,
    ViewMap         = 1 << 1,
    StepForward     = 1 << 2,
    StepBack        = 1 << 3,
    StepLeft        = 1 << 4,
    StepRight       = 1 << 5,
    TurnLeft        = 1 << 6,
    TurnRight       = 1 << 7,
    LookUp          = 1 << 8,
    LookDown        = 1 << 9,
    ViewInventory   = 1 << 10,
    EquipMachete    = 1 << 11,
    EquipRifle      = 1 << 12,
    EquipShotgun    = 1 << 13,
    EquipUzi        = 1 << 14,
};

enum class Item {
    Rifle,
    Uzi,
    Shotgun,
    Machete,
    Jacket,

    Ammo1,      // Shells
    Ammo2,      // Bullets
    Ammo3,      // clips
    Flower,
    Crystal,
    Compass,
    FlareGun,

    BoltCutters,
    WireClipper,
    Chemicals,
    Beaker,
    Rags,
    OilCan,
    Lighter,
    Smokes,

    Companion,
    Book,
    Syringe,
    Drug,
    FirstAid,
    Coconut,
    Mango,
    Banana,

    Raft,
    GoldMedal1,
    GoldMedal2,
    DeadWolf,

    OiledRifle,
    OilyRags,

    BeakerFlower,
    BeakerFlowerChemicals,
    BeakerFlowerChemicalsCrystal,

    Antidote,

    StuckCompass,

    TripWire,
    Crate,
    Soldier,
};

enum class DamageType {
    Machete,
    Bullet,
};

enum class DeathType {
    None,
    Acid,
    Bat,
    Companion,
    Doc,
    Fence,
    Missile,
    Nurse,
    Plane,
    Rifle,
    Snare,
    Suicide,
    Wire,
    Zombie,
};

enum class Sky {
    Day,
    Cave,
};

enum class Ground {
    Dirt,
    Cave,
};

#endif //GAME_H
