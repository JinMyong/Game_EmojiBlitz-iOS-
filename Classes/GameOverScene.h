#ifndef __GAMEOVER_SCENE_H__
#define __GAMEOVER_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class GameOverScene : public cocos2d::Layer
{
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // menu callbacks
    void menuPlayCallback(Ref* pSender);
    void menuRateCallback(Ref* pSender);
    void menuLeaderboardCallback(Ref* pSender);
    void menuNoAdsCallback(Ref* pSender);
    void menuShareCallback(Ref* pSender);
    void menuMoreCallback(Ref* pSender);
    void menuStoreCallback(Ref* pSender);
    
    void showStoreDialog();
    void createEmojiScrollView();

    void updateScore();
    
    void updateActive(int index);
    void showNextEmojis(Ref* pSender);
    void showPreviousEmojis(Ref* pSender);
    void showActiveEmoji(int index);
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameOverScene);
    
};

#endif // __GAMEOVER_SCENE_H__
