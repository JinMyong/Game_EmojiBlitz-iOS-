#import <UIKit/UIKit.h>
#import "Chartboost/Chartboost.h"
#import "GoogleMobileAds/GoogleMobileAds.h"
#import <StoreKit/StoreKit.h>

#import <Pushwoosh/PushNotificationManager.h>

@class RootViewController;

@interface AppController : NSObject <UIApplicationDelegate,ChartboostDelegate,GADBannerViewDelegate,SKProductsRequestDelegate,SKPaymentTransactionObserver, PushNotificationDelegate> {
    
    UIWindow *window;
    GADBannerView *bannerView_;
    
}

@property(nonatomic, readonly) RootViewController* viewController;
@property (nonatomic,strong)GADBannerView *bannerView;
-(GADRequest *)createRequest;


- (void) completeTransaction: (SKPaymentTransaction *)transaction;
- (void) restoreTransaction: (SKPaymentTransaction *)transaction;
- (void) failedTransaction: (SKPaymentTransaction *)transaction;
- (bool) loadRewardVideo;       // Added by Louis
- (void) getCoinPurchase:(NSString*) purchaseKey;   // Added by Louis
- (void) showConfirmAlert;

@end