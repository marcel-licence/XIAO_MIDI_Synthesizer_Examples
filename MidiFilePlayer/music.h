musicData track1[] = 
{

    {
        CHANNEL_5,
        {
            {64, true},
            {67, true},
            {71, true},
            {74, true},
        },
        VELOCITY_DEFAULT,
        0,
        1000
    },
    {
        CHANNEL_5,
        {
            {65, true},
            {69, true},
            {72, true},
            {76, true},
        },
        VELOCITY_DEFAULT,
        1,
        1000
    },
    {
        CHANNEL_5,
        {
            {62, true},
            {65, true},
            {69, true},
            {72, true},
        },
        VELOCITY_DEFAULT,
        2,
        1000
    },
    {
        CHANNEL_5,
        {
            {60, true},
            {64, true},
            {67, true},
            {71, true},
        },
        VELOCITY_DEFAULT,
        3,
        1000
    }
};

musicData track2[] = 
{
    {
        CHANNEL_2,
        {
            {48, true},  // Changed note 64 -> 48
            {52, true},  // Changed note 67 -> 52
            {55, true},  // Changed note 71 -> 55
            {59, true},  // Changed note 74 -> 59
        },
        VELOCITY_DEFAULT,
        0,
        1000
    },
    {
        CHANNEL_2,
        {
            {50, true},  // Changed note 65 -> 50
            {53, true},  // Changed note 69 -> 53
            {57, true},  // Changed note 72 -> 57
            {62, true},  // Changed note 76 -> 62
        },
        VELOCITY_DEFAULT,
        1,
        1000
    },
    {
        CHANNEL_2,
        {
            {45, true},  // Changed note 62 -> 45
            {50, true},  // Changed note 65 -> 50
            {55, true},  // Changed note 69 -> 55
            {59, true},  // Changed note 72 -> 59
        },
        VELOCITY_DEFAULT,
        2,
        1000
    },
    {
        CHANNEL_2,
        {
            {43, true},  // Changed note 60 -> 43
            {48, true},  // Changed note 64 -> 48
            {52, true},  // Changed note 67 -> 52
            {55, true},  // Changed note 71 -> 55
        },
        VELOCITY_DEFAULT,
        3,
        1000
    }
};



//Example of multi-track chord data definition
const musicData channel_1_chord =
{
    CHANNEL_9,
    {
        {NOTE_C2, true},
        {NOTE_FS2, true},
    },
    VELOCITY_DEFAULT ,
    0,
    BPM_DEFAULT + BPM_STEP,
};

const musicData channel_2_chord =
{
    CHANNEL_9,
    {
        {NOTE_FS2, true},
    },
    VELOCITY_DEFAULT ,
    1,
    BPM_DEFAULT - BPM_STEP,
};

const musicData channel_3_chord =
{
    CHANNEL_9,
    {
        {NOTE_D2, true},
        {NOTE_FS2, true},
    },
    VELOCITY_DEFAULT ,
    2,
    BPM_DEFAULT - BPM_STEP,
};

const musicData channel_4_chord =
{
    CHANNEL_9,
    {
        {NOTE_FS2, true},
    },
    VELOCITY_DEFAULT ,
    3,
    BPM_DEFAULT + BPM_STEP,
};
musicData track3[] = {channel_1_chord,channel_2_chord,channel_3_chord,channel_4_chord};

