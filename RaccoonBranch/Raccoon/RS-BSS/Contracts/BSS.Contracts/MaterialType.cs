using System;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    /// <summary>
    /// Represents material type.
    /// </summary>
    public enum MaterialType : ushort
    {
        None = 0,
        DurusWhite = 430,
        RGD515 = 515,
        RGD525 = 525,
        TangoPlus = 530,
        RGD531 = 531,
        RGD535 = 535,
        MED610 = 610,
        FullCure720 = 720,
        VeroClear = 810,
        VeroWhite = 830,
        VeroYellow = 836,
        VeroBlue = 840,
        VeroGrey = 850,
        VeroMgnt = 851,
        VeroBlack = 870,
        TangoGrey = 950,
        TangoBlack = 970,
        TangoBlackPlus = 980,
        RGD450 = 450,
        VeroCyan = 841,
        FullCure705 = 705,
        SUP707 = 707,
        MCleanser = 10,
        SCleanser = 11,
    }
}