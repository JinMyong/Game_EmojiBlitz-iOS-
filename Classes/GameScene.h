#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#import "Box2D/Box2D.h"

#define PTM_RATIO 32.0

USING_NS_CC;

class GameScene : public Layer
{
    
public:
    
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    
    void actionDone(Ref* pSender);
    void spriteMoveFinished(Ref* pSender);
    void update(float dt);
    void diamondPop();
    
    cocos2d::Size visibleSize;
    Vec2 origin;
    
    Sprite* fingerprint;
    Sprite* gosprite;
    Sprite* diamond;
    
    Label* scoreLabel;
    
    int touchcount;
    bool diamondPopped;
    bool gameoversound;
    bool gameover;
    bool calledreward;
    
    int _currentTouchID;
    int scoreshowing;
    
    b2World *_worldb2;
    b2Body *_groundBody;
    
    b2BodyDef blockBodyDef;
    b2BodyDef block2BodyDef;
    b2BodyDef block3BodyDef;
    b2BodyDef block4BodyDef;
    
    b2Body* blockBody;
    b2Body* block2Body;
    b2Body* block3Body;
    b2Body* block4Body;
    
    b2Fixture *_bottomFixture;
    b2Fixture *_blockFixture;
    b2Fixture *_block2Fixture;
    b2Fixture *_block3Fixture;
    b2Fixture *_block4Fixture;
    
    CREATE_FUNC(GameScene);
    
    
    PhysicsWorld* _world;
    
    void setPhysicWorld(PhysicsWorld* world) { _world = world; }

    void setPhysicsBody(PhysicsBody* body);
    
    bool onContactBegin(const PhysicsContact& contact);
    
    
    Sprite *emoji;
    Sprite *emojitop;

    Sprite *enemy;
    Sprite *enemy2;
    Sprite *enemy3;
    Sprite *enemy4;

    
    PhysicsBody *emojibody;
    PhysicsBody *emojitopbody;
    PhysicsBody *diamondbody;

    PhysicsBody *enemyBody;
    PhysicsBody *enemy2Body;
    PhysicsBody *enemy3Body;
    PhysicsBody *enemy4Body;

    std::unordered_map<int, Node*> _mouses;
    
    Node* _wallNode;


};

#endif // __GAME_SCENE_H__
