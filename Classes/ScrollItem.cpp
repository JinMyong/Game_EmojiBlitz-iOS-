//
//  ScrollItem.cpp
//  emojimuncher
//
//  Created by Louis on 2/25/16.
//
//

#include "ScrollItem.hpp"
#include "CocosGUI.h"
#include "cocos-ext.h"
#include "Universal.h"
#include "config.h"
#include "SocialGame.h"


ScrollItem* ScrollItem::create(int cost, const char* nor_fileName, const char* prs_fileName, int index)
{
    auto node = Node::create();
    
    
    int emojiIndex = index - 20;
    
    char keyname[20];
    sprintf(keyname, "%s%d", kBoughtItem, emojiIndex);
    
    auto button = ui::Button::create();
    button->setTouchEnabled(true);
    button->ignoreContentAdaptWithSize(false);
    button->setContentSize(Size(100, 100));
    button->loadTextures(nor_fileName, prs_fileName);
    button->setTag(index);
    button->addTouchEventListener( [](Ref* pSender, cocos2d::ui::Widget::TouchEventType type) {
        if (type == cocos2d::ui::Widget::TouchEventType::ENDED)
        {
            auto button = (ui::Button*)pSender;
            int tag = button->getTag();
            log("Item is Clicked %d", tag);
            
            char keyname[20];
            sprintf(keyname, "%s%d", kBoughtItem, tag - 20);
            
            bool isBought = UserDefault::getInstance()->getBoolForKey(keyname, false);
            if (isBought) {
                Universal::selectEmoji(tag);
            }else
            {
                Universal::selectedEmojiIndex = tag;
                SocialGame::confirmMessage();
            }
        }
    } );
    node->addChild(button);
    
    Sprite* active_img = Sprite::create("active.png");
    active_img->setPosition(button->getPosition());
    active_img->setAnchorPoint(Vec2(0.5, 0.5));
    active_img->setScale(0.7);
    active_img->setTag(ACTIVE_IMG_TAG);
    node->addChild(active_img);
    
    char cost_str[10];
    sprintf(cost_str, "%d", cost);
    
    auto lbl_cost = Label::createWithTTF(cost_str, "Nexa Bold.otf",30);
    lbl_cost->setColor(Color3B(255,255,255));
    lbl_cost->setPosition(Vec2(button->getPositionX(), button->getPositionY() - button->getContentSize().height * 0.7));
    lbl_cost->setAnchorPoint(Vec2(0,1.0f));
    lbl_cost->setTag(ITEM_LBL_TAG);
    node->addChild(lbl_cost, 10);
    
    Sprite* img_coin = Sprite::create("diamond.png");
    img_coin->setScale(0.25);
    img_coin->setPosition(Vec2(button->getPositionX(), button->getPositionY() - button->getContentSize().height * 0.7));
    img_coin->setAnchorPoint(Vec2(1, 1));
    img_coin->setTag(ITEM_COIN__TAG);
    node->addChild(img_coin, 10);
    
    node->setContentSize(Size(button->getContentSize().width, button->getContentSize().height + img_coin->getContentSize().height));
    
    bool isBought = UserDefault::getInstance()->getBoolForKey(keyname, false);
    if (cost <= 0 || isBought) {
        img_coin->setVisible(false);
        lbl_cost->setVisible(false);
    }
    
    return (ScrollItem*)node;
}


