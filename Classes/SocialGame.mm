//
//  SocialGame.m
//  AlephBet
//
//  Created by Louis on 2/1/16.
//
//

#include "SocialGame.h"
#include "AppController.h"
#include "config.h"
#include "Universal.h"
#include "IntroScene.h"
#include "GameOverScene.h"
#include "StoreDialogLayer.hpp"

bool SocialGame::showRewardVideo()
{
    AppController* appDel = (AppController*)[UIApplication sharedApplication].delegate;
    bool retValue = false;
    if (appDel != nil) {
        retValue = [appDel loadRewardVideo];
    }
    return retValue;
}

void SocialGame::getCoin(int cost)
{
    AppController* appDel = (AppController*)[UIApplication sharedApplication].delegate;
    switch (cost) {
        case 100:
            [appDel getCoinPurchase:kGetCoin100];
            break;
        case 250:
            [appDel getCoinPurchase:kGetCoin250];
            break;
        case 500:
            [appDel getCoinPurchase:kGetCoin500];
            break;
        case 5000:
            [appDel getCoinPurchase:kGetCoin5000];
            break;            
        default:
            break;
    }
}

void SocialGame::completeGetCoin(int coins)
{
    Universal::TOTAL_COIN += coins;
    
    if (Universal::IAP_ITEM_INDEX != -1) {
        int item_cost = Universal::getItemPrice(Universal::IAP_ITEM_INDEX);
        
        if (Universal::TOTAL_COIN < item_cost) {
            MessageBox(txtMoreCoin, "");
            Universal::IAP_ITEM_INDEX = -1;
        }else
        {
            char keyname[20];
            sprintf(keyname, "%s%d", kBoughtItem, Universal::IAP_ITEM_INDEX);
            
//            UserDefault::getInstance()->setBoolForKey(keyname, true);
            Universal::selectEmoji(Universal::IAP_ITEM_INDEX + 20);
            Universal::IAP_ITEM_INDEX = -1;
        }
    }
    
    Scene* currentScene = Director::getInstance()->getRunningScene();
    Layer* mainlayer = (Layer*)currentScene->getChildByTag(LAYER_TAG);
    if (mainlayer != NULL) {
        if (dynamic_cast<IntroScene*>(mainlayer) ) {
            IntroScene* current = (IntroScene*)mainlayer;
            current->updateScore();
        }else if (dynamic_cast<GameOverScene*>(mainlayer))
        {
            GameOverScene* current = (GameOverScene*) mainlayer;
            current->updateScore();
        }
    }    
}

void SocialGame::completeRewardVideo()
{
    if (Universal::isGetFreeCoins) {
        Universal::isGetFreeCoins = false;
        Scene* currentScene = Director::getInstance()->getRunningScene();
        Layer* mainlayer = (Layer*)currentScene->getChildByTag(LAYER_TAG);
        if (mainlayer != NULL) {
            if (dynamic_cast<IntroScene*>(mainlayer) ) {
                IntroScene* current = (IntroScene*)mainlayer;
                StoreDialogLayer* dialog = (StoreDialogLayer*)current->getChildByTag(STORE_DLG_TAG);
                dialog->completeGetFreecoins();
            }else if (dynamic_cast<GameOverScene*>(mainlayer))
            {
                GameOverScene* current = (GameOverScene*) mainlayer;
                StoreDialogLayer* dialog = (StoreDialogLayer*)current->getChildByTag(STORE_DLG_TAG);
                dialog->completeGetFreecoins();
            }
        }
    }    
}

void SocialGame::confirmMessage()
{
    AppController* appDel = (AppController*)[UIApplication sharedApplication].delegate;
    [appDel showConfirmAlert];
}

void SocialGame::getConfirmResult(bool result)
{
    if (result) {
        Universal::selectEmoji(Universal::selectedEmojiIndex);
    }
}