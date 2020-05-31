#include "FFTManager.h"

void FFTManager::Start() {
    fft = nullptr;
    audioTexture.allocate(1024,1,GL_RGB);
}

FFTManager::FFTManager() {
    audioTexture.allocate(1024,1,GL_RGB);
}

FFTManager::~FFTManager() {
    if (fft != nullptr) {
        delete fft;
    }
}

void FFTManager::StartMicInput() {
    fft = new ofxEasyFft();
    fft->setup(1024, OF_FFT_WINDOW_HAMMING);
    fft->setUseNormalization(true);
    currentState = InputStateMicInput;
}

void FFTManager::Update() {

    if (isPaused) {
        return;
    }

    if (currentState == InputStateMicInput) {
        fft->update();

        vector<float>& buffer = fft->getBins();
        int n = MIN(1024, buffer.size());

        unsigned char signal[1024*3];
        int audioIndex = 0;

        for (int i = 0; i < n; i+=3) {
            float v = MIN(255,buffer.at(audioIndex) * 255 + (float)lastBuffer[i] * 0.85 ); // FFT
            signal[i] = (unsigned char) v;
            signal[i+1] = (unsigned char)v;
            signal[i+2] = (unsigned char)v;
            audioIndex++;
        }
        audioTexture.loadData(signal, 1024, 1, GL_RGB);

        for (int i = 0; i < n; i+=3) {
            lastBuffer[i] = signal[i];
        }
    }

    else if (currentState == InputStateSoundFile) {

        float *buffer = ofSoundGetSpectrum(1024);
        unsigned char signal[1024*3];
        int audioIndex = 0;

        for (int i = 0; i < 1024; i+=3) {
            float v = MIN(255,buffer[audioIndex] * 255 + (float)lastBuffer[i] * 0.85 ); // FFT
            signal[i] = (unsigned char) v;
            signal[i+1] = (unsigned char)v;
            signal[i+2] = (unsigned char)v;
            audioIndex++;
        }
        audioTexture.loadData(signal, 1024, 1, GL_RGB);

        for (int i = 0; i < 1024; i+=3) {
            lastBuffer[i] = signal[i];
        }
    }
}

void FFTManager::UpdateShader(ofShader *shader, int textureIndex) {
    shader->setUniformTexture("_AudioTexture", audioTexture, textureIndex);
}

void FFTManager::loadSoundFile(string filePath) {
    soundFilePath = filePath;
    audioTexture.allocate(1024,1,GL_RGB);
    soundPlayer.load(filePath);
    soundPlayer.play();

    soundPlayer.setPosition(0.9999);
    soundFileDuration = soundPlayer.getPositionMS() / 1000;
    cout << "sound file duration: " << soundFileDuration << endl;
    soundPlayer.setPosition(0.0);

    soundPlayer.play();
    soundPlayer.setLoop(true);
    currentState = InputStateSoundFile;
    isPaused = false;
}

void FFTManager::setPaused(bool val) {
    soundPlayer.setPaused(val);
    isPaused = val;
}

void FFTManager::setTime(float time) {
    //soundPlayer.setPaused(true);

    if (currentState != InputStateNone) {
        float t = fmod(time,soundFileDuration)*1000.;
        soundPlayer.setPositionMS(t);
    }
}

void FFTManager::resetSongIfPlaying() {
    setTime(0.0);
}
