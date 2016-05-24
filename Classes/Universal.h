#ifndef __Universal__
#define __Universal__

#include <iostream>
#include "cocos2d.h"

#include "config.h"

using namespace cocos2d;
using namespace std;

enum
{
    COINLABEL_TAG = 0,
    LAYER_TAG,
    MENU_TAG,
    ITEM_LBL_TAG,
    ITEM_COIN__TAG,
    SCROLL_TAG,
    ACTIVE_IMG_TAG,
    STORE_DLG_TAG
};


class Universal
{
    
public:
    
    static int SCORE;
    static int GAMESPLAYED;
    static int HIGH_SCORE;
    static int TOTAL_COIN;      // Added by Louis
    static int IAP_ITEM_INDEX;
    static bool isGetFreeCoins;
    static int selectedEmojiIndex;
    
    static bool isMusicPlaying;
    static string   emojifileName;
    static bool REWARD_USED;
    
    static void selectEmoji(int no);
    static void updateItem(int no);
    static int getItemPrice(int index);

};

#endif /* defined(__Universal__) */
