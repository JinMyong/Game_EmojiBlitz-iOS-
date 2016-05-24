#include "Universal.h"
#include "StoreDialogLayer.hpp"
#include "IntroScene.h"
#include "GameOverScene.h"
#include "ScrollItem.hpp"

#include "CocosGUI.h"
#include "cocos-ext.h"

int Universal::SCORE = 0;
int Universal::HIGH_SCORE = 0;
int Universal::GAMESPLAYED = 0;
int Universal::TOTAL_COIN = 0;          // Add by Louis
int Universal::IAP_ITEM_INDEX = -1;      // Added by Louis
bool Universal::isGetFreeCoins = false;

bool Universal::isMusicPlaying = false;

string Universal::emojifileName = "emoji0.png";

bool Universal::REWARD_USED = false;

int Universal::selectedEmojiIndex = -1;

void Universal::selectEmoji(int no)
{
    int emojiIndex = no - 20;
    int cost = Universal::getItemPrice(emojiIndex);
    
    char keyname[20];
    sprintf(keyname, "%s%d", kBoughtItem, emojiIndex);
    
    bool isBought = UserDefault::getInstance()->getBoolForKey(keyname, false);
    
    if (isBought) {
        char name[20];
        sprintf(name,"emoji%d.png", emojiIndex);
        Universal::emojifileName = name;
        Universal::updateItem(emojiIndex);
        UserDefault::getInstance()->setIntegerForKey(kEmogjiname, emojiIndex);
        return;
    }
    
    if (TOTAL_COIN < cost) {
        Scene* runningScene = Director::getInstance()->getRunningScene();
        Layer* layer = (Layer*)runningScene->getChildByTag(LAYER_TAG);
        Universal::IAP_ITEM_INDEX = emojiIndex;
        if (dynamic_cast<IntroScene*>(layer) ) {
            IntroScene* current = (IntroScene*)layer;
            current->showStoreDialog();
        }else if (dynamic_cast<GameOverScene*>(layer))
        {
            GameOverScene* current = (GameOverScene*) layer;
            current->showStoreDialog();
        }
    }else
    {
        char name[20];
        TOTAL_COIN -= cost;
        sprintf(name,"emoji%d.png", emojiIndex);
        Universal::emojifileName = name;
        UserDefault::getInstance()->setBoolForKey(keyname, true);
        UserDefault::getInstance()->setIntegerForKey(kEmogjiname, emojiIndex);
        UserDefault::getInstance()->setIntegerForKey(kTotalCoin, Universal::TOTAL_COIN);
        Universal::updateItem(emojiIndex);
    }
}

void Universal::updateItem(int no)
{
    Scene* runningScene = Director::getInstance()->getRunningScene();
    Layer* layer = (Layer*)runningScene->getChildByTag(1);
    if (dynamic_cast<IntroScene*>(layer) ) {
        
        IntroScene* current = (IntroScene*)layer;
        current->updateActive(no);
        current->updateScore();
        
        cocos2d::ui::ScrollView *scrollview = (cocos2d::ui::ScrollView*)current->getChildByTag(SCROLL_TAG);
        ScrollItem* item = (ScrollItem*)scrollview->getChildByTag(no);
        item->getChildByTag(ITEM_LBL_TAG)->setVisible(false);
        item->getChildByTag(ITEM_COIN__TAG)->setVisible(false);
        
    }else if (dynamic_cast<GameOverScene*>(layer))
    {
        GameOverScene* current = (GameOverScene*) layer;
        current->updateActive(no);
        current->updateScore();
        
        cocos2d::ui::ScrollView *scrollview = (cocos2d::ui::ScrollView*)current->getChildByTag(SCROLL_TAG);
        ScrollItem* item = (ScrollItem*)scrollview->getChildByTag(no);
        item->getChildByTag(ITEM_LBL_TAG)->setVisible(false);
        item->getChildByTag(ITEM_COIN__TAG)->setVisible(false);
    }
}

int Universal::getItemPrice(int index)
{
    int cost = 0;
    if (index > 0 && index <= 7) {
        cost = 50;
    }else if(index >= 8 && index <= 20)
    {
        cost = 100;
    }else if (index >= 21 && index <= 28)
    {
        cost = 250;
    }
    
    return cost;
}