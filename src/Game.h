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

enum class Item {
    Rifle,
    Uzi,
    Shotgun,
    Machete,
    Jacket,

    Ammo1,
    Ammo2,
    Ammo3,
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
