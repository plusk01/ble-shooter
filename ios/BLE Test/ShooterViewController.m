//
//  ShooterViewController.m
//  Adafruit Bluefruit LE Connect
//
//  Created by Parker Lusk on 11/18/14.
//  Copyright (c) 2014 Adafruit Industries. All rights reserved.
//

#import "ShooterViewController.h"

// Protocol
#define PROTOCOL_SET        @"s"
#define PROTOCOL_GET        @"g"
#define PROTOCOL_FIRE       @"f"
#define PROTOCOL_ULTRASONIC @"u"
#define PROTOCOL_PITCH      @"v" // pitch is up and down
#define PROTOCOL_YAW        @"h" // yaw is left and right
#define PROTOCOL_GO_HOME    @"c" // c = center

#define MAX_DISTANCE        1000
#define HOME_VSLIDER        0.05f
#define HOME_HSLIDER        0.45f

@interface ShooterViewController ()

@end

@implementation ShooterViewController

#pragma mark View Lifecycle

- (id)initWithDelegate:(id<ShooterViewControllerDelegate>)aDelegate {
    
    NSString *nibName = @"ShooterViewController";
    
    self = [super initWithNibName:nibName bundle:[NSBundle mainBundle]];
    
    if (self) {
        self.delegate = aDelegate;
        self.title = @"Shooter";
        
        // Rotate slider to be vertical
        UIView *superView = self.vSlider.superview;
        [self.vSlider removeFromSuperview];
        [self.vSlider removeConstraints:self.view.constraints];
        self.vSlider.translatesAutoresizingMaskIntoConstraints = YES;
        self.vSlider.transform = CGAffineTransformMakeRotation(-M_PI_2);
        [superView addSubview:self.vSlider];
        
        [self resetUI];
        
        // Set up the timer to read ultrasonic data
        [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(sendUltrasonicRequest) userInfo:nil repeats:YES];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)resetUI {
    self.vSlider.value = HOME_VSLIDER;
    self.hSlider.value = HOME_HSLIDER;
}

#pragma mark IBActions

- (IBAction)sliderChanged:(id)sender {
    // TODO: Do some Tx debouncing for same slider value?
    
    NSMutableString *command = [NSMutableString stringWithString:PROTOCOL_SET];
    
    if ([sender isEqual:self.vSlider]) {
        [command appendString:PROTOCOL_YAW];
    } else if ([sender isEqual:self.hSlider]) {
        [command appendString:PROTOCOL_PITCH];
    }
    
    NSString *valueString = [NSString stringWithFormat:@"%f", ((UISlider *)sender).value];
    
    [command appendString:valueString];
    [self sendWithString:command];
}

- (IBAction)fireButton:(id)sender {
    //Send inputField's string via UART
    NSString *newString = PROTOCOL_FIRE;
    [self sendWithString:newString];
}

- (IBAction)requestDistanceButton:(id)sender {
    [self sendUltrasonicRequest];
}

- (IBAction)homeButton:(id)sender {
    NSMutableString *command = [NSMutableString stringWithString:PROTOCOL_SET];
    [command appendString:PROTOCOL_GO_HOME];
    [self sendWithString:command];
    
    // send sliders home
    [self resetUI];
}

#pragma mark Data Handling

- (void)receiveData:(NSData*)newData{
    //Receive data from device
    [self parseProtocol:newData];
}

- (void)sendWithString:(NSString *)str {
    NSData *data = [NSData dataWithBytes:str.UTF8String length:str.length];
    [_delegate sendData:data];
}

- (void)didConnect {
    [self resetUI];
    NSLog(@"Connected to Shooter");
}

#pragma mark Helper Protocol Methods

- (void)sendUltrasonicRequest {
    NSMutableString *command = [NSMutableString stringWithString:PROTOCOL_GET];
    [command appendString:PROTOCOL_ULTRASONIC];
    [self sendWithString:command];
}

- (void)parseProtocol:(NSData *)newData {
    int dataLength = (int)newData.length;
    uint8_t data[dataLength];
    
    [newData getBytes:&data length:dataLength];
    
    NSString *newString = [[NSString alloc]initWithBytes:&data
                                                  length:dataLength
                                                encoding:NSUTF8StringEncoding];

    
    if ([[newString substringToIndex:1] isEqualToString:PROTOCOL_GET]) {
        if ([[newString substringWithRange:NSMakeRange(1, 1)] isEqualToString:PROTOCOL_ULTRASONIC]) {
            NSString *distanceString = [newString substringFromIndex:2];
            
            self.distanceLabel.text = [NSString stringWithFormat:@"Distance: %@ cm", distanceString];
            
            float distanceFloat = [distanceString floatValue];
            self.distanceProgressBar.progress = distanceFloat / MAX_DISTANCE;
            
        }
        
    }
}

@end
