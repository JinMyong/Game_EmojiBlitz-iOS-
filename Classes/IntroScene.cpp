#include "IntroScene.h"
#include "GameScene.h"
#include "Universal.h"
#include "NDKHelper.h"
#include "config.h"
#include "SimpleAudioEngine.h"
#include "StoreDialogLayer.hpp"
#include "ScrollItem.hpp"

#include "CocosGUI.h"
#include "cocos-ext.h"

using namespace CocosDenshion;
using namespace cocos2d::extension;

Scene* IntroScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = IntroScene::create();
    layer->setTag(LAYER_TAG);
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool IntroScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    // get visible size
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    // get highscore and games played
    UserDefault *userDefaults = UserDefault::getInstance();
    Universal::HIGH_SCORE = userDefaults->getIntegerForKey("HIGHSCORE");
    userDefaults->flush();
    
// Added by Louis for load total coins.
    Universal::TOTAL_COIN = userDefaults->getIntegerForKey(kTotalCoin);
    userDefaults->flush();
// Add end
    
    Universal::GAMESPLAYED = userDefaults->getIntegerForKey("GAMESPLAYED");
    userDefaults->flush();
    
    // log into gamecenter
    IOSManager::shared()->loginGameCenter();
    
    // create retry button
    auto play = MenuItemImage::create(
                                      "play.png",
                                      "play.png",
                                      CC_CALLBACK_1(IntroScene::menuPlayCallback, this));
    
    play->setPosition(Vec2(visibleSize.width * 1 / 5, visibleSize.height / 2.9));
    
    // create store button
    auto store = MenuItemImage::create("store.png",
                                       "store.png",
                                       CC_CALLBACK_1(IntroScene::menuStoreCallback, this));
    store->setPosition(Vec2(visibleSize.width * 2/ 5, play->getPosition().y));
    
    
    // create remove ads button
    auto share = MenuItemImage::create(
                                       "share.png",
                                       "share.png",
                                       CC_CALLBACK_1(IntroScene::menuShareCallback, this));
    share->setPosition(Vec2(visibleSize.width * 3 / 5, play->getPosition().y));
    
    // create remove ads button
    auto removeads = MenuItemImage::create(
                                           "noads.png",
                                           "noads.png",
                                           CC_CALLBACK_1(IntroScene::menuNoAdsCallback, this));
    
    removeads->setPosition(Vec2(visibleSize.width * 4 / 5, play->getPosition().y));
    
    // create rate button
    auto rate = MenuItemImage::create(
                                      "rate.png",
                                      "rate.png",
                                      CC_CALLBACK_1(IntroScene::menuRateCallback, this));
    rate->setPosition(Vec2(visibleSize.width / 4, play->getPosition().y - rate->getContentSize().height * 1.25));
    
    // create more games button
    auto more = MenuItemImage::create(
                                      "moregames.png",
                                      "moregames.png",
                                      CC_CALLBACK_1(IntroScene::menuMoreCallback, this));
    
    more->setPosition(Vec2(visibleSize.width * 3 / 4 , play->getPosition().y - more->getContentSize().height * 1.25));
    
    // create leaderboard button
    auto leaderboard = MenuItemImage::create(
                                             "leaderboard.png",
                                             "leaderboard.png",
                                             CC_CALLBACK_1(IntroScene::menuLeaderboardCallback, this));
    leaderboard->setPosition(Vec2(visibleSize.width / 2, play->getPositionY() - store->getContentSize().height * 1.25));
    
    
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(play, rate, leaderboard, removeads, share, more, store, NULL);
    menu->setPosition(Vec2::ZERO);
    menu->setTag(MENU_TAG);
    this->addChild(menu, 1);
    
    
    
    // add high score
    char _highscore[256];
    sprintf(_highscore, "%d", Universal::HIGH_SCORE);
    std::string strHighscore = "BEST: ";
    strHighscore.append(_highscore);
    const char * charHighscore = strHighscore.c_str();
    auto highLabel = Label::createWithTTF(charHighscore, "Nexa Bold.otf",65);
    highLabel->setColor(Color3B(255,255,255));
    highLabel->setPosition( Vec2(visibleSize.width/2,  visibleSize.height/1.4) );
    highLabel->setAnchorPoint(Vec2(0.5f,1.0f));
    this->addChild(highLabel, 10);
    
// Added by Louis
    // add total coin
    Sprite* img_coin = Sprite::create("diamond.png");
    img_coin->setScale(0.3);
    img_coin->setAnchorPoint(Vec2(1, 0.5));
    img_coin->setPosition(Vec2(visibleSize.width * 0.75, visibleSize.height * 0.95));
    this->addChild(img_coin, 10);
    
    char _totalcoin[256];
    sprintf(_totalcoin, "%d", Universal::TOTAL_COIN);
    auto totalLabel = Label::createWithTTF(_totalcoin, "Nexa Bold.otf", 40);
    totalLabel->setColor(Color3B(255,255,255));
    totalLabel->setAnchorPoint(Vec2(0, 0.5));
    totalLabel->setPosition(img_coin->getPosition());
    totalLabel->setTag(COINLABEL_TAG);
    this->addChild(totalLabel, 10);
// Add End
    
    
    // logo
    auto logo = Sprite::create("logo.png");
    logo->setPosition(Vec2(visibleSize.width/2, visibleSize.height/1.17) );
    this->addChild(logo,5);
    
    
    // add background
    auto background = Sprite::create("gameoverbackground.png");
    background->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(background, 0);
    
    createEmojiScrollView();
    
//    makeEmojiScroll();
    
    // create scroll button button
    auto btn_next = MenuItemImage::create(
                                      "next.png",
                                      "next.png",
                                      CC_CALLBACK_1(IntroScene::showNextEmojis, this));
    
    btn_next->setPosition(Vec2(visibleSize.width - btn_next->getContentSize().width * 0.5 - 10, visibleSize.height / 1.8));
    
    auto btn_prev = MenuItemImage::create(
                                          "back.png",
                                          "back.png",
                                          CC_CALLBACK_1(IntroScene::showPreviousEmojis, this));
    
    btn_prev->setPosition(Vec2(btn_next->getContentSize().width * 0.5 + 10, visibleSize.height / 1.8));
    Menu* emoji_menu = Menu::create(btn_next,
                                    btn_prev,
                                    NULL);
    emoji_menu->setPosition(Vec2::ZERO);
    this->addChild(emoji_menu);
    
    return true;
}

void IntroScene::createEmojiScrollView()
{    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Size scollFrameSize = Size(visibleSize.width * 4 / 5, visibleSize.height / 7.0);
    auto scrollView = cocos2d::ui::ScrollView::create();
    scrollView->setContentSize(scollFrameSize);
    scrollView->setOpacity(0);
    scrollView->setPosition(Point(visibleSize.width / 2, visibleSize.height/1.8));
    scrollView->setDirection(cocos2d::ui::ScrollView::Direction::HORIZONTAL);
    scrollView->setBounceEnabled(true);
    scrollView->setTouchEnabled(true);
    auto containerSize = Size(scollFrameSize.width * 7.5, scollFrameSize.height);
    scrollView->setInnerContainerSize(containerSize);
    scrollView->setAnchorPoint(Vec2(0.5, 0.5));
    scrollView->setTag(SCROLL_TAG);
    this->addChild(scrollView);
    
    for (int i = 0; i < EmojiCount; i++) {
        String* filename = String::createWithFormat("emoji%d.png", i);
        int cost = 0;
        if (i > 0 && i <= 7) {
            cost = 50;
        }else if(i >= 8 && i <= 20)
        {
            cost = 100;
        }else if (i >= 21 && i <= 28)
        {
            cost = 250;
        }
        ScrollItem* item_1 = ScrollItem::create(cost, filename->getCString(), filename->getCString(), 20 + i);
        item_1->setPosition(Point(item_1->getContentSize().width * 0.55 + containerSize.width * i / EmojiCount, containerSize.height * 0.67));
        item_1->setTag(i);
        scrollView->addChild(item_1);
    }
    int active_index = UserDefault::getInstance()->getIntegerForKey(kEmogjiname, 0);
    updateActive(active_index);
    showActiveEmoji(active_index);
}

// replay
void IntroScene::menuPlayCallback(Ref* pSender)
{
    // set score to 0
    Universal::SCORE=0;
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // push game scene
    Director::getInstance()->pushScene(GameScene::createScene());
    
}

// rate
void IntroScene::menuRateCallback(Ref* pSender)
{
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // open url of game in App Store - see RootViewController.mm to change link url to your app
    SendMessageWithParams(string("openUrl"), NULL);
    
}

// leaderboard
void IntroScene::menuLeaderboardCallback(Ref* pSender)
{
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // open gamecenter - see Config.h to change leaderboard IDs
    IOSManager::shared()->showLeaderBoard();
    
}

// remove ads button
void IntroScene::menuNoAdsCallback(Ref* pSender)
{
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // open purchase AppController.mm
    SendMessageWithParams(string("openPurchase"), NULL);
    
}

// share button
void IntroScene::menuShareCallback(Ref* pSender)
{
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    string s = to_string(Universal::HIGH_SCORE);
    Dictionary* prms = Dictionary::create();
    prms->setObject(CCString::create(s), "score" );
    
    // open share in AppController.mm and send score
    SendMessageWithParams(string("openShare"), prms);
    
}

// more games button
void IntroScene::menuMoreCallback(Ref* pSender)
{
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    SendMessageWithParams(string("LoadMore"), NULL);
    
}

// store button
void IntroScene::menuStoreCallback(cocos2d::Ref *pSender)
{
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    showStoreDialog();
}

void IntroScene::showStoreDialog()
{
    
    ((Menu*)this->getChildByTag(MENU_TAG))->setEnabled(false);
    
    Layer* storedialog = StoreDialogLayer::create();
    storedialog->setTag(STORE_DLG_TAG);
    this->addChild(storedialog, 100);
}

void IntroScene::updateScore()
{
    char score[10];
    sprintf(score, "%d", Universal::TOTAL_COIN);
    Label* lbl_score = (Label*)this->getChildByTag(COINLABEL_TAG);
    lbl_score->setString(score);
    
    UserDefault::getInstance()->setIntegerForKey(kTotalCoin, Universal::TOTAL_COIN);
}

void IntroScene::updateActive(int index)
{
    ui::ScrollView* scrollView = (ui::ScrollView*)this->getChildByTag(SCROLL_TAG);
    for (int idx = 0; idx < EmojiCount; idx++) {
        ScrollItem* item = (ScrollItem*)scrollView->getChildByTag(idx);
        Sprite* active_img = (Sprite*)item->getChildByTag(ACTIVE_IMG_TAG);
        if (idx == index) {
            active_img->setVisible(true);
        }else
        {
            active_img->setVisible(false);
        }
    }
}

void IntroScene::makeEmojiScroll()
{
    LayerColor* emoji_layer = LayerColor::create(Color4B(0, 0, 0, 255));
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Size scrollFrameSize = Size(visibleSize.width * 4 / 5, visibleSize.height / 7.2);
    Size containerSize = Size(scrollFrameSize.width * 7.5, scrollFrameSize.height);
    emoji_layer->setContentSize(scrollFrameSize);
    emoji_layer->setPosition(Point(0, visibleSize.height/1.7));
    emoji_layer->setAnchorPoint(Vec2(0.5, 0.5));
    emoji_layer->setColor(Color3B(255,255,0));
    emoji_layer->setScale(0.7);
    emoji_layer->setTag(SCROLL_TAG);
//    emoji_layer->setTouchEnabled(true);
    
    
    
    this->addChild(emoji_layer);
    
    for (int i = 0; i < EmojiCount; i++) {
        String* filename = String::createWithFormat("emoji%d.png", i);
        int cost = Universal::getItemPrice(i);
        
        ScrollItem* item_1 = ScrollItem::create(cost, filename->getCString(), filename->getCString(), 20 + i);
        item_1->setPosition(Point(item_1->getContentSize().width * 0.5 + containerSize.width * i / EmojiCount, containerSize.height * 0.67));
        item_1->setTag(i);
        emoji_layer->addChild(item_1);
    }
}

void IntroScene::showNextEmojis(cocos2d::Ref *pSender)
{
    cocos2d::ui::ScrollView *scrollview = (cocos2d::ui::ScrollView*)this->getChildByTag(SCROLL_TAG);
    
    auto inner = scrollview->getInnerContainer();
    auto curPos = inner->getPosition();
    float offsetX = scrollview->getContentSize().width;
    auto nexPos = Vec2(curPos.x + offsetX, curPos.y);
    auto topMostX = scrollview->getContentSize().width - inner->getContentSize().width;
    
    float cur_percent = curPos.x / topMostX * 100;
    
    scrollview->scrollToPercentHorizontal(cur_percent + 10, 0.5, true);
}

void IntroScene::showPreviousEmojis(cocos2d::Ref *pSender)
{
    cocos2d::ui::ScrollView *scrollview = (cocos2d::ui::ScrollView*)this->getChildByTag(SCROLL_TAG);
    
    auto inner = scrollview->getInnerContainer();
    auto curPos = inner->getPosition();
    float offsetX = scrollview->getContentSize().width;
    auto nexPos = Vec2(curPos.x + offsetX, curPos.y);
    auto topMostX = scrollview->getContentSize().width - inner->getContentSize().width;
    
    float cur_percent = curPos.x / topMostX * 100;
    
    scrollview->scrollToPercentHorizontal(cur_percent - 10, 0.5, true);
}

void IntroScene::showActiveEmoji(int index)
{
    cocos2d::ui::ScrollView *scrollview = (cocos2d::ui::ScrollView*)this->getChildByTag(SCROLL_TAG);
    ScrollItem* item = (ScrollItem*)scrollview->getChildByTag(index);
    
    auto inner = scrollview->getInnerContainer();
    float curPosX = scrollview->getContentSize().width - item->getPosition().x - item->getContentSize().width * 3;
    auto topMostX = scrollview->getContentSize().width - inner->getContentSize().width;
    
    float cur_percent = curPosX / topMostX * 100;
    
    scrollview->scrollToPercentHorizontal(cur_percent, 0.5, false);
}