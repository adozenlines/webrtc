/*
*  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/

#ifndef WEBRTC_MODULES_AUDIO_DEVICE_TEST_WINRT_WINRTTESTMANAGER_H_
#define WEBRTC_MODULES_AUDIO_DEVICE_TEST_WINRT_WINRTTESTMANAGER_H_

#include <list>
#include <string>

#include "webrtc/modules/audio_device/audio_device_utility.h"
#include "webrtc/common_audio/resampler/include/resampler.h"
#include "webrtc/modules/audio_device/include/audio_device.h"
#include "webrtc/modules/audio_device/test/audio_device_test_defines.h"
#include "webrtc/system_wrappers/interface/file_wrapper.h"
#include "webrtc/typedefs.h"


// Sets the default pause time if using sleep as pause
#define DEFAULT_PAUSE_TIME 10000
#if defined(USE_SLEEP_AS_PAUSE)
#define PAUSE(a) SleepMs(a);
#elif defined(WINRT)
#define PAUSE(a)  WinRTTestManager::waitForUserInput(a)
#else
#define PAUSE(a) AudioDeviceUtility::WaitForKey();
#endif

#define ADM_AUDIO_LAYER AudioDeviceModule::kWindowsWasapiAudio

struct AudioPacket {
  uint8_t dataBuffer[4 * 960];
  uint16_t nSamples;
  uint16_t nBytesPerSample;
  uint8_t nChannels;
  uint32_t samplesPerSec;
};

using namespace webrtc;

class AudioEventObserver;

// ----------------------------------------------------------------------------
//  AudioEventObserver
// ----------------------------------------------------------------------------

class AudioEventObserver : public AudioDeviceObserver {
 public:
  virtual void OnErrorIsReported(const ErrorCode error);
  virtual void OnWarningIsReported(const WarningCode warning);
  explicit AudioEventObserver(AudioDeviceModule* audioDevice);
  ~AudioEventObserver();
 public:
  ErrorCode _error;
  WarningCode _warning;
};

// ----------------------------------------------------------------------------
//  AudioTransport
// ----------------------------------------------------------------------------

class AudioTransportImpl : public AudioTransport {
 public:
  virtual int32_t
    RecordedDataIsAvailable(const void* audioSamples,
    const uint32_t nSamples,
    const uint8_t nBytesPerSample,
    const uint8_t nChannels,
    const uint32_t samplesPerSec,
    const uint32_t totalDelayMS,
    const int32_t clockDrift,
    const uint32_t currentMicLevel,
    const bool keyPressed,
    uint32_t& newMicLevel);

  virtual int32_t NeedMorePlayData(const uint32_t nSamples,
    const uint8_t nBytesPerSample,
    const uint8_t nChannels,
    const uint32_t samplesPerSec,
    void* audioSamples,
    uint32_t& nSamplesOut,
    int64_t* elapsed_time_ms,
    int64_t* ntp_time_ms);

  virtual int OnDataAvailable(const int voe_channels[],
    int number_of_voe_channels,
    const int16_t* audio_data,
    int sample_rate,
    int number_of_channels,
    int number_of_frames,
    int audio_delay_milliseconds,
    int current_volume,
    bool key_pressed,
    bool need_audio_processing);

  virtual void PushCaptureData(int voe_channel, const void* audio_data,
    int bits_per_sample, int sample_rate,
    int number_of_channels,
    int number_of_frames);

  virtual void PullRenderData(int bits_per_sample, int sample_rate,
    int number_of_channels, int number_of_frames,
    void* audio_data,
    int64_t* elapsed_time_ms,
    int64_t* ntp_time_ms);

  explicit AudioTransportImpl(AudioDeviceModule* audioDevice);
  ~AudioTransportImpl();

 public:
  int32_t SetFilePlayout(bool enable, const char* fileName = NULL);
  void SetFullDuplex(bool enable);
  void SetSpeakerVolume(bool enable) {
    _speakerVolume = enable;
  }

  void SetSpeakerMute(bool enable) {
    _speakerMute = enable;
  }

  void SetMicrophoneMute(bool enable) {
    _microphoneMute = enable;
  }

  void SetMicrophoneVolume(bool enable) {
    _microphoneVolume = enable;
  }

  void SetMicrophoneBoost(bool enable) {
    _microphoneBoost = enable;
  }

  void SetLoopbackMeasurements(bool enable) {
    _loopBackMeasurements = enable;
  }

  void SetMicrophoneAGC(bool enable) {
    _microphoneAGC = enable;
  }

 private:
  typedef std::list<AudioPacket*> AudioPacketList;
  AudioDeviceModule* _audioDevice;

  bool _playFromFile;
  bool _fullDuplex;
  bool _speakerVolume;
  bool _speakerMute;
  bool _microphoneVolume;
  bool _microphoneMute;
  bool _microphoneBoost;
  bool _microphoneAGC;
  bool _loopBackMeasurements;

  FileWrapper& _playFile;

  uint32_t _recCount;
  uint32_t _playCount;
  AudioPacketList _audioList;

  Resampler _resampler;
};


// ----------------------------------------------------------------------------
//  WinRTTestManager
// ----------------------------------------------------------------------------

class WinRTTestManager {
 public:
  WinRTTestManager();
  ~WinRTTestManager();
  int32_t Init();

 public:
  int32_t TestDeviceEnumeration();
  int32_t TestDeviceSelection();
  int32_t TestAudioTransport();
  int32_t TestSpeakerVolume();
  int32_t TestMicrophoneVolume();
  int32_t TestSpeakerMute();
  int32_t TestMicrophoneMute();
  int32_t TestMicrophoneBoost();
  int32_t TestLoopback();
  int32_t TestDeviceRemoval();
  int32_t TestExtra();
  int32_t TestMicrophoneAGC();
  int32_t SelectPlayoutDevice();
  int32_t SelectRecordingDevice();

  AudioDeviceModule* _audioDevice;
  rtc::scoped_ptr<ProcessThread> _processThread;
  AudioEventObserver* _audioEventObserver;
  AudioTransportImpl* _audioTransport;

#ifdef WINRT
  static void waitForUserInput(int ms);
  static void userSignalToContinue();
#endif

 private:
  // Paths to where the resource files to be used for this test are located.
  std::string _playoutFile48;
  std::string _playoutFile44;
  std::string _playoutFile16;
  std::string _playoutFile8;
#ifdef WINRT
  static HANDLE _semhandle;
#endif
};

#endif  // WEBRTC_MODULES_AUDIO_DEVICE_TEST_WINRT_WINRTTESTMANAGER_H_
