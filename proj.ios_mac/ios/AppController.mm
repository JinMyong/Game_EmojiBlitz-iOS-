#import "AppController.h"
#import "platform/ios/CCEAGLView-ios.h"
#import "cocos2d.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import "Chartboost/Chartboost.h"
#import <GameKit/GameKit.h>
#include "config.h"
#include "IOSNDKHelper.h"
#import <AudioToolbox/AudioServices.h>
#include "GameScene.h"
#include "SocialGame.h"
#include "SimpleAudioEngine.h"
// don't change this
#define k_Save @"Saveitem"


#import <MobileAppTracker/MobileAppTracker.h>

@implementation AppController

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    

    cocos2d::Application *app = cocos2d::Application::getInstance();
    app->initGLContextAttrs();
    cocos2d::GLViewImpl::convertAttrs();

    // Override point for customization after application launch.
    
    // Coming from C++
    [IOSNDKHelper SetNDKReciever:self];

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];

    // Init the CCEAGLView
    CCEAGLView *eaglView = [CCEAGLView viewWithFrame: [window bounds]
                                         pixelFormat: (NSString*)cocos2d::GLViewImpl::_pixelFormat
                                         depthFormat: cocos2d::GLViewImpl::_depthFormat
                                  preserveBackbuffer: NO
                                          sharegroup: nil
                                       multiSampling: NO
                                     numberOfSamples: 0 ];
    
    // Enable or disable multiple touches
    [eaglView setMultipleTouchEnabled:NO];

    // Use RootViewController manage CCEAGLView 
    _viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    _viewController.wantsFullScreenLayout = YES;
    _viewController.view = eaglView;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: _viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:_viewController];
    }

    [window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden:true];

    // IMPORTANT: Setting the GLView should be done after creating the RootViewController
    cocos2d::GLView *glview = cocos2d::GLViewImpl::createWithEAGLView(eaglView);
    cocos2d::Director::getInstance()->setOpenGLView(glview);

    app->run();
    
    NSUserDefaults *saveapp = [NSUserDefaults standardUserDefaults];
    
    bool saved = [saveapp boolForKey:k_Save];
    
    if (!saved){
        //IAP not purchased
        NSLog(@"not purchased");
        
        if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
            CGPoint origin = CGPointMake(0.0,window.frame.size.height - 50);
            _bannerView = [[[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerPortrait origin:origin ] autorelease];
        }
        else{
            
            CGPoint origin = CGPointMake(0.0,window.frame.size.height - 90);
            _bannerView = [[[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerPortrait origin:origin ] autorelease];
        }
        
        // do not change this - set admob id in the config.h file
        _bannerView.adUnitID = AdMobAdID;
        [_bannerView setRootViewController:_viewController];
        [[[UIApplication sharedApplication].keyWindow rootViewController].view addSubview:_bannerView];
        [_bannerView loadRequest:[self createRequest]];
        
    } else {
        
        
        //IAP purchased
        
    }

    //-----------PUSHWOOSH PART-----------
    // set custom delegate for push handling, in our case - view controller
    PushNotificationManager * pushManager = [PushNotificationManager pushManager];
    pushManager.delegate = self;
    
    // handling push on app start
    [[PushNotificationManager pushManager] handlePushReceived:launchOptions];
    
    // make sure we count app open in Pushwoosh stats
    [[PushNotificationManager pushManager] sendAppOpen];
    
    // register for push notifications!
    [[PushNotificationManager pushManager] registerForPushNotifications];
    
    return YES;
}

- (void) onPushAccepted:(PushNotificationManager *)pushManager withNotification:(NSDictionary *)pushNotification {
    NSLog(@"Push notification received");
}

// system push notification registration success callback, delegate to pushManager
- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [[PushNotificationManager pushManager] handlePushRegistration:deviceToken];
}

// system push notification registration error callback, delegate to pushManager
- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    [[PushNotificationManager pushManager] handlePushRegistrationFailure:error];
}

// system push notifications callback, delegate to pushManager
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    [[PushNotificationManager pushManager] handlePushReceived:userInfo];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
     //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->pause(); */
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    
    // do not change this - set your Chartboost IDs in the config.h file
    
    [Tune measureSession];
    
    [Chartboost startWithAppId:ChartboostAppID appSignature:ChartboostAppSignature delegate:self];
    [Chartboost cacheRewardedVideo:CBLocationHomeScreen]; // Added by Louis
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    
     //We don't need to call this method any more. It will interupt user defined game pause&resume logic
    /* cocos2d::Director::getInstance()->resume(); */
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    cocos2d::Application::getInstance()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    cocos2d::Application::getInstance()->applicationWillEnterForeground();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
}


- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    [Tune applicationDidOpenURL:[url absoluteString] sourceApplication:sourceApplication];
    
    return YES;
}


#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
}


- (void)dealloc {
    [window release];
    [super dealloc];
}


-(GADRequest *)createRequest {
    GADRequest *request = [GADRequest request];
    return request;
}

-(void)adViewDidReceiveAd:(GADBannerView *)adView {
    
    NSUserDefaults *saveapp = [NSUserDefaults standardUserDefaults];
    bool saved = [saveapp boolForKey:k_Save];
    
    if (!saved){
        
        NSLog(@"Ad Received");
        
        [UIView animateWithDuration:1.0 animations:^{
            
            CGPoint origin = CGPointMake(0.0,0.0);
            _bannerView = [[[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerPortrait origin:origin ] autorelease];
            
        }];
        
        
    } else {
        //IAP purchased
        
        NSLog(@"saved");
        
        if (_bannerView.hidden==false) {
            
            log("bannerview not hidden -> hidden");
            
            [_bannerView setHidden:true];
            
        }
        
    }
    
}

-(void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error {
    NSLog(@"Failed to receive ad due to: %@", [error localizedFailureReason]);
}

- (void) hideAdmob:(NSObject*)prms
{
    [_bannerView setHidden:true];
    
}

- (void) showAdmob:(NSObject*)prms
{
    [_bannerView setHidden:false];
}

- (void) openUrl:(NSObject*)prms
{
    
    // don't change this - set your store link in the config.h file
    
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:kStoreLink]];
    
}


- (void) LoadInterstitial:(NSObject *)prms
{
    
    
    NSUserDefaults *saveapp = [NSUserDefaults standardUserDefaults];
    bool saved = [saveapp boolForKey:k_Save];
    
    if (!saved){
        //IAP not purchased
        NSLog(@"not saved");
        [Chartboost showInterstitial:CBLocationHomeScreen];
        
        
    } else {
        
        //IAP purchased
        NSLog(@"saved");
    }
    
    
}

- (bool) loadRewardVideo
{
    bool retValue = false;
    if ([Chartboost hasRewardedVideo:CBLocationHomeScreen]) {
        [Chartboost showRewardedVideo:CBLocationHomeScreen];
        retValue = true;
    }else
    {
        retValue = false;
    }
    
    return retValue;
}

- (void) LoadMore:(NSObject *)prms
{
    
    [Chartboost showMoreApps:CBLocationDefault];
}

// don't change this - set your product identifier in the config.h file
- (void) openPurchase:(NSObject*)prms
{
    
    // iOS 8 Alert Controller
    if ([UIAlertController class])
    {
        
        
        // UIAlertController
        
        UIAlertController * view=   [UIAlertController
                                     alertControllerWithTitle:@"Get rid of these ads!"
                                     message:@""
                                     preferredStyle:UIAlertControllerStyleActionSheet];
        
        UIAlertAction* purchase = [UIAlertAction
                                   actionWithTitle:@"Remove Ads"
                                   style:UIAlertActionStyleDefault
                                   handler:^(UIAlertAction * action)
                                   {
                                       SKProductsRequest *request= [[SKProductsRequest alloc]
                                                                    initWithProductIdentifiers: [NSSet setWithObject: kRemoveAdsProductIdentifier]];
                                       request.delegate = self;
                                       [request start];
                                       
                                       
                                       
                                       [view dismissViewControllerAnimated:YES completion:nil];
                                       
                                   }];
        
        
        
        UIAlertAction* restore = [UIAlertAction
                                  actionWithTitle:@"Restore Purchase"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      [[SKPaymentQueue defaultQueue]restoreCompletedTransactions];
                                      
                                      
                                      
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      
                                  }];
        
        UIAlertAction* cancel = [UIAlertAction
                                 actionWithTitle:@"Cancel"
                                 style:UIAlertActionStyleDefault
                                 handler:^(UIAlertAction * action)
                                 {
                                     [view dismissViewControllerAnimated:YES completion:nil];
                                     
                                 }];
        
        
        [view addAction:purchase];
        [view addAction:restore];
        [view addAction:cancel];
        
        //if iPhone
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        {
            [_viewController presentViewController:view animated:YES completion:nil];
        }
        //if iPad
        else
        {
            UIPopoverController *popup = [[UIPopoverController alloc] initWithContentViewController:view];
            NSLog(@"%f",window.frame.size.width/2);
            [popup presentPopoverFromRect:CGRectMake(window.frame.size.width/2, window.frame.size.height, 0, 0)inView:_viewController.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
        }
        
        
    }
    // iOS 7
    else
    {
        
        // UIAlertView
        
        UIAlertView * alert = [[UIAlertView alloc]initWithTitle:@"Get rid of these ads!" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Remove Ads",@"Restore Purchase", nil];
        [alert setTag:0];
        [alert show];
    }
    
    
}

- (void)alertView:(UIAlertView *)alertView willDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if ([alertView tag] == 1)
    {
        if (buttonIndex == 0) {
            NSLog(@"Cancel Tapped.");
            SocialGame::getConfirmResult(false);
        }
        else if (buttonIndex == 1) {
            NSLog(@"OK Tapped. Hello World!");
            SocialGame::getConfirmResult(true);
        }
    }else
    {
        if(buttonIndex == 0)
        {
            NSLog(@"Cancel");
        }
        else if(buttonIndex == 1)
        {
            
            NSLog(@"Remove Ads");
            
            SKProductsRequest *request= [[SKProductsRequest alloc]
                                         initWithProductIdentifiers: [NSSet setWithObject: kRemoveAdsProductIdentifier]];
            request.delegate = self;
            [request start];
            
        }
        else if(buttonIndex == 2)
        {
            
            NSLog(@"Restore Ads");
            
            [[SKPaymentQueue defaultQueue]restoreCompletedTransactions];
            
        }
    }
    
}


- (void) openRestore:(NSObject*)prms
{
    
    [[SKPaymentQueue defaultQueue]restoreCompletedTransactions];
    
}



- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    
    NSArray *myProduct = response.products;
    NSLog(@"%@",[[myProduct objectAtIndex:0] productIdentifier]);
    
    SKPayment *newPayment = [SKPayment paymentWithProduct:[myProduct objectAtIndex:0]];
    [[SKPaymentQueue defaultQueue] addPayment:newPayment];
    
    
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions {
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
            default:
                break;
        }
    }
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
    log("purchased");
    
    if([transaction.payment.productIdentifier isEqualToString:kRemoveAdsProductIdentifier])
    {   if (_bannerView.hidden==false) {
        
        log("bannerview not hidden -> hidden");
        
        [_bannerView setHidden:true];
        
        }
    }
    
    if([transaction.payment.productIdentifier isEqualToString:kGetCoin100])
    {
        SocialGame::completeGetCoin(100);
    }
    if([transaction.payment.productIdentifier isEqualToString:kGetCoin250])
    {
        SocialGame::completeGetCoin(250);
    }
    if([transaction.payment.productIdentifier isEqualToString:kGetCoin500])
    {
        SocialGame::completeGetCoin(500);
    }
    if([transaction.payment.productIdentifier isEqualToString:kGetCoin5000])
    {
        SocialGame::completeGetCoin(5000);
    }
    
    NSUserDefaults *saveapp = [NSUserDefaults standardUserDefaults];
    [saveapp setBool:TRUE forKey:k_Save];
    [saveapp synchronize];
    
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
    log("purchase restored");
    
    if (_bannerView.hidden==false) {
        
        log("bannerview not hidden -> hidden");
        
        [_bannerView setHidden:true];
        
    }
    
    
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Purchase Restored"
                                                        message:@"Your purchase has been restored!"
                                                       delegate:self
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:nil];
        [alert show];
    }
    
    
    
    
    NSUserDefaults *saveapp = [NSUserDefaults standardUserDefaults];
    [saveapp setBool:TRUE forKey:k_Save];
    [saveapp synchronize];
    
    
    
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Purchase Unsuccessful"
                                                        message:@"Your purchase failed. Please try again."
                                                       delegate:self
                                              cancelButtonTitle:@"OK"
                                              otherButtonTitles:nil];
        [alert show];
    }
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
    
}


// sharing
- (void) openShare:(NSObject *)prms
{
    
    NSDictionary *parameters = (NSDictionary*)prms;
    NSString* scorestring = (NSString*)[parameters objectForKey:@"score"];
    int points=scorestring.intValue;
    
    
    
    
    if(points==1){
        
        NSString *text = [NSString stringWithFormat: @"I got %d diamonds in %@! Think you can beat me? %@",points,kAppName, kStoreLink];
        
        UIActivityViewController *controller =
        [[UIActivityViewController alloc]
         initWithActivityItems:@[text]
         applicationActivities:nil];
        
        
        controller.excludedActivityTypes = @[UIActivityTypePostToWeibo,
                                             UIActivityTypeMail,
                                             UIActivityTypePrint,
                                             UIActivityTypeCopyToPasteboard,
                                             UIActivityTypeAssignToContact,
                                             UIActivityTypeSaveToCameraRoll,
                                             UIActivityTypeAddToReadingList,
                                             UIActivityTypePostToFlickr,
                                             UIActivityTypePostToVimeo,
                                             UIActivityTypePostToTencentWeibo,
                                             UIActivityTypeAirDrop];
        
        
        //if iPhone
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        {
            [_viewController presentViewController:controller animated:YES completion:nil];
        }
        //if iPad
        else
        {
            // Change Rect to position Popover
            UIPopoverController *popup = [[UIPopoverController alloc] initWithContentViewController:controller];
            NSLog(@"%f",window.frame.size.width/2);
            [popup presentPopoverFromRect:CGRectMake(window.frame.size.width/2, window.frame.size.height, 0, 0)inView:_viewController.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
        }
        
        
        
    }
    else{
        
        NSString *text = [NSString stringWithFormat: @"I got %d diamonds in %@! Think you can beat me? %@",points,kAppName, kStoreLink];
        
        UIActivityViewController *controller =
        [[UIActivityViewController alloc]
         initWithActivityItems:@[text]
         applicationActivities:nil];
        
        controller.excludedActivityTypes = @[UIActivityTypePostToWeibo,
                                             UIActivityTypeMail,
                                             UIActivityTypePrint,
                                             UIActivityTypeCopyToPasteboard,
                                             UIActivityTypeAssignToContact,
                                             UIActivityTypeSaveToCameraRoll,
                                             UIActivityTypeAddToReadingList,
                                             UIActivityTypePostToFlickr,
                                             UIActivityTypePostToVimeo,
                                             UIActivityTypePostToTencentWeibo,
                                             UIActivityTypeAirDrop];
        
        
        //if iPhone
        if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone)
        {
            [_viewController presentViewController:controller animated:YES completion:nil];
        }
        //if iPad
        else
        {
            // Change Rect to position Popover
            UIPopoverController *popup = [[UIPopoverController alloc] initWithContentViewController:controller];
            NSLog(@"%f",window.frame.size.width/2);
            [popup presentPopoverFromRect:CGRectMake(window.frame.size.width/2, window.frame.size.height, 0, 0)inView:_viewController.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
        }
        
        
    }
    
    
    
}


- (void) vibrateDevice:(NSObject *)prms
{
    NSLog(@"VIBRATE");
    AudioServicesPlayAlertSound(kSystemSoundID_Vibrate);
    //AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}







- (void)didCompleteRewardedVideo:(CBLocation)location withReward:(int)reward {
    NSLog(@"completed rewarded video view at location %@ with reward amount %d", location, reward);
    
    SocialGame::completeRewardVideo();
    
//    Director::getInstance()->pushScene(GameScene::createScene());

}

/*
 * didFailToLoadRewardedVideo
 *
 * This is called when a Rewarded Video has failed to load. The error enum specifies
 * the reason of the failure
 */

- (void)didFailToLoadRewardedVideo:(NSString *)location withError:(CBLoadError)error {
    switch(error){
        case CBLoadErrorInternetUnavailable: {
            NSLog(@"Failed to load Rewarded Video, no Internet connection !");
        } break;
        case CBLoadErrorInternal: {
            NSLog(@"Failed to load Rewarded Video, internal error !");
        } break;
        case CBLoadErrorNetworkFailure: {
            NSLog(@"Failed to load Rewarded Video, network error !");
        } break;
        case CBLoadErrorWrongOrientation: {
            NSLog(@"Failed to load Rewarded Video, wrong orientation !");
        } break;
        case CBLoadErrorTooManyConnections: {
            NSLog(@"Failed to load Rewarded Video, too many connections !");
        } break;
        case CBLoadErrorFirstSessionInterstitialsDisabled: {
            NSLog(@"Failed to load Rewarded Video, first session !");
        } break;
        case CBLoadErrorNoAdFound : {
            NSLog(@"Failed to load Rewarded Video, no ad found !");
        } break;
        case CBLoadErrorSessionNotStarted : {
            NSLog(@"Failed to load Rewarded Video, session not started !");
        } break;
        case CBLoadErrorNoLocationFound : {
            NSLog(@"Failed to load Rewarded Video, missing location parameter !");
        } break;
        default: {
            NSLog(@"Failed to load Rewarded Video, unknown error !");
        }
    }
}


- (void)CacheRewards:(NSObject *)prms {
    [Chartboost cacheRewardedVideo:CBLocationHomeScreen];
}


- (void) LoadRewards:(NSObject *)prms
{
    
    [Chartboost showRewardedVideo:CBLocationDefault];
    
}




- (void)didDismissRewardedVideo:(CBLocation)location{
    
    NSLog(@"dismissed rewards at location %@", location);

    
}

-(void)didDisplayRewardedVided:(CBLocation)location{
    

}

-(void) getCoinPurchase:(NSString *)purchaseKey
{
    SKProductsRequest *request= [[SKProductsRequest alloc]
                                 initWithProductIdentifiers: [NSSet setWithObject: purchaseKey]];
    request.delegate = self;
    [request start];
}

- (void) showConfirmAlert
{
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@""
                                                    message:@"Do you want to purchase this character?"
                                                   delegate:self
                                          cancelButtonTitle:@"Cancel"
                                          otherButtonTitles:@"OK",nil];
    [alert show];
    [alert setTag:1];
    [alert release];
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
    
}


@end
