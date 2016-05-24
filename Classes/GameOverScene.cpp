#include "GameOverScene.h"
#include "GameScene.h"
#include "IntroScene.h"
#include "Universal.h"
#include "NDKHelper.h"
#include "config.h"
#include "SimpleAudioEngine.h"
#include "StoreDialogLayer.hpp"
#include "ScrollItem.hpp"

#include "CocosGUI.h"
#include "cocos-ext.h"

using namespace CocosDenshion;

Scene* GameOverScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameOverScene::create();
    layer->setTag(LAYER_TAG);
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool GameOverScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    // get visible size
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // get number of games played
    UserDefault *userDefaults = UserDefault::getInstance();
    Universal::GAMESPLAYED = userDefaults->getIntegerForKey("GAMESPLAYED");
    
    // increment and set games played
    ++Universal::GAMESPLAYED;
    userDefaults->setIntegerForKey("GAMESPLAYED", Universal::GAMESPLAYED);
    userDefaults->flush();
    
    // send score to game center
    IOSManager::shared()->sendScore(Universal::SCORE, LeaderBoardIDHighScore);
    
    // set how often to show chartboost in config.h
    if(Universal::GAMESPLAYED % showChartboost==0){
        
        SendMessageWithParams(string("LoadInterstitial"), NULL);
        
    }
    
    // create retry button
    auto play = MenuItemImage::create(
                                           "playagainbutton.png",
                                           "playagainbutton.png",
                                           CC_CALLBACK_1(GameOverScene::menuPlayCallback, this));
    
    play->setPosition(Vec2(visibleSize.width * 1 / 5, visibleSize.height / 2.9));
    
    // create store button
    auto store = MenuItemImage::create(
                                       "store.png",
                                       "store.png",
                                       CC_CALLBACK_1(GameOverScene::menuStoreCallback, this));
    
    store->setPosition(Vec2(visibleSize.width * 2/ 5, play->getPosition().y));
    
    // create remove ads button
    auto removeads = MenuItemImage::create(
                                             "noads.png",
                                             "noads.png",
                                             CC_CALLBACK_1(GameOverScene::menuNoAdsCallback, this));
    
    removeads->setPosition(Vec2(visibleSize.width * 4 / 5, play->getPosition().y));
    
    // create remove ads button
    auto share = MenuItemImage::create(
                                           "share.png",
                                           "share.png",
                                           CC_CALLBACK_1(GameOverScene::menuShareCallback, this));
    
    share->setPosition(Vec2(visibleSize.width * 3 / 5, play->getPosition().y));

    // create rate button
    auto rate = MenuItemImage::create(
                                      "rate.png",
                                      "rate.png",
                                      CC_CALLBACK_1(GameOverScene::menuRateCallback, this));
    
    rate->setPosition(Vec2(visibleSize.width / 4, play->getPosition().y - rate->getContentSize().height * 1.25));
    
    // create more games button
    auto more = MenuItemImage::create(
                                      "moregames.png",
                                      "moregames.png",
                                      CC_CALLBACK_1(GameOverScene::menuMoreCallback, this));
    
    more->setPosition(Vec2(visibleSize.width * 3 / 4 , play->getPosition().y - more->getContentSize().height * 1.25));
    
    
    // create leaderboard button
    auto leaderboard = MenuItemImage::create(
                                             "leaderboard.png",
                                             "leaderboard.png",
                                             CC_CALLBACK_1(GameOverScene::menuLeaderboardCallback, this));
    
    leaderboard->setPosition(Vec2(visibleSize.width / 2, play->getPositionY() - store->getContentSize().height * 1.25));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(play, rate, leaderboard, removeads, share, more, store, NULL);
    menu->setPosition(Vec2::ZERO);
    menu->setTag(MENU_TAG);
    this->addChild(menu, 1);
    
    
    // add score
    char _score[256];
    sprintf(_score, "%d", Universal::SCORE);
    std::string strScore = "";
    strScore.append(_score);
    const char * charScore = strScore.c_str();
    auto scoreLabel = Label::createWithTTF(charScore, "Nexa Bold.otf", 100);
    scoreLabel->setColor(Color3B(255,255,255));
    scoreLabel->setAnchorPoint(Vec2(0.5f,0.5f));
    scoreLabel->setPosition( Vec2(visibleSize.width/2, visibleSize.height/1.35 ) );
    scoreLabel->setVisible(true);
    this->addChild(scoreLabel, 10);
    
    // add high score
    char _highscore[256];
    sprintf(_highscore, "%d", Universal::HIGH_SCORE);
    std::string strHighscore = "BEST: ";
    strHighscore.append(_highscore);
    const char * charHighscore = strHighscore.c_str();
    auto highLabel = Label::createWithTTF(charHighscore, "Nexa Bold.otf",55);
    highLabel->setColor(Color3B(255,255,255));
    highLabel->setPosition( Vec2(visibleSize.width/2,  visibleSize.height/1.45) );
    highLabel->setAnchorPoint(Vec2(0.5f,1.0f));
    this->addChild(highLabel, 10);
    
// Added by Louis
    // add total coin
    Sprite* img_coin = Sprite::create("diamond.png");
    img_coin->setScale(0.3);
    img_coin->setAnchorPoint(Vec2(1, 0.5));
    img_coin->setPosition(Vec2(visibleSize.width * 0.8, visibleSize.height * 0.95));
    this->addChild(img_coin, 10);
    
    char _totalcoin[256];
    sprintf(_totalcoin, "%d", Universal::TOTAL_COIN);
    auto totalLabel = Label::createWithTTF(_totalcoin, "Nexa Bold.otf",40);
    totalLabel->setColor(Color3B(255,255,255));
    totalLabel->setAnchorPoint(Vec2(0, 0.5));
    totalLabel->setPosition(img_coin->getPosition());
    totalLabel->setTag(COINLABEL_TAG);
    this->addChild(totalLabel, 10);
// Add End
    
    // game over
    auto go = Sprite::create("gameoverWhite.png");
    go->setPosition(Vec2(visibleSize.width/2, visibleSize.height/1.17) );
    go->setScale(0.9);
    this->addChild(go,5);
    
    
    // add background
    auto background = Sprite::create("gameoverbackground.png");
    background->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(background, 0);
    
    createEmojiScrollView();
    
    auto btn_next = MenuItemImage::create(
                                          "next.png",
                                          "next.png",
                                          CC_CALLBACK_1(GameOverScene::showNextEmojis, this));
    
    btn_next->setPosition(Vec2(visibleSize.width - btn_next->getContentSize().width * 0.5 - 10, visibleSize.height / 1.8));
    
    auto btn_prev = MenuItemImage::create(
                                          "back.png",
                                          "back.png",
                                          CC_CALLBACK_1(GameOverScene::showPreviousEmojis, this));
    
    btn_prev->setPosition(Vec2(btn_next->getContentSize().width * 0.5 + 10, visibleSize.height / 1.8));
    Menu* emoji_menu = Menu::create(btn_next,
                                    btn_prev,
                                    NULL);
    emoji_menu->setPosition(Vec2::ZERO);
    this->addChild(emoji_menu);
    
    return true;
}

// replay
void GameOverScene::menuPlayCallback(Ref* pSender)
{
    // set score to 0
    Universal::SCORE=0;
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // push game scene
    Director::getInstance()->pushScene(GameScene::createScene());
    
}

// rate
void GameOverScene::menuRateCallback(Ref* pSender)
{
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // open url of game in App Store - see RootViewController.mm to change link url to your app
    SendMessageWithParams(string("openUrl"), NULL);
    
}

// leaderboard
void GameOverScene::menuLeaderboardCallback(Ref* pSender)
{
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // open gamecenter - see Config.h to change leaderboard IDs
    IOSManager::shared()->showLeaderBoard();
    
}

// remove ads button
void GameOverScene::menuNoAdsCallback(Ref* pSender)
{
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    // open purchase AppController.mm
    SendMessageWithParams(string("openPurchase"), NULL);
    
}

// share button
void GameOverScene::menuShareCallback(Ref* pSender)
{
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    string s = to_string(Universal::SCORE);
    Dictionary* prms = Dictionary::create();
    prms->setObject(CCString::create(s), "score" );
    
    // open share in AppController.mm and send score
    SendMessageWithParams(string("openShare"), prms);
    
}

// more games button
void GameOverScene::menuMoreCallback(Ref* pSender)
{
    
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");

    SendMessageWithParams(string("LoadMore"), NULL);
    
}

// store button
void GameOverScene::menuStoreCallback(cocos2d::Ref *pSender)
{
    // click sound
    SimpleAudioEngine::getInstance()->playEffect("ButtonClick.mp3");
    
    showStoreDialog();
}

void GameOverScene::showStoreDialog()
{
    ((Menu*)this->getChildByTag(MENU_TAG))->setEnabled(false);
    
    Layer* storedialog = StoreDialogLayer::create();
    storedialog->setTag(STORE_DLG_TAG);
    this->addChild(storedialog, 100);
}

void GameOverScene::createEmojiScrollView()
{
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Size scrollFrameSize = Size(visibleSize.width * 4 / 5, visibleSize.height / 7.0);
    auto scrollView = cocos2d::ui::ScrollView::create();
    scrollView->setContentSize(scrollFrameSize);
    scrollView->setOpacity(0);
    scrollView->setPosition(Point(visibleSize.width / 2, visibleSize.height/1.8));
    scrollView->setDirection(cocos2d::ui::ScrollView::Direction::HORIZONTAL);
    scrollView->setBounceEnabled(true);
    scrollView->setTouchEnabled(true);
    auto containerSize = Size(scrollFrameSize.width * 7.5, scrollFrameSize.height);
    scrollView->setInnerContainerSize(containerSize);
    scrollView->setAnchorPoint(Vec2(0.5, 0.5));
    scrollView->setTag(SCROLL_TAG);
    this->addChild(scrollView);
    
    for (int i = 0; i < EmojiCount; i++) {
        String* filename = String::createWithFormat("emoji%d.png", i);
        int cost = Universal::getItemPrice(i);
        
        ScrollItem* item_1 = ScrollItem::create(cost, filename->getCString(), filename->getCString(), 20 + i);
        item_1->setPosition(Point(item_1->getContentSize().width * 0.5 + containerSize.width * i / EmojiCount, containerSize.height * 0.67));
        item_1->setTag(i);
        scrollView->addChild(item_1);
    }
    
    int active_index = UserDefault::getInstance()->getIntegerForKey(kEmogjiname, 0);
    updateActive(active_index);
    showActiveEmoji(active_index);
}

void GameOverScene::updateScore()
{
    char score[10];
    sprintf(score, "%d", Universal::TOTAL_COIN);
    Label* lbl_score = (Label*)this->getChildByTag(COINLABEL_TAG);
    lbl_score->setString(score);
    
    UserDefault::getInstance()->setIntegerForKey(kTotalCoin, Universal::TOTAL_COIN);
}

void GameOverScene::updateActive(int index)
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

void GameOverScene::showNextEmojis(cocos2d::Ref *pSender)
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

void GameOverScene::showPreviousEmojis(cocos2d::Ref *pSender)
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

void GameOverScene::showActiveEmoji(int index)
{
    cocos2d::ui::ScrollView *scrollview = (cocos2d::ui::ScrollView*)this->getChildByTag(SCROLL_TAG);
    ScrollItem* item = (ScrollItem*)scrollview->getChildByTag(index);
    
    auto inner = scrollview->getInnerContainer();
    float curPosX = scrollview->getContentSize().width - item->getPosition().x - item->getContentSize().width * 3;
    auto topMostX = scrollview->getContentSize().width - inner->getContentSize().width;
    
    float cur_percent = curPosX / topMostX * 100;
    
    scrollview->scrollToPercentHorizontal(cur_percent, 0.5, false);
}
