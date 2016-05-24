#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "Universal.h"
#include "GameOverScene.h"
#include "NDKHelper.h"
#include "config.h"
static const int DRAG_BODYS_TAG = 0x80;

Scene* GameScene::createScene()
{
    auto scene = Scene::createWithPhysics();
    scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
    scene->getPhysicsWorld()->setGravity(Vect(0, 0));
    auto layer = GameScene::create();
    layer->setPhysicWorld(scene->getPhysicsWorld());
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
    
    if ( !Layer::init() )
    {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    Size winSize = Director::getInstance()->getWinSize();
    
    // get highscore and games played
    UserDefault *userDefaults = UserDefault::getInstance();
    Universal::HIGH_SCORE = userDefaults->getIntegerForKey("HIGHSCORE");
    userDefaults->flush();
    Universal::GAMESPLAYED = userDefaults->getIntegerForKey("GAMESPLAYED");
    userDefaults->flush();
    
    if (Universal::SCORE==0) {
        SendMessageWithParams(string("CacheRewards"), NULL);
        Universal::REWARD_USED=false;
    }
    else{
        Universal::REWARD_USED=true;
    }
    
    // start background music
    if (Universal::isMusicPlaying == false) {
        
        // uncomment this line for background music
        //CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("background.mp3",true);
        Universal::isMusicPlaying=true;
        
    }
    
    gameover=false;
    calledreward=false;

    // Create a world
    b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
    _worldb2 = new b2World(gravity);
    
    // Create edges around the entire screen
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0,winSize.height/2/PTM_RATIO);
    _groundBody = _worldb2->CreateBody(&groundBodyDef);
    
    b2EdgeShape groundBox;
    b2FixtureDef groundBoxDef;
    groundBoxDef.shape = &groundBox;
    
    groundBox.Set(b2Vec2(0,0), b2Vec2(winSize.width/PTM_RATIO, 0));
    _bottomFixture = _groundBody->CreateFixture(&groundBoxDef);
    groundBox.Set(b2Vec2(0,0), b2Vec2(0, winSize.height/2/PTM_RATIO));
    _groundBody->CreateFixture(&groundBoxDef);
    groundBox.Set(b2Vec2(0, winSize.height/2/PTM_RATIO), b2Vec2(winSize.width/PTM_RATIO,winSize.height/2/PTM_RATIO));
    _groundBody->CreateFixture(&groundBoxDef);
    groundBox.Set(b2Vec2(winSize.width/PTM_RATIO, winSize.height/PTM_RATIO),b2Vec2(winSize.width/PTM_RATIO, 0));
    _groundBody->CreateFixture(&groundBoxDef);


    // Enable touches
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);;
    
    
//    auto body = PhysicsBody::createEdgeBox(Size(visibleSize.width,visibleSize.height/2),PHYSICSBODY_MATERIAL_DEFAULT,3);
//    _wallNode = Node::create();
//    body->setGroup(1);
//    _wallNode->setPosition(Point(visibleSize.width/2,visibleSize.height-visibleSize.height/4));
//    _wallNode->setPhysicsBody(body);
//    this->addChild(_wallNode);
//    
    
    
    
    auto bottomBody = PhysicsBody::createBox(Size(visibleSize.width, visibleSize.height/2));
    auto _bottomNode2 = Node::create();
    bottomBody->setGroup(1);
    _bottomNode2->setPosition(Point(visibleSize.width/2,visibleSize.height/4));
    _bottomNode2->setPhysicsBody(bottomBody);
    bottomBody->setDynamic(false);
    this->addChild(_bottomNode2);
    
    
    auto topBody = PhysicsBody::createBox(Size(visibleSize.width, visibleSize.height/2));
    auto _topNode = Node::create();
    topBody->setGroup(1);
    _topNode->setPosition(Point(visibleSize.width/2,visibleSize.height+visibleSize.height/4));
    _topNode->setPhysicsBody(topBody);
    topBody->setDynamic(false);
    this->addChild(_topNode);
    
    auto rightBody = PhysicsBody::createBox(Size(visibleSize.width, visibleSize.height*2));
    auto _rightNode = Node::create();
    rightBody->setGroup(1);
    _rightNode->setPosition(Point(visibleSize.width+visibleSize.width/2,visibleSize.height/2));
    _rightNode->setPhysicsBody(rightBody);
    rightBody->setDynamic(false);
    this->addChild(_rightNode);
    
    
    auto leftBody = PhysicsBody::createBox(Size(visibleSize.width, visibleSize.height*2));
    auto _leftNode = Node::create();
    leftBody->setGroup(1);
    _leftNode->setPosition(Point(origin.x-visibleSize.width/2,visibleSize.height/2));
    _leftNode->setPhysicsBody(leftBody);
    leftBody->setDynamic(false);
    this->addChild(_leftNode);
    
    
    

    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    

    // player emoji
    emoji = Sprite::create(Universal::emojifileName.c_str());
    emoji->setTag(4);
    emoji->setScale(0.5);
    emojibody = PhysicsBody::createCircle(25);
    emoji->setPhysicsBody(emojibody);
    emoji->setPosition(Vec2(visibleSize.width/2, visibleSize.height/1.5));
    this->addChild(emoji,3);
    
    
    emojibody->setTag(DRAG_BODYS_TAG);
    emojibody->setGravityEnable(false);
    emojibody->setCollisionBitmask(2);
    emojibody->setContactTestBitmask(true);
    emojibody->setRotationEnable(false);
    
    
    
    // enemy
    auto enemy = Sprite::create("enemyEmoji.png");
    enemy->setPosition(Vec2(visibleSize.width - enemy->getContentSize().width/4, visibleSize.height - enemy->getContentSize().height/4));
    enemy->setTag(2);
    enemy->setScale(0.5);
    this->addChild(enemy,5);
    
    blockBodyDef.type = b2_dynamicBody;
    blockBodyDef.position.Set(winSize.width/1.06/PTM_RATIO, winSize.height/1.04/PTM_RATIO);
    blockBodyDef.userData = enemy;
    blockBody = _worldb2->CreateBody(&blockBodyDef);
    
    b2PolygonShape smallSquare;
    smallSquare.SetAsBox(enemy->getContentSize().width/3.85/PTM_RATIO/2,enemy->getContentSize().height/2.5/PTM_RATIO/2);
    
    b2FixtureDef blockShapeDef;
    blockShapeDef.shape = &smallSquare;
    blockShapeDef.density = 0.0f;
    blockShapeDef.friction = 0.0f;
    blockShapeDef.restitution = 1.0f;
    blockShapeDef.filter.groupIndex = -1;
    _blockFixture = blockBody->CreateFixture(&blockShapeDef);
    blockBody->SetFixedRotation(true);

    enemyBody = PhysicsBody::createBox(Size(60, 70));
    enemy->setPhysicsBody(enemyBody);
    
    enemyBody->setTag(DRAG_BODYS_TAG);
    enemyBody->setGravityEnable(false);
    enemyBody->setRotationEnable(false);
    
    enemyBody->setCategoryBitmask(0x01);
    enemyBody->setContactTestBitmask(0x01);
    enemyBody->setCollisionBitmask(0x01);
    // end enemy
    
    
    // enemy 2
    enemy2= Sprite::create("enemyEmoji.png");
    enemy2->setTag(2);
    enemy2->setScale(0.5);
    enemy2->setPosition(Vec2(origin.x+enemy2->getContentSize().width/4, visibleSize.height-enemy2->getContentSize().height/4));
    this->addChild(enemy2,5);
    
    block2BodyDef.type = b2_dynamicBody;
    block2BodyDef.position.Set(enemy2->getContentSize().width/4.5/PTM_RATIO, winSize.height/1.03649635/PTM_RATIO);
    block2BodyDef.userData = enemy2;
    block2Body = _worldb2->CreateBody(&block2BodyDef);
    
    b2FixtureDef block2ShapeDef;
    block2ShapeDef.shape = &smallSquare;
    block2ShapeDef.density = 0.0f;
    block2ShapeDef.friction = 0.0f;
    block2ShapeDef.restitution = 1.0f;
    block2ShapeDef.filter.groupIndex = -1;
    _block2Fixture = block2Body->CreateFixture(&block2ShapeDef);
    block2Body->SetFixedRotation(true);
    
    enemy2Body = PhysicsBody::createBox(Size(60, 70));
    enemy2->setPhysicsBody(enemy2Body);
    
    enemy2Body->setTag(DRAG_BODYS_TAG);
    enemy2Body->setGravityEnable(false);
    enemy2Body->setRotationEnable(false);
    
    enemy2Body->setCategoryBitmask(0x01);
    enemy2Body->setContactTestBitmask(0x01);
    enemy2Body->setCollisionBitmask(0x01);
    // end enemy 2
    

    // enemy 3
    enemy3= Sprite::create("enemyEmoji.png");
    enemy3->setTag(2);
    enemy3->setScale(0.5);
    enemy3->setPosition(Vec2(visibleSize.width-enemy3->getContentSize().width/4, visibleSize.height/2+enemy3->getContentSize().height/4));
    this->addChild(enemy3,5);
    
    block3BodyDef.type = b2_dynamicBody;
    block3BodyDef.position.Set(enemy3->getContentSize().width/4.5/PTM_RATIO, winSize.height/1.86/PTM_RATIO);
    block3BodyDef.userData = enemy3;
    block3Body = _worldb2->CreateBody(&block3BodyDef);
    
    b2FixtureDef block3ShapeDef;
    block3ShapeDef.shape = &smallSquare;
    block3ShapeDef.density = 0.0f;
    block3ShapeDef.friction = 0.0f;
    block3ShapeDef.restitution = 1.0f;
    block3ShapeDef.filter.groupIndex = -1;
    _block3Fixture = block3Body->CreateFixture(&block3ShapeDef);
    block3Body->SetFixedRotation(true);
    
    enemy3Body = PhysicsBody::createBox(Size(60, 70));
    enemy3->setPhysicsBody(enemy3Body);
    
    enemy3Body->setTag(DRAG_BODYS_TAG);
    enemy3Body->setGravityEnable(false);
    enemy3Body->setRotationEnable(false);
    
    enemy3Body->setCategoryBitmask(0x01);
    enemy3Body->setContactTestBitmask(0x01);
    enemy3Body->setCollisionBitmask(0x01);
    // end enemy 3
    
    
    // enemy 4
    enemy4= Sprite::create("enemyEmoji.png");
    enemy4->setTag(2);
    enemy4->setScale(0.5);
    enemy4->setPosition(Vec2(origin.x+enemy4->getContentSize().width/4, visibleSize.height/2+enemy4->getContentSize().height/4));
    this->addChild(enemy4,5);
    
    block4BodyDef.type = b2_dynamicBody;
    block4BodyDef.position.Set(winSize.width/1.06/PTM_RATIO, winSize.height/1.86/PTM_RATIO);
    block4BodyDef.userData = enemy4;
    block4Body = _worldb2->CreateBody(&block4BodyDef);
    
    b2FixtureDef block4ShapeDef;
    block4ShapeDef.shape = &smallSquare;
    block4ShapeDef.density = 0.0f;
    block4ShapeDef.friction = 0.0f;
    block4ShapeDef.restitution = 1.0f;
    block4ShapeDef.filter.groupIndex = -1;
    _block4Fixture = block4Body->CreateFixture(&block4ShapeDef);
    block4Body->SetFixedRotation(true);
    
    enemy4Body = PhysicsBody::createBox(Size(60, 70));
    enemy4->setPhysicsBody(enemy4Body);
    
    enemy4Body->setTag(DRAG_BODYS_TAG);
    enemy4Body->setGravityEnable(false);
    enemy4Body->setRotationEnable(false);
    
    enemy4Body->setCategoryBitmask(0x01);
    enemy4Body->setContactTestBitmask(0x01);
    enemy4Body->setCollisionBitmask(0x01);
    
    ///// end enemy 4
    
    touchcount = 0;

    _currentTouchID = -1;
    
    // fingerprint sprite
    fingerprint = Sprite::create("finger.png");
    fingerprint->setPosition( Vec2(winSize.width/2, winSize.height/4));
    this->addChild(fingerprint);
    
    // game over sprite
    gosprite = Sprite::create("gameoverBlue.png");
    gosprite->setPosition( Vec2(winSize.width/2, winSize.height+gosprite->getContentSize().height));
    this->addChild(gosprite);
    
    
    // background image
    auto background = Sprite::create("background.png");
    background->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    this->addChild(background, -1);
    
    // add score and set hidden
    char _score[256];
    sprintf(_score, "%d", Universal::SCORE);
    std::string strScore = "";
    strScore.append(_score);
    const char * charScore = strScore.c_str();
    scoreLabel = Label::createWithTTF(charScore, "Nexa Bold.otf", 200);
    scoreLabel->setColor(Color3B(255,255,255));
    scoreLabel->setOpacity(100);
    scoreLabel->setPosition( Vec2(winSize.width/2, winSize.height/4 ) );
    scoreLabel->setVisible(false);
    this->addChild(scoreLabel, 10);
    
    // set score showing to 0
    scoreshowing=0;
    
    
    
    // add diamond
    diamond = Sprite::create("diamond.png");
    diamond->setTag(6);
    int randW = random(origin.x+diamond->getContentSize().width/2, visibleSize.width-diamond->getContentSize().width/2);
    int randH = random(origin.y+visibleSize.height/2+diamond->getContentSize().height/2, visibleSize.height-diamond->getContentSize().height/2);
    diamond->setPosition(Vec2(randW,randH));
    diamond->setScale(0.5);
    this->addChild(diamond,0);
    
    diamondbody = PhysicsBody::createBox(Size(45,50));

    diamond->setPhysicsBody(diamondbody);
    
    diamondbody->setGravityEnable(false);
    diamondbody->setRotationEnable(false);
    diamondbody->setCollisionBitmask(0);
    diamondbody->setContactTestBitmask(true);

    
    
    // set diamond popped to false
    diamondPopped=false;
    
    this->scheduleUpdate();
    
    return true;
}


void GameScene::diamondPop(){
    
    if (diamondPopped==false) {
        
        diamondPopped=true;
        
        // add bubble count to score
        Universal::SCORE=Universal::SCORE+1;
        
        // add Total count
        Universal::TOTAL_COIN = Universal::TOTAL_COIN + 1;      // Add by Louis
        UserDefault* uerdefault = UserDefault::getInstance();
        uerdefault->setIntegerForKey(kTotalCoin, Universal::TOTAL_COIN);
        
        // point sound
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("point.mp3");
        
        // callback
        auto actionDone = CallFuncN::create(CC_CALLBACK_1(GameScene::actionDone,this));
        
        // run pop action
        diamond->runAction(Sequence::create(actionDone, NULL));
        

        
    }
    
}

// action done
void GameScene::actionDone(Ref* pSender){
    
    // get new random width
    int randW = random(origin.x+diamond->getContentSize().width/2, visibleSize.width-diamond->getContentSize().width/2);
    // get new random height
    int randH = random(origin.y+visibleSize.height/2+diamond->getContentSize().height/2, visibleSize.height-diamond->getContentSize().height/2);
    
    // set diamond in random position on screen
    diamond->setPosition(Vec2(randW,randH));
    
    diamondPopped=false;
    
    
}

// update
void GameScene::update(float dt)
{
    
    // update score label
    char buff[100];
    sprintf( buff, "%d", Universal::SCORE );
    scoreLabel->setString(buff);
    

    // check for new high score
    if(Universal::SCORE>Universal::HIGH_SCORE)
    {
        Universal::HIGH_SCORE = Universal::SCORE;
        UserDefault *userDefaults = UserDefault::getInstance();
        userDefaults->setIntegerForKey("HIGHSCORE", Universal::HIGH_SCORE);
        userDefaults->flush();
    }

    // box2d
    _worldb2->Step(dt, 10, 10);
    
    for(b2Body *b = _worldb2->GetBodyList(); b; b=b->GetNext()) {
        
        if (b->GetUserData() != NULL) {
            Sprite *sprite = (Sprite *)b->GetUserData();
            
            if (sprite->getTag() == 1 || sprite->getTag() == 3) {
                static int maxSpeed = 100;
                
                b2Vec2 velocity = b->GetLinearVelocity();
                float32 speed = velocity.Length();
                
                if (speed > maxSpeed) {
                    b->SetLinearDamping(0.5);
                } else if (speed < maxSpeed) {
                    b->SetLinearDamping(-0.05);
                }
                
            }
            
            sprite->setPosition(Vec2(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
        }
        
        
    }

    
}

// touch began
bool GameScene::onTouchBegan(Touch* touch, Event* event){
    
    if (touchcount==0) {
        
        fingerprint->setVisible(false);
        scoreLabel->setVisible(true);

        
        
        // set velocity of small block
        b2Vec2 force = b2Vec2(6, 5);
        blockBody->SetLinearVelocity(force);
        
        // set velocity of small block 2
        b2Vec2 force2 = b2Vec2(10, 10);
        block2Body->SetLinearVelocity(force2);
        
        // set linear velocity of medium block
        b2Vec2 force3 = b2Vec2(4, 5);
        block3Body->SetLinearVelocity(force3);
        
        // set linear velocity of long block
        b2Vec2 force4 = b2Vec2(10, 10);
        block4Body->SetLinearVelocity(force4);


        touchcount++;
    }
    
    
    if (gameover==false) {
        
    
    emojibody->setVelocityLimit(1000.0);

    auto location = touch->getLocation();
    auto shapeArr = _world->getShapes(location);
    
    if(touch->getLocation().y < visibleSize.height/2){
    
    if(emojibody != nullptr)
    {
        Node* mouse = Node::create();
        mouse->setPhysicsBody(PhysicsBody::create(PHYSICS_INFINITY, PHYSICS_INFINITY));
        mouse->getPhysicsBody()->setDynamic(false);
        mouse->setPosition(location*kEmojiSensitivity);
        this->addChild(mouse);
        
        PhysicsJointFixed* joint = PhysicsJointFixed::construct(mouse->getPhysicsBody(), emojibody, location);
        joint->setMaxForce(50000.0f * emojibody->getMass());
        _world->addJoint(joint);
        _mouses.insert(std::make_pair(touch->getID(), mouse));
        
        return true;
        
    }
    return false;
        
    }
    
}
    
return true;
    
}

// touch moved
void  GameScene::onTouchMoved(Touch* touch, Event* event){
    
    if (gameover==false) {

    auto it = _mouses.find(touch->getID());
    if (it != _mouses.end())
    {
        it->second->setPosition(touch->getLocation()*kEmojiSensitivity);
    }
}
    
}

// touch ended
void GameScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{

    if (gameover==false) {
        
    
    emojibody->setVelocity(Vec2(0,0));
    emojibody->setVelocityLimit(0.0);
    
    auto it = _mouses.find(touch->getID());
    if (it != _mouses.end())
    {
        emojibody->setVelocity(Vec2(0,0));
        this->removeChild(it->second);
        _mouses.erase(it);
    }
}

}

// game over scene
void GameScene::spriteMoveFinished(Ref* pSender){
    
    Director::getInstance()->pushScene(GameOverScene::createScene());
    
}


// set physics body
void GameScene::setPhysicsBody(PhysicsBody* body)
{
    _physicsBody = body;
    _physicsBody->retain();
    
}

bool GameScene::onContactBegin(const PhysicsContact& contact)
{
    auto spriteA = (Sprite*)contact.getShapeA()->getBody()->getNode();
    auto spriteB = (Sprite*)contact.getShapeB()->getBody()->getNode();
    
    
    // hit enemy check 1
    if (spriteA->getTag() == 4 && spriteB->getTag()==2)
    {
        
        blockBody->SetAwake(false);
        blockBody->SetLinearVelocity(b2Vec2(0,0));
        blockBody->SetAngularVelocity(0);
        
        block2Body->SetAwake(false);
        block2Body->SetLinearVelocity(b2Vec2(0,0));
        block2Body->SetAngularVelocity(0);
        
        block3Body->SetAwake(false);
        block3Body->SetLinearVelocity(b2Vec2(0,0));
        block3Body->SetAngularVelocity(0);
        
        block4Body->SetAwake(false);
        block4Body->SetLinearVelocity(b2Vec2(0,0));
        block4Body->SetAngularVelocity(0);

        
        gameover=true;
        emojibody->setEnable(false);
        emoji->setTexture("emoji_dead.png");
        
        
        // play game over sound once
        if(gameoversound==false){
            
            // game over sound
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("lose.mp3");
            gameoversound=true;
            
        }
        
        // pause event listners
        Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(this);
        
        
        
        
        
        if(Universal::REWARD_USED==false && Universal::SCORE >=15){
            
            if(calledreward==false){
                SendMessageWithParams(string("LoadRewards"), NULL);
                calledreward=true;
            }
            Director::getInstance()->pushScene(GameOverScene::createScene());

            
        }
        else{
            // gameover actions
            auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(GameScene::spriteMoveFinished,this));
            auto gameoveraction = MoveTo::create(1.0,Vec2(visibleSize.width/2, visibleSize.height/1.8));
            
            // gameover sprite run action
            gosprite->runAction(Sequence::create(gameoveraction, actionMoveDone,NULL));
            
            
        }
        
        
    }
    
    // hit enemy check 2
    if (spriteB->getTag() == 4 && spriteA->getTag()==2)
    {
        
        // stop block
        blockBody->SetAwake(false);
        blockBody->SetLinearVelocity(b2Vec2(0,0));
        blockBody->SetAngularVelocity(0);
        
        // stop block 2
        block2Body->SetAwake(false);
        block2Body->SetLinearVelocity(b2Vec2(0,0));
        block2Body->SetAngularVelocity(0);
        
        // stop medium body
        block3Body->SetAwake(false);
        block3Body->SetLinearVelocity(b2Vec2(0,0));
        block3Body->SetAngularVelocity(0);
        
        // stop long body
        block4Body->SetAwake(false);
        block4Body->SetLinearVelocity(b2Vec2(0,0));
        block4Body->SetAngularVelocity(0);

        gameover=true;
        emojibody->setEnable(false);
        emoji->setTexture("emoji_dead.png");


        // play game over sound once
        if(gameoversound==false){
            
            // game over sound
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("lose.mp3");
            gameoversound=true;
            
        }
        
        // pause event listners
        Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(this);
        
        
        if(Universal::REWARD_USED==false && Universal::SCORE >=15){
            
            if(calledreward==false){
                SendMessageWithParams(string("LoadRewards"), NULL);
                calledreward=true;
            }
            Director::getInstance()->pushScene(GameOverScene::createScene());
            
            
        }
        else{
            // gameover actions
            auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(GameScene::spriteMoveFinished,this));
            auto gameoveraction = MoveTo::create(1.0,Vec2(visibleSize.width/2, visibleSize.height/1.8));
            
            // gameover sprite run action
            gosprite->runAction(Sequence::create(gameoveraction, actionMoveDone,NULL));
            
            
        }
        
        
    }
    
    // diamond hit by emoji
    if (spriteA->getTag() == 4 && spriteB->getTag()==6)
    {
        
        diamondPop();
        
    }
    if (spriteB->getTag() == 6 && spriteA->getTag()==4)
    {
        
        diamondPop();
        
    }
    

    
    return true;
}

