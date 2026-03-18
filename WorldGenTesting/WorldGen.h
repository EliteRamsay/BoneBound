#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "string"

using namespace std;

typedef enum  
{
    POCKET,
    SMALLER,
    SMALL,
    MEDIUM,
    LARGE,
    GIGANTIC
}
WorldSize;

typedef enum
{
    FIVEYEARS,
    FIFTYEARS,
    HUNDREDYEARS,
    TWOHUNDREDANDFIFTYEARS,
    FIVEHUNDREDYEARS,
    THOUSANDYEARS
}
HistoryLength;


//Based of world size
typedef enum
{
    VERYLOW,
    LOW,
    MEDIUM,
    HIGH,
    VERYHIGH
}
NumberOfCivilizations;

//Based of world size
typedef enum
{
    VERYLOW,
    LOW,
    MEDIUM,
    HIGH,
    VERYHIGH
}
MaxiumNumberOfSites;

//Based of world size
typedef enum
{
    VERYLOW,
    LOW,
    MEDIUM,
    HIGH,
    VERYHIGH
}
NumberOfBeasts;//Dragons,Titans

typedef enum
{
    VERYLOW,
    LOW,
    MEDIUM,
    HIGH,
    VERYHIGH
}
NaturalSavagery;//Increasing this value increases the number of savage biomes in the world. In short, this means that more areas are likely to have aggressive animals which may kill Cats or Dogs

typedef enum
{
    VERYLOW,
    LOW,
    MEDIUM,
    HIGH,
    VERYHIGH
}
MineralOccurrence;//Increasing this value increases the number of mineral veins in the world. This means that more areas are likely to have veins of coal, iron, gold, etc.














#endif