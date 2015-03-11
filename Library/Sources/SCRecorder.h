//
//  SCNewCamera.h
//  SCAudioVideoRecorder
//
//  Created by Simon CORSIN on 27/03/14.
//  Copyright (c) 2014 rFlex. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "SCRecordSession.h"
#import "SCSampleBufferHolder.h"
#import "SCVideoPlayerView.h"
#import "SCPlayer.h"
#import "SCAssetExportSession.h"
#import "SCImageView.h"
#import "SCSwipeableFilterView.h"
#import "SCRecorderToolsView.h"
#import "SCVideoConfiguration.h"
#import "SCAudioConfiguration.h"
#import "SCPhotoConfiguration.h"
#import "SCRecorderTools.h"

typedef NS_ENUM(NSInteger, SCFlashMode) {
    SCFlashModeOff  = AVCaptureFlashModeOff,
    SCFlashModeOn   = AVCaptureFlashModeOn,
    SCFlashModeAuto = AVCaptureFlashModeAuto,
    SCFlashModeLight
};

@class SCRecorder;

@protocol SCRecorderDelegate <NSObject>

@optional

// Camera stuffs
- (void)recorder:(SCRecorder *)recorder didReconfigureVideoInput:(NSError *)videoInputError;
- (void)recorder:(SCRecorder *)recorder didReconfigureAudioInput:(NSError *)audioInputError;
- (void)recorder:(SCRecorder *)recorder didChangeFlashMode:(SCFlashMode)flashMode error:(NSError *)error;
- (void)recorderWillStartFocus:(SCRecorder *)recorder;
- (void)recorderDidStartFocus:(SCRecorder *)recorder;
- (void)recorderDidEndFocus:(SCRecorder *)recorder;

// session stuffs
- (void)recorder:(SCRecorder *)recorder didInitializeAudioInSession:(SCRecordSession *)session error:(NSError *)error;
- (void)recorder:(SCRecorder *)recorder didInitializeVideoInSession:(SCRecordSession *)session error:(NSError *)error;
- (void)recorder:(SCRecorder *)recorder didBeginSegmentInSession:(SCRecordSession *)session error:(NSError *)error;
- (void)recorder:(SCRecorder *)recorder didCompleteSegment:(SCRecordSessionSegment *)segment inSession:(SCRecordSession *)session error:(NSError *)error;

- (void)recorder:(SCRecorder *)recorder didAppendVideoSampleBufferInSession:(SCRecordSession *)session;
- (void)recorder:(SCRecorder *)recorder didAppendAudioSampleBufferInSession:(SCRecordSession *)session;
- (void)recorder:(SCRecorder *)recorder didSkipAudioSampleBufferInSession:(SCRecordSession *)session;
- (void)recorder:(SCRecorder *)recorder didSkipVideoSampleBufferInSession:(SCRecordSession *)session;
- (void)recorder:(SCRecorder *)recorder didCompleteSession:(SCRecordSession *)session;

@end


@interface SCRecorder : NSObject<AVCaptureAudioDataOutputSampleBufferDelegate, AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureFileOutputRecordingDelegate>

/**
 Access the configuration for the video.
 */
@property (readonly, nonatomic) SCVideoConfiguration *videoConfiguration;

/**
 Access the configuration for the audio.
 */
@property (readonly, nonatomic) SCAudioConfiguration *audioConfiguration;

/**
 Access the configuration for the photo.
 */
@property (readonly, nonatomic) SCPhotoConfiguration *photoConfiguration;

/**
 Return whether the video is enabled and ready to use.
 */
@property (readonly, nonatomic) BOOL videoEnabledAndReady;

/**
 Return whether the audio is enabled and ready to use.
 */
@property (readonly, nonatomic) BOOL audioEnabledAndReady;

/**
 Will be true if the SCRecorder is currently recording
 */
@property (readonly, nonatomic) BOOL isRecording;

/**
 Change the flash mode on the camera
 */
@property (assign, nonatomic) SCFlashMode flashMode;

/**
 Determine whether the device has flash
 */
@property (nonatomic, readonly) BOOL deviceHasFlash;

/**
 Change the current used device
 */
@property (assign, nonatomic) AVCaptureDevicePosition device;

/**
 The zoom factor applied to the video.
 */
@property (assign, nonatomic) CGFloat videoZoomFactor;

/**
 Whether the zoom should be reset whenever the device changes.
 */
@property (assign, nonatomic) BOOL resetZoomOnChangeDevice;

/**
 Get the current focus mode used by the camera device
 */
@property (readonly, nonatomic) AVCaptureFocusMode focusMode;

/**
 Will be true if the camera is adjusting the focus.
 This property is KVO observable.
 */
@property (readonly, nonatomic) BOOL isAdjustingFocus;

/**
 The session preset used for the AVCaptureSession
 */
@property (copy, nonatomic) NSString *captureSessionPreset;

/**
 The captureSession. This will be null until prepare or startRunning has
 been called. Calling unprepare will set this property to null again.
 */
@property (readonly, nonatomic) AVCaptureSession *captureSession;

/**
 Whether the recorder has been prepared.
 */
@property (readonly, nonatomic) BOOL isPrepared;

/**
 The preview layer used for the video preview
 */
@property (readonly, nonatomic) AVCaptureVideoPreviewLayer *previewLayer;

/**
 Convenient way to set a view inside the preview layer
 */
@property (strong, nonatomic) UIView *previewView;

/**
 If set, this render will receive every received frames as CIImage.
 Can be useful for displaying a real time filter for example.
 */
@property (strong, nonatomic) id<CIImageRenderer> CIImageRenderer;

/**
 Set the delegate used to receive messages for the SCRecorder
 */
@property (weak, nonatomic) id<SCRecorderDelegate> delegate;

/**
 The record session to which the recorder will flow the camera/microphone buffers
 */
@property (strong, nonatomic) SCRecordSession *session;

/**
 The video orientation. This is automatically set if autoSetVideoOrientation is enabled
 */
@property (assign, nonatomic) AVCaptureVideoOrientation videoOrientation;

/**
 If true, the videoOrientation property will be set automatically
 depending on the current device orientation
 Default is NO
 */
@property (assign, nonatomic) BOOL autoSetVideoOrientation;

/**
 The frameRate for the video
 */
@property (assign, nonatomic) CMTimeScale frameRate;

/**
 The maximum record duration. When the record session record duration
 reaches this bound, the recorder will automatically pause the recording,
 end the current record segment and send recorder:didCompletesession: on the 
 delegate.
 */
@property (assign, nonatomic) CMTime maxRecordDuration;

/**
 Whether the fast recording method should be enabled.
 Enabling this will disallow pretty much every features provided
 by SCVideoConfiguration and SCAudioConfiguration. It will internally 
 uses a AVCaptureMovieFileOutput that provides no settings. If you have
 some performance issue, you can try enabling this.
 Default is NO.
 */
@property (assign, nonatomic) BOOL fastRecordMethodEnabled;

/**
 If maxRecordDuration is not kCMTimeInvalid,
 this will contains a float between 0 and 1 representing the
 recorded ratio on the current record session, 1 being fully recorded.
 */
@property (readonly, nonatomic) CGFloat ratioRecorded;

/**
 If enabled, the recorder will initialize the session and create the record segments
 when asking to record. Otherwise it will do it as soon as possible.
 Default is YES
 */
@property (assign, nonatomic) BOOL initializeSessionLazily;

/**
 If enabled, mirrored video buffers like when using a front camera
 will be written also as mirrored.
 */
@property (assign, nonatomic) BOOL keepMirroringOnWrite;

/**
 Whether the focus is supported on the current camera device
 */
@property (readonly, nonatomic) BOOL focusSupported;

/**
 The current focus point of interest
 */
@property (readonly, nonatomic) CGPoint focusPointOfInterest;

@property (readonly, nonatomic) NSError *videoError;

@property (readonly, nonatomic) NSError *audioError;

@property (readonly, nonatomic) NSError *photoError;

@property (readonly, nonatomic) NSError *sessionError;

/**
 The underlying AVCaptureVideoDataOutput
 */
@property (readonly, nonatomic) AVCaptureVideoDataOutput *videoOutput;

/**
 The underlying AVCaptureAudioDataOutput
 */
@property (readonly, nonatomic) AVCaptureAudioDataOutput *audioOutput;

/**
 The underlying AVCaptureStillImageOutput
 */
@property (readonly, nonatomic) AVCaptureStillImageOutput *photoOutput;

/**
 The dispatch queue that the SCRecorder uses for sending messages to the attached
 SCSession.
 */
@property (readonly, nonatomic) dispatch_queue_t sessionQueue;

/**
 Create a recorder
 @return the newly created recorder
 */
+ (SCRecorder*)recorder;

/**
 Create the AVCaptureSession
 Calling this method will set the captureSesion and configure it properly.
 If an error occured during the creation of the captureSession, this methods will return NO.
 The errors can be found on the videoError, audioError, photoError and sessionError properties
 */
- (BOOL)prepare;

/**
 Close and destroy the AVCaptureSession.
 */
- (void)unprepare;

/**
 Start the flow of inputs in the AVCaptureSession.
 prepare will be called if it wasn't prepared before.
 Calling this method will block until it's done.
 */
- (void)startRunning;

/**
 End the flow of inputs in the AVCaptureSession
 This wont destroy the AVCaptureSession.
 */
- (void)stopRunning;

/**
 Offer a way to configure multiple things at once.
 You can call beginSessionConfiguration multiple times.
 Only the latest most outer commitSessionConfiguration will effectively commit
 the configuration
 */
- (void)beginConfiguration;

/**
 Commit the session configuration after beginSessionConfiguration has been called
 */
- (void)commitConfiguration;

/**
 Switch between the camera devices
 */
- (void)switchCaptureDevices;

/**
 Convert the view coordinates to a point usable by the focus methods
 @return a point of interest usable in the focus methods
 */
- (CGPoint)convertToPointOfInterestFromViewCoordinates:(CGPoint)viewCoordinates;

/**
 Focus automatically at the given point of interest.
 Once the focus is completed, the camera device will goes to locked mode
 and won't try to do any further focus
 @param point A point of interest between 0,0 and 1,1
 */
- (void)autoFocusAtPoint:(CGPoint)point;

/**
 Continously focus at a point. The camera device detects when it needs to focus
 and focus automatically when needed.
 @param point A point of interest between 0,0 and 1,1,
 */
- (void)continuousFocusAtPoint:(CGPoint)point;

/**
 Focus at the center then switch back to a continuous focus at the center.
 */
- (void)focusCenter;

/**
 Refocus at the current position 
 */
- (void)refocus;

/**
 Lock the current focus and prevent any new further focus
 */
- (void)lockFocus;

/**
 Set an active device format that supports the requested framerate and the current video size
 This changes the frameRate.
 @return whether the method has succeeded or not
 */
- (BOOL)setActiveFormatWithFrameRate:(CMTimeScale)frameRate error:(NSError **)error;

/**
 Set an active device format that supports the request framerate and size
 This changes the frameRate.
 @return whether the method has succeeded or not
 */
- (BOOL)setActiveFormatWithFrameRate:(CMTimeScale)frameRate width:(int)width andHeight:(int)height error:(NSError**)error;

/**
 Allow the recorder to append the sample buffers inside the current setted session
 */
- (void)record;

/**
 Disallow the recorder to append the sample buffers inside the current setted session.
 If a record segment has started, this will be either canceled or completed depending on
 if it is empty or not.
 */
- (void)pause;

/**
 Disallow the recorder to append the sample buffers inside the current setted session.
 If a record segment has started, this will be either canceled or completed depending on
 if it is empty or not.
 @param completionHandler called on the main queue when the recorder is ready to record again.
 */
- (void)pause:(void(^)())completionHandler;

/**
 Capture a photo from the camera
 @param completionHandler called on the main queue with the image taken or an error in case of a problem
 */
- (void)capturePhoto:(void(^)(NSError *error, UIImage *image))completionHandler;

/**
 Signal to the recorder that the previewView frame has changed.
 This will make the previewLayer to matches the size of the previewView.
 */
- (void)previewViewFrameChanged;

/**
 Get an image representing the last output video buffer.
 */
- (UIImage *)snapshotOfLastVideoBuffer;

/**
 Get an image representing the last appended video buffer
 */
- (UIImage *)snapshotOfLastAppendedVideoBuffer;

/**
 Returns a shared recorder if you want to use the same instance throughout your app.
 */
+ (SCRecorder *)sharedRecorder;

/**
 Returns whether the current queue is the record session queue.
 */
+ (BOOL)isSessionQueue;

@end
