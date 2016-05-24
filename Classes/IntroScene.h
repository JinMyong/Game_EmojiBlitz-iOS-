#ifndef __INTRO_SCENE_H__
#define __INTRO_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "IOSManager.h"
#include "config.h"
#endif

USING_NS_CC;
USING_NS_CC_EXT;

class IntroScene : public cocos2d::Layer, public cocos2d::extension::ScrollViewDelegate
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
    void updateScore();
    void updateActive(int index);
    void createEmojiScrollView();
    
    void makeEmojiScroll();
    void showNextEmojis(Ref* pSender);
    void showPreviousEmojis(Ref* pSender);
    void showActiveEmoji(int index);
    void emojiSelect();
    
    // implement the "static create()" method manually
    CREATE_FUNC(IntroScene);
    
};

#endif // __INTRO_SCENE_H__
