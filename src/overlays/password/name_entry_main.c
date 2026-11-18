#include "../../types.h"
#include "name_entry_main.h"
#include "name_entry_keyboard.h"
#include "name_entry_starter_deck.h"
#include "../../game/main.h"
#include "../../game/mem_card.h"
#include "../../game/card_constants.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"
#include "../../game/campaign_flags.h"
#include "../../game/util.h"

/* Weight of card `id` in a starter-deck pool, out of 2048. */
#define STARTER_DECK_WEIGHT(id, weight) [(id) - CARD_ID_FIRST] = (weight)

/* This unit's .data follows name_entry_runtime.c's keyboard tables. Each pool
 * is a draw count and per-card weights; NameEntry_BuildStarterDeck walks the
 * null-terminated pointer table. */
NameEntryStarterDeckPool
    gNameEntry_aStarterDeckPools[NAME_ENTRY_STARTER_DECK_POOL_COUNT] = {
    {
        16,
        {
            STARTER_DECK_WEIGHT(9, 48), STARTER_DECK_WEIGHT(24, 48),
            STARTER_DECK_WEIGHT(58, 48), STARTER_DECK_WEIGHT(105, 48),
            STARTER_DECK_WEIGHT(123, 48), STARTER_DECK_WEIGHT(130, 48),
            STARTER_DECK_WEIGHT(137, 48), STARTER_DECK_WEIGHT(167, 48),
            STARTER_DECK_WEIGHT(192, 48), STARTER_DECK_WEIGHT(197, 48),
            STARTER_DECK_WEIGHT(202, 48), STARTER_DECK_WEIGHT(237, 48),
            STARTER_DECK_WEIGHT(289, 48), STARTER_DECK_WEIGHT(387, 48),
            STARTER_DECK_WEIGHT(393, 48), STARTER_DECK_WEIGHT(394, 48),
            STARTER_DECK_WEIGHT(395, 48), STARTER_DECK_WEIGHT(397, 48),
            STARTER_DECK_WEIGHT(398, 48), STARTER_DECK_WEIGHT(399, 48),
            STARTER_DECK_WEIGHT(402, 48), STARTER_DECK_WEIGHT(410, 48),
            STARTER_DECK_WEIGHT(411, 48), STARTER_DECK_WEIGHT(422, 48),
            STARTER_DECK_WEIGHT(436, 48), STARTER_DECK_WEIGHT(444, 48),
            STARTER_DECK_WEIGHT(469, 48), STARTER_DECK_WEIGHT(475, 48),
            STARTER_DECK_WEIGHT(484, 48), STARTER_DECK_WEIGHT(485, 48),
            STARTER_DECK_WEIGHT(488, 48), STARTER_DECK_WEIGHT(501, 48),
            STARTER_DECK_WEIGHT(504, 48), STARTER_DECK_WEIGHT(516, 48),
            STARTER_DECK_WEIGHT(524, 48), STARTER_DECK_WEIGHT(527, 48),
            STARTER_DECK_WEIGHT(547, 48), STARTER_DECK_WEIGHT(548, 48),
            STARTER_DECK_WEIGHT(558, 56), STARTER_DECK_WEIGHT(563, 56),
            STARTER_DECK_WEIGHT(635, 56), STARTER_DECK_WEIGHT(644, 56),
        },
    },
    {
        16,
        {
            STARTER_DECK_WEIGHT(5, 24), STARTER_DECK_WEIGHT(8, 24),
            STARTER_DECK_WEIGHT(50, 24), STARTER_DECK_WEIGHT(75, 24),
            STARTER_DECK_WEIGHT(101, 24), STARTER_DECK_WEIGHT(102, 24),
            STARTER_DECK_WEIGHT(104, 24), STARTER_DECK_WEIGHT(107, 24),
            STARTER_DECK_WEIGHT(122, 24), STARTER_DECK_WEIGHT(129, 24),
            STARTER_DECK_WEIGHT(134, 24), STARTER_DECK_WEIGHT(135, 24),
            STARTER_DECK_WEIGHT(145, 24), STARTER_DECK_WEIGHT(148, 24),
            STARTER_DECK_WEIGHT(152, 24), STARTER_DECK_WEIGHT(154, 24),
            STARTER_DECK_WEIGHT(156, 24), STARTER_DECK_WEIGHT(157, 24),
            STARTER_DECK_WEIGHT(158, 24), STARTER_DECK_WEIGHT(159, 24),
            STARTER_DECK_WEIGHT(160, 24), STARTER_DECK_WEIGHT(161, 24),
            STARTER_DECK_WEIGHT(174, 24), STARTER_DECK_WEIGHT(176, 24),
            STARTER_DECK_WEIGHT(179, 24), STARTER_DECK_WEIGHT(182, 24),
            STARTER_DECK_WEIGHT(183, 24), STARTER_DECK_WEIGHT(184, 24),
            STARTER_DECK_WEIGHT(185, 24), STARTER_DECK_WEIGHT(187, 24),
            STARTER_DECK_WEIGHT(191, 24), STARTER_DECK_WEIGHT(195, 24),
            STARTER_DECK_WEIGHT(198, 24), STARTER_DECK_WEIGHT(200, 24),
            STARTER_DECK_WEIGHT(203, 24), STARTER_DECK_WEIGHT(207, 24),
            STARTER_DECK_WEIGHT(208, 24), STARTER_DECK_WEIGHT(209, 24),
            STARTER_DECK_WEIGHT(210, 24), STARTER_DECK_WEIGHT(211, 24),
            STARTER_DECK_WEIGHT(212, 24), STARTER_DECK_WEIGHT(214, 24),
            STARTER_DECK_WEIGHT(222, 24), STARTER_DECK_WEIGHT(227, 24),
            STARTER_DECK_WEIGHT(229, 24), STARTER_DECK_WEIGHT(232, 24),
            STARTER_DECK_WEIGHT(238, 24), STARTER_DECK_WEIGHT(240, 24),
            STARTER_DECK_WEIGHT(242, 24), STARTER_DECK_WEIGHT(245, 24),
            STARTER_DECK_WEIGHT(254, 24), STARTER_DECK_WEIGHT(261, 24),
            STARTER_DECK_WEIGHT(265, 24), STARTER_DECK_WEIGHT(267, 24),
            STARTER_DECK_WEIGHT(268, 24), STARTER_DECK_WEIGHT(271, 24),
            STARTER_DECK_WEIGHT(285, 24), STARTER_DECK_WEIGHT(292, 24),
            STARTER_DECK_WEIGHT(420, 24), STARTER_DECK_WEIGHT(421, 24),
            STARTER_DECK_WEIGHT(451, 24), STARTER_DECK_WEIGHT(452, 24),
            STARTER_DECK_WEIGHT(476, 24), STARTER_DECK_WEIGHT(486, 24),
            STARTER_DECK_WEIGHT(490, 24), STARTER_DECK_WEIGHT(492, 24),
            STARTER_DECK_WEIGHT(506, 24), STARTER_DECK_WEIGHT(510, 24),
            STARTER_DECK_WEIGHT(536, 24), STARTER_DECK_WEIGHT(537, 24),
            STARTER_DECK_WEIGHT(540, 24), STARTER_DECK_WEIGHT(544, 24),
            STARTER_DECK_WEIGHT(549, 24), STARTER_DECK_WEIGHT(556, 24),
            STARTER_DECK_WEIGHT(579, 24), STARTER_DECK_WEIGHT(586, 24),
            STARTER_DECK_WEIGHT(589, 24), STARTER_DECK_WEIGHT(591, 24),
            STARTER_DECK_WEIGHT(598, 24), STARTER_DECK_WEIGHT(602, 24),
            STARTER_DECK_WEIGHT(604, 32), STARTER_DECK_WEIGHT(606, 32),
            STARTER_DECK_WEIGHT(609, 32), STARTER_DECK_WEIGHT(611, 32),
        },
    },
    {
        4,
        {
            STARTER_DECK_WEIGHT(29, 16), STARTER_DECK_WEIGHT(40, 16),
            STARTER_DECK_WEIGHT(47, 16), STARTER_DECK_WEIGHT(48, 16),
            STARTER_DECK_WEIGHT(59, 16), STARTER_DECK_WEIGHT(61, 16),
            STARTER_DECK_WEIGHT(65, 16), STARTER_DECK_WEIGHT(100, 16),
            STARTER_DECK_WEIGHT(108, 16), STARTER_DECK_WEIGHT(113, 16),
            STARTER_DECK_WEIGHT(114, 16), STARTER_DECK_WEIGHT(116, 16),
            STARTER_DECK_WEIGHT(118, 16), STARTER_DECK_WEIGHT(133, 16),
            STARTER_DECK_WEIGHT(138, 16), STARTER_DECK_WEIGHT(139, 24),
            STARTER_DECK_WEIGHT(140, 16), STARTER_DECK_WEIGHT(141, 16),
            STARTER_DECK_WEIGHT(142, 24), STARTER_DECK_WEIGHT(143, 24),
            STARTER_DECK_WEIGHT(144, 24), STARTER_DECK_WEIGHT(155, 24),
            STARTER_DECK_WEIGHT(162, 16), STARTER_DECK_WEIGHT(169, 16),
            STARTER_DECK_WEIGHT(172, 16), STARTER_DECK_WEIGHT(173, 24),
            STARTER_DECK_WEIGHT(175, 24), STARTER_DECK_WEIGHT(177, 24),
            STARTER_DECK_WEIGHT(178, 24), STARTER_DECK_WEIGHT(180, 24),
            STARTER_DECK_WEIGHT(181, 16), STARTER_DECK_WEIGHT(188, 24),
            STARTER_DECK_WEIGHT(189, 24), STARTER_DECK_WEIGHT(190, 24),
            STARTER_DECK_WEIGHT(199, 24), STARTER_DECK_WEIGHT(205, 24),
            STARTER_DECK_WEIGHT(206, 24), STARTER_DECK_WEIGHT(215, 24),
            STARTER_DECK_WEIGHT(218, 24), STARTER_DECK_WEIGHT(219, 16),
            STARTER_DECK_WEIGHT(224, 24), STARTER_DECK_WEIGHT(226, 24),
            STARTER_DECK_WEIGHT(228, 24), STARTER_DECK_WEIGHT(231, 16),
            STARTER_DECK_WEIGHT(239, 24), STARTER_DECK_WEIGHT(243, 24),
            STARTER_DECK_WEIGHT(244, 24), STARTER_DECK_WEIGHT(247, 24),
            STARTER_DECK_WEIGHT(250, 16), STARTER_DECK_WEIGHT(253, 24),
            STARTER_DECK_WEIGHT(257, 16), STARTER_DECK_WEIGHT(260, 24),
            STARTER_DECK_WEIGHT(264, 24), STARTER_DECK_WEIGHT(266, 24),
            STARTER_DECK_WEIGHT(269, 16), STARTER_DECK_WEIGHT(270, 24),
            STARTER_DECK_WEIGHT(276, 16), STARTER_DECK_WEIGHT(277, 24),
            STARTER_DECK_WEIGHT(279, 24), STARTER_DECK_WEIGHT(282, 24),
            STARTER_DECK_WEIGHT(283, 24), STARTER_DECK_WEIGHT(295, 24),
            STARTER_DECK_WEIGHT(296, 16), STARTER_DECK_WEIGHT(298, 24),
            STARTER_DECK_WEIGHT(300, 24), STARTER_DECK_WEIGHT(417, 24),
            STARTER_DECK_WEIGHT(431, 16), STARTER_DECK_WEIGHT(432, 24),
            STARTER_DECK_WEIGHT(446, 24), STARTER_DECK_WEIGHT(461, 24),
            STARTER_DECK_WEIGHT(463, 24), STARTER_DECK_WEIGHT(477, 24),
            STARTER_DECK_WEIGHT(478, 24), STARTER_DECK_WEIGHT(481, 24),
            STARTER_DECK_WEIGHT(489, 24), STARTER_DECK_WEIGHT(503, 24),
            STARTER_DECK_WEIGHT(505, 24), STARTER_DECK_WEIGHT(530, 24),
            STARTER_DECK_WEIGHT(534, 24), STARTER_DECK_WEIGHT(538, 24),
            STARTER_DECK_WEIGHT(539, 24), STARTER_DECK_WEIGHT(553, 16),
            STARTER_DECK_WEIGHT(559, 24), STARTER_DECK_WEIGHT(568, 24),
            STARTER_DECK_WEIGHT(569, 24), STARTER_DECK_WEIGHT(573, 24),
            STARTER_DECK_WEIGHT(580, 16), STARTER_DECK_WEIGHT(585, 24),
            STARTER_DECK_WEIGHT(588, 16), STARTER_DECK_WEIGHT(590, 24),
            STARTER_DECK_WEIGHT(592, 24), STARTER_DECK_WEIGHT(605, 24),
            STARTER_DECK_WEIGHT(610, 24), STARTER_DECK_WEIGHT(612, 24),
            STARTER_DECK_WEIGHT(629, 16), STARTER_DECK_WEIGHT(642, 16),
            STARTER_DECK_WEIGHT(646, 16),
        },
    },
    {
        1,
        {
            STARTER_DECK_WEIGHT(3, 24), STARTER_DECK_WEIGHT(10, 24),
            STARTER_DECK_WEIGHT(23, 24), STARTER_DECK_WEIGHT(25, 24),
            STARTER_DECK_WEIGHT(30, 24), STARTER_DECK_WEIGHT(34, 24),
            STARTER_DECK_WEIGHT(53, 24), STARTER_DECK_WEIGHT(76, 24),
            STARTER_DECK_WEIGHT(80, 24), STARTER_DECK_WEIGHT(109, 24),
            STARTER_DECK_WEIGHT(110, 24), STARTER_DECK_WEIGHT(112, 24),
            STARTER_DECK_WEIGHT(115, 24), STARTER_DECK_WEIGHT(119, 24),
            STARTER_DECK_WEIGHT(120, 24), STARTER_DECK_WEIGHT(121, 24),
            STARTER_DECK_WEIGHT(132, 24), STARTER_DECK_WEIGHT(146, 24),
            STARTER_DECK_WEIGHT(153, 24), STARTER_DECK_WEIGHT(164, 24),
            STARTER_DECK_WEIGHT(165, 24), STARTER_DECK_WEIGHT(171, 24),
            STARTER_DECK_WEIGHT(196, 24), STARTER_DECK_WEIGHT(201, 24),
            STARTER_DECK_WEIGHT(220, 24), STARTER_DECK_WEIGHT(221, 24),
            STARTER_DECK_WEIGHT(225, 24), STARTER_DECK_WEIGHT(233, 16),
            STARTER_DECK_WEIGHT(234, 16), STARTER_DECK_WEIGHT(236, 24),
            STARTER_DECK_WEIGHT(241, 24), STARTER_DECK_WEIGHT(246, 24),
            STARTER_DECK_WEIGHT(248, 24), STARTER_DECK_WEIGHT(251, 24),
            STARTER_DECK_WEIGHT(256, 24), STARTER_DECK_WEIGHT(258, 24),
            STARTER_DECK_WEIGHT(259, 24), STARTER_DECK_WEIGHT(262, 24),
            STARTER_DECK_WEIGHT(263, 24), STARTER_DECK_WEIGHT(272, 24),
            STARTER_DECK_WEIGHT(273, 24), STARTER_DECK_WEIGHT(274, 24),
            STARTER_DECK_WEIGHT(280, 24), STARTER_DECK_WEIGHT(290, 24),
            STARTER_DECK_WEIGHT(291, 24), STARTER_DECK_WEIGHT(293, 24),
            STARTER_DECK_WEIGHT(294, 24), STARTER_DECK_WEIGHT(381, 24),
            STARTER_DECK_WEIGHT(406, 24), STARTER_DECK_WEIGHT(414, 24),
            STARTER_DECK_WEIGHT(430, 24), STARTER_DECK_WEIGHT(435, 24),
            STARTER_DECK_WEIGHT(445, 24), STARTER_DECK_WEIGHT(450, 24),
            STARTER_DECK_WEIGHT(455, 24), STARTER_DECK_WEIGHT(457, 24),
            STARTER_DECK_WEIGHT(474, 24), STARTER_DECK_WEIGHT(480, 24),
            STARTER_DECK_WEIGHT(496, 24), STARTER_DECK_WEIGHT(502, 24),
            STARTER_DECK_WEIGHT(514, 24), STARTER_DECK_WEIGHT(543, 24),
            STARTER_DECK_WEIGHT(546, 24), STARTER_DECK_WEIGHT(550, 24),
            STARTER_DECK_WEIGHT(552, 24), STARTER_DECK_WEIGHT(560, 24),
            STARTER_DECK_WEIGHT(561, 24), STARTER_DECK_WEIGHT(566, 24),
            STARTER_DECK_WEIGHT(567, 24), STARTER_DECK_WEIGHT(570, 24),
            STARTER_DECK_WEIGHT(574, 24), STARTER_DECK_WEIGHT(576, 24),
            STARTER_DECK_WEIGHT(581, 24), STARTER_DECK_WEIGHT(583, 24),
            STARTER_DECK_WEIGHT(584, 24), STARTER_DECK_WEIGHT(599, 24),
            STARTER_DECK_WEIGHT(600, 24), STARTER_DECK_WEIGHT(601, 24),
            STARTER_DECK_WEIGHT(608, 24), STARTER_DECK_WEIGHT(615, 24),
            STARTER_DECK_WEIGHT(616, 24), STARTER_DECK_WEIGHT(620, 24),
            STARTER_DECK_WEIGHT(634, 24), STARTER_DECK_WEIGHT(643, 24),
            STARTER_DECK_WEIGHT(647, 24), STARTER_DECK_WEIGHT(649, 24),
        },
    },
    {
        1,
        {
            STARTER_DECK_WEIGHT(336, 1024), STARTER_DECK_WEIGHT(337, 1024),
        },
    },
    {
        1,
        {
            STARTER_DECK_WEIGHT(330, 344), STARTER_DECK_WEIGHT(331, 344),
            STARTER_DECK_WEIGHT(332, 344), STARTER_DECK_WEIGHT(333, 344),
            STARTER_DECK_WEIGHT(334, 336), STARTER_DECK_WEIGHT(335, 336),
        },
    },
    {
        1,
        {
            STARTER_DECK_WEIGHT(301, 72), STARTER_DECK_WEIGHT(302, 72),
            STARTER_DECK_WEIGHT(303, 72), STARTER_DECK_WEIGHT(304, 72),
            STARTER_DECK_WEIGHT(305, 72), STARTER_DECK_WEIGHT(306, 72),
            STARTER_DECK_WEIGHT(307, 72), STARTER_DECK_WEIGHT(308, 72),
            STARTER_DECK_WEIGHT(309, 72), STARTER_DECK_WEIGHT(310, 72),
            STARTER_DECK_WEIGHT(311, 72), STARTER_DECK_WEIGHT(312, 72),
            STARTER_DECK_WEIGHT(313, 72), STARTER_DECK_WEIGHT(314, 72),
            STARTER_DECK_WEIGHT(315, 80), STARTER_DECK_WEIGHT(316, 72),
            STARTER_DECK_WEIGHT(317, 72), STARTER_DECK_WEIGHT(319, 72),
            STARTER_DECK_WEIGHT(321, 72), STARTER_DECK_WEIGHT(322, 72),
            STARTER_DECK_WEIGHT(323, 72), STARTER_DECK_WEIGHT(324, 72),
            STARTER_DECK_WEIGHT(326, 72), STARTER_DECK_WEIGHT(327, 72),
            STARTER_DECK_WEIGHT(328, 72), STARTER_DECK_WEIGHT(652, 80),
            STARTER_DECK_WEIGHT(654, 80), STARTER_DECK_WEIGHT(659, 80),
        },
    },
};

NameEntryStarterDeckPool
    *gNameEntry_apStarterDeckPools[NAME_ENTRY_STARTER_DECK_POOL_POINTER_COUNT] = {
    &gNameEntry_aStarterDeckPools[0],
    &gNameEntry_aStarterDeckPools[1],
    &gNameEntry_aStarterDeckPools[2],
    &gNameEntry_aStarterDeckPools[3],
    &gNameEntry_aStarterDeckPools[4],
    &gNameEntry_aStarterDeckPools[5],
    &gNameEntry_aStarterDeckPools[6],
    0,
};

void NameEntry_BuildStarterDeck(void)
{
    u8 counts[CARD_COUNT];
    NameEntryStarterDeckPool **table;
    u16 *entry;
    u16 *p;
    u16 *out;
    s32 remaining;
    s32 acc;
    s32 threshold;
    s32 i;

    for (i = CARD_COUNT - 1; i >= 0; i--) {
        counts[i] = 0;
    }
    out = gDuel_awPlayerDeck;
    table = gNameEntry_apStarterDeckPools;
    entry = &(*table)->draw_count;
    while (entry != 0) {
        remaining = *entry;
        entry++;
        do {
            threshold = (rand() & 0x7FF) + 1;
            acc = 0;
            i = 0;
            p = entry;
            do {
                rand();
                acc += *p;
                if (acc >= threshold) {
                    if (counts[i] >= 3) {
                        remaining++;
                    } else {
                        counts[i] = counts[i] + 1;
                        *out = i + 1;
                        Library_UpdateCardUsedFlag(i + 289);
                        out++;
                    }
                    break;
                }
                i++;
                p++;
            } while (i < STARTER_DECK_WEIGHT_SCAN_COUNT);
            remaining--;
        } while (remaining != 0);
        table++;
        entry = &(*table)->draw_count;
    }
}

void NameEntry_Main(void)
{
    SaveDataState *state;
    u8 *entry;
    s32 checksum;
    s32 value;
    s32 i;

    Util_FillMemory(gSaveData_Workspace.prefix, 0, 0x3000);
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
    /* The Japanese build has no load-buffer trace. */
    printf("SaveLoadBuf add = 0x%x size = 0x%x\n", &gSaveData_Workspace, 0x3000);
#endif
    NameEntry_Init();
    do {
        Main_AdvanceFrame();
        rand();
    } while (NameEntry_PollCompletion() == 0);
    NameEntry_BuildStarterDeck();
    state = &gSaveData_Workspace.state;
    checksum = 0;
    entry = state->player_name_sjis;
    for (i = SAVE_DATA_PLAYER_NAME_SIZE - 1; i >= 0; i--) {
        checksum ^= *entry;
        entry++;
    }
    value = gMain_dwFrameCounter << 8;
    while ((state->duelist_code = value | checksum) == 0) {
        value = rand() << 8;
    }
}
