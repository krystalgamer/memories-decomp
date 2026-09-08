#include "../../types.h"
#include "../../psyq/rand.h"
#include "../../game/card_constants.h"

extern s16 D_801D4D8E[];
extern s32 D_801D4244[];

s32 MainMenu_CompareCardsByMaxStat(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 hiA;
    s32 hiB;
    s32 loA;
    s32 loB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            statsA = D_801D4244[idA - 1];
            if (((statsA >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkA = (statsA & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defA = ((statsA >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                hiA = defA;
                if (atkA > hiA) {
                    hiA = atkA;
                }
                loA = defA;
                if (atkA < loA) {
                    loA = atkA;
                }
            } else {
                loA = 0x80000001;
                hiA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            loA = 0x80000001;
            hiA = 0x80000001;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            statsB = D_801D4244[idB - 1];
            if (((statsB >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkB = (statsB & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defB = ((statsB >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                hiB = defB;
                if (atkB > hiB) {
                    hiB = atkB;
                }
                loB = defB;
                if (atkB < loB) {
                    loB = atkB;
                }
            } else {
                loB = 0x80000001;
                hiB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            loB = 0x80000001;
            hiB = 0x80000001;
        }
        if (hiB <= hiA) {
            if (hiA <= hiB) {
                if (loB <= loA) {
                    if (loA <= loB) {
                        if (keyA <= keyB) {
                            if (keyA < keyB) {
                                result = -1;
                            } else {
                                result = 0;
                            }
                        } else {
                            result = 1;
                        }
                    } else {
                        result = -1;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByAttack(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            statsA = D_801D4244[idA - 1];
            if (((statsA >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkA = (statsA & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defA = ((statsA >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                defA = 0x80000001;
                atkA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            defA = 0x80000001;
            atkA = 0x80000001;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            statsB = D_801D4244[idB - 1];
            if (((statsB >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkB = (statsB & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defB = ((statsB >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                defB = 0x80000001;
                atkB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            defB = 0x80000001;
            atkB = 0x80000001;
        }
        if (atkB <= atkA) {
            if (atkA <= atkB) {
                if (defB <= defA) {
                    if (defA <= defB) {
                        if (keyA <= keyB) {
                            if (keyA < keyB) {
                                result = -1;
                            } else {
                                result = 0;
                            }
                        } else {
                            result = 1;
                        }
                    } else {
                        result = -1;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByDefense(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            statsA = D_801D4244[idA - 1];
            if (((statsA >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkA = (statsA & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defA = ((statsA >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                atkA = 0x80000001;
                defA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            atkA = 0x80000001;
            defA = 0x80000001;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            statsB = D_801D4244[idB - 1];
            if (((statsB >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkB = (statsB & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defB = ((statsB >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                atkB = 0x80000001;
                defB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            atkB = 0x80000001;
            defB = 0x80000001;
        }
        if (defB <= defA) {
            if (defA <= defB) {
                if (atkB <= atkA) {
                    if (atkA <= atkB) {
                        if (keyA <= keyB) {
                            if (keyA < keyB) {
                                result = -1;
                            } else {
                                result = 0;
                            }
                        } else {
                            result = 1;
                        }
                    } else {
                        result = -1;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByType(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 typeA;
    s32 typeB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
    } else {
        if (idA != 0) {
            keyA = D_801D4D8E[idA - 1];
            typeA = (D_801D4244[idA - 1] >> CARD_STAT_TYPE_SHIFT) &
                    CARD_STAT_TYPE_MASK;
        } else {
            typeA = 0x7FFFFFFF;
            keyA = 0x7FFFFFFF;
        }
        if (idB != 0) {
            keyB = D_801D4D8E[idB - 1];
            typeB = (D_801D4244[idB - 1] >> CARD_STAT_TYPE_SHIFT) &
                    CARD_STAT_TYPE_MASK;
        } else {
            typeB = 0x7FFFFFFF;
            keyB = 0x7FFFFFFF;
        }
        if (typeA <= typeB) {
            if (typeA >= typeB) {
                if (keyA <= keyB) {
                    if (keyA < keyB) {
                        result = -1;
                    } else {
                        result = 0;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}
