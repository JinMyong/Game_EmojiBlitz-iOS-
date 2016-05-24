//
//  StoreDialogLayer.cpp
//  emojimuncher
//
//  Created by Louis on 2/25/16.
//
//

#include "StoreDialogLayer.hpp"
#include "SocialGame.h"
#include "Universal.h"
#include "config.h"
#include "IntroScene.h"
#include "GameOverScene.h"

bool StoreDialogLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    // get visible size
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    Sprite* bg = Sprite::create("blankBackground.png");
    bg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(bg, 18);
    
    Sprite* dialog = Sprite::create("storedialog.png");
    dialog->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2.2));
    this->addChild(dialog, 19);
    
    MenuItemImage* freebutton = MenuItemImage::create(
                                                      "freecoins.png",
                                                      "freecoins.png",
                                                      CC_CALLBACK_1(StoreDialogLayer::menuFreeCoinCallback, this));
    //    freebutton->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 1.5));
    
    MenuItemImage* coin_100 = MenuItemImage::create(
                                                    "coins_100.png",
                                                    "coins_100.png",
                                                    CC_CALLBACK_1(StoreDialogLayer::menuPurchaseCoinCallback, this));
    coin_100->setTag(BTN_COIN_100);
    
    MenuItemImage* coin_250 = MenuItemImage::create(
                                                    "coins_250.png",
                                                    "coins_250.png",
                                                    CC_CALLBACK_1(StoreDialogLayer::menuPurchaseCoinCallback, this));
    coin_250->setTag(BTN_COIN_250);
    
    MenuItemImage* coin_500 = MenuItemImage::create(
                                                    "coins_500.png",
                                                    "coins_500.png",
                                                    CC_CALLBACK_1(StoreDialogLayer::menuPurchaseCoinCallback, this));
    coin_500->setTag(BTN_COIN_500);
    
    MenuItemImage* coin_5000 = MenuItemImage::create(
                                                     "coins_5000.png",
                                                     "coins_5000.png",
                                                     CC_CALLBACK_1(StoreDialogLayer::menuPurchaseCoinCallback, this));
    coin_5000->setTag(BTN_COIN_5000);
    
    
    
    Menu* menu = Menu::create(freebutton,
                              coin_100,
                              coin_250,
                              coin_500,
                              coin_5000,
                              NULL);
    menu->alignItemsVerticallyWithPadding(20);
    menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2.2));
    this->addChild(menu, 20);
    
    MenuItemImage* close_item = MenuItemImage::create(
                                                      "close.png",
                                                      "close.png",
                                                      CC_CALLBACK_1(StoreDialogLayer::menuCloseCallback, this));
    close_item->setPosition(Vec2(dialog->getPositionX() + dialog->getContentSize().width * 0.5, dialog->getPositionY() + dialog->getContentSize().height * 0.5));
    Menu* close_menu = Menu::create(close_item, NULL);
    close_menu->setPosition(Vec2::ZERO);
    this->addChild(close_menu, 25);
    
    return true;
}

void StoreDialogLayer::menuFreeCoinCallback(cocos2d::Ref *pSender)
{
    Universal::isGetFreeCoins = true;
    bool result = SocialGame::showRewardVideo();
    if (result) {
        
    }else{
        Universal::isGetFreeCoins = false;
        MessageBox("Video Ads is not ready. Please try again later", "");
//        Layer* layer = (Layer*)this->getParent();
//        ((Menu*)layer->getChildByTag(MENU_TAG))->setEnabled(true);
//        this->removeFromParentAndCleanup(true);
    }
}

void StoreDialogLayer::menuPurchaseCoinCallback(cocos2d::Ref *pSender)
{
    int button_index = ((MenuItem*)pSender)->getTag();
    int cost = 100;
    switch (button_index) {
        case BTN_COIN_100:
            cost = 100;
            break;
        case BTN_COIN_250:
            cost = 250;
            break;
        case BTN_COIN_500:
            cost = 500;
            break;
        case BTN_COIN_5000:
            cost = 5000;
            break;
        default:
            break;
    }
    SocialGame::getCoin(cost);
    Node* parent = this->getParent();
    ((Menu*)parent->getChildByTag(MENU_TAG))->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}

void StoreDialogLayer::menuCloseCallback(cocos2d::Ref *pSender)
{
    Node* parent = this->getParent();
    ((Menu*)parent->getChildByTag(MENU_TAG))->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}

void StoreDialogLayer::completeGetFreecoins()
{
    Universal::TOTAL_COIN += vRewardVideoPrice;
    Universal::isGetFreeCoins = false;
    
    Layer* layer = (Layer*)this->getParent();
    if (dynamic_cast<IntroScene*>(layer) ) {
        IntroScene* current = (IntroScene*)layer;
        current->updateScore();
    }else if (dynamic_cast<GameOverScene*>(layer))
    {
        GameOverScene* current = (GameOverScene*) layer;
        current->updateScore();
    }
    ((Menu*)layer->getChildByTag(MENU_TAG))->setEnabled(true);
    this->removeFromParentAndCleanup(true);
}