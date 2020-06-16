#pragma once
#include <vector>

#include "ofxGuiExtended2.h"
#include "ofxMidi.h"
#include "ofxJSON.h"
#include "MidiMapper.h"
#include "ShaderPass.h"
#include "PNGRenderer.h"
#include "FFTManager.h"
#include "PassesGui.h"
#include "TextureInputSelectionView.h"

class ShaderChain: public ofxMidiListener {
public:

    string defaultPresetPath = "presets/default.json";
    vector<ShaderPass*> passes;
    float time;
    ofParameter<bool> isRunning;
    ofxMidiIn midiIn;
    ofxJSONElement result;
    bool isMouseDown;
    ofFbo cumulativeBuffer;
    ofFbo cumulativeBufferSwap;
    ofShader cumulativeShader;

    ShaderChain(glm::vec2 res);
    ShaderChain() {}
    ~ShaderChain();
    void Setup(glm::vec2 res);
    void SetupGui();
    void BeginSaveFrames();
    void update();
    void draw();
    void AddPass(ShaderPass *pass);

    void KeyPressed(int key);
    void newMidiMessage(ofxMidiMessage& eventArgs);
    void WriteToJson();
    void ReadFromJson(std::string path);
    void SetupMidi();
    void dragEvent(ofDragInfo info);

private:
    PNGRenderer *pngRenderer;
    ofxGui gui;
    ofxGuiContainer *guiGlobal;
    PassesGui *passesGui;
    ofxGuiContainer *parameterPanel;
    FFTManager fft;
    TextureInputSelectionView textureInputSelectionView;
    ofVideoGrabber vidGrabber;

    float mouseMoveSpeed = 10.0;

    bool showGui;
    bool isShowingFileDialogue;

    int frame;
    RenderStruct renderStruct;

    void openDefaultPreset();
    void RenderPasses();
    void removed(RemovedElementData& data);
    void moved(MovingElementData& data);
    void UpdateResolutionIfChanged(bool force);
    void OpenFilePressed();
    void savePresetAsPressed();
    void LoadPassFromFile(string path);
    void playingChanged(bool &val);
    void updateStatusText(string s);
    void loadMp3(string filePath);
    void processFileInput(string filePath);
    void saveVideo(string outputFilename);
    void encodeMp4Pressed();
    void encodeGifPressed();
    ofPlanePrimitive cumulativeRenderPlane;
    void startWebcam();
    void toggleWebcam(bool &val);
    void stopWebcam();
    void freeUnusedResources();
    void pauseResourcesForCurrentPlaybackState();
    string createUniqueFilePath(string path);
    bool mouseScrolled(ofMouseEventArgs & args);

    void midiButtonPressed();
    MidiMapper midiMapper;

    void newPresetButtonPressed();
    void updateShaderJsonPressed();
};
