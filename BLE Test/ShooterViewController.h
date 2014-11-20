//
//  ShooterViewController.h
//  Adafruit Bluefruit LE Connect
//
//  Created by Parker Lusk on 11/18/14.
//  Copyright (c) 2014 Adafruit Industries. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ShooterViewControllerDelegate <NSObject>

- (void)sendData:(NSData*)newData;

@end


@interface ShooterViewController : UIViewController

@property (weak, nonatomic) id<ShooterViewControllerDelegate>       delegate;

@property (strong, nonatomic) IBOutlet UISlider *vSlider;
@property (strong, nonatomic) IBOutlet UISlider *hSlider;
@property (strong, nonatomic) IBOutlet UILabel *distanceLabel;
@property (strong, nonatomic) IBOutlet UIProgressView *distanceProgressBar;

- (id)initWithDelegate:(id<ShooterViewControllerDelegate>)aDelegate;
- (void)receiveData:(NSData*)newData;
- (void)didConnect;
- (void)resetUI;
- (IBAction)sliderChanged:(id)sender;
- (IBAction)fireButton:(id)sender;
- (IBAction)requestDistanceButton:(id)sender;
- (IBAction)homeButton:(id)sender;
@end
