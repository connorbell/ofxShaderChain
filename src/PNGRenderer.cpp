#include "PNGRenderer.h"
#include <math.h>
#include <algorithm>

#include "ShaderChain.h"

PNGRenderer::PNGRenderer(float animduration, int fps, glm::vec2 resolution) {
    this->animduration = animduration;
    this->FPS = fps;
    this->presetFilePath = "";
    this->presetDisplayName = "";
    this->currentFrame = 0;
    this->totalFrames = animduration * fps;
    this->resolutionX = resolution.x;
    this->resolutionY = resolution.y;
    this->gifResolutionScale = 1.0;
    this->displayScaleParam = 1.0;
    this->renderedFrames = 1;
    this->frameskip = 1;
    this->FPS = 30;
    this->numLoops = 1;
    this->numBlendFrames = 1;
    this->preview = false;
}

void PNGRenderer::AddToGui(ofxGuiContainer *panel, ofxGuiContainer *statusLabelPanel, FFTManager *fft) {

    statusLabelPanel->add(statusLabel.set("Playing"));
    panel->add(spaceBufferLabel.set(" "));
    panel->addFpsPlotter();

    fileGroup.setName("File");

    ofxGuiMenu* fileMenu = panel->addMenu(fileGroup);

    fft->addToGui(panel);

    
    fileMenu->add(presetDisplayNameLabel.set("Preset: default"));
    fileMenu->add(newPresetButton.set("New Preset"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    fileMenu->add(openFileButton.set("Open File"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    fileMenu->add(saveAsPresetButton.set("Save Preset As…"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    fileMenu->add(savePresetButton.set("Save Preset"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    fileMenu->add(updateShaderJsonButton.set("Update Shader JSON"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    fileMenu->add(mapMidiButton.set("Midi Mapper"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    vidMenuGroup.setName("Mp4");
    vidMenuGroup.add(numLoops.set("Num loops", numLoops, 1, 32));

    renderParameterGroup.setName("Render");

    gifNumColors = 256;
    gifMenuGroup.setName("Gif");
    gifMenuGroup.add(gifNumColors.set("Colors", gifNumColors, 1, 255));

    ofxGuiMenu* renderingMenu = panel->addMenu(renderParameterGroup);
    renderingMenu->add<ofxGuiFloatInputField>(resolutionX.set("Res x", resolutionX, 1, 4096));
    renderingMenu->add<ofxGuiFloatInputField>(resolutionY.set("Res y", resolutionY, 1, 4096));
    renderingMenu->add<ofxGuiFloatInputField>(animduration.set("Duration          ", animduration, 0, 10000000));
    renderingMenu->add<ofxGuiIntInputField>(FPS.set("FPS", FPS, 1, 1000));
    renderingMenu->add(numBlendFrames.set("Blend Frames", numBlendFrames, 1, 128));
    renderingMenu->add(frameskip.set("Frameskip", frameskip, 1, 10));
    renderingMenu->add(preview.set("Preview", preview));
    saveFramesButton = renderingMenu->add(saveButton.set("Save Frames"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));

    panel->add(displayScaleParam.set("Display scale", displayScaleParam, 0.1, 5.0));

    ofxGuiMenu* gifGroup = renderingMenu->addMenu(gifMenuGroup);
    ofxGuiMenu* vidGroup = renderingMenu->addMenu(vidMenuGroup);
    gifGroup->add<ofxGuiFloatInputField>(gifResolutionScale.set("Scale", gifResolutionScale, 0.0, 1.0));
    gifGroup->add(encodeGifButton.set("Encode gif"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
    vidGroup->add(encodeMp4Button.set("Encode mp4"), ofJson({{"type", "fullsize"}, {"text-align", "center"}}));
}

float PNGRenderer::Tick() {
    float progress = (float)this->currentFrame / this->totalFrames;

    this->currentFrame = (this->currentFrame+1) % this->totalFrames;
    this->renderedFrames++;
    this->statusLabel = ("Rendered " + to_string(renderedFrames) + "/" + to_string(totalFrames) + " frames");

    if (this->renderedFrames == this->totalFrames) {
        this->isCapturing = false;
        saveFramesButton->setName("Save Frames");
        saveFramesButton->updateLayout();
    }

    return progress * this->animduration;
}

void PNGRenderer::WritePNG(ofFbo *buffer) {
  ofPixels outputPixels;
  outputPixels.allocate(this->resolutionX, this->resolutionY, OF_IMAGE_COLOR);
  int totalZeros = (int)floor(log10 (((float)this->totalFrames)));
  int totalZerosNeeded = totalZeros - (int)floor(log10 (((float)this->currentFrame)));
  if (this->currentFrame == 0) totalZerosNeeded = totalZeros;

  std::string s = "";
  for (int i = 0; i < totalZerosNeeded; i++) s += "0";

  s += std::to_string(this->currentFrame);
  buffer->readToPixels(outputPixels);
  string destFilePath = this->renderDirectory + this->presetDisplayName.get() + "_" + s + ".png";
  cout << destFilePath << endl;

  ofSaveImage(outputPixels, destFilePath, OF_IMAGE_QUALITY_BEST);
}

void PNGRenderer::Start() {
    if (this->isCapturing) {
        saveFramesButton->setName("Save Frames");
        this->isCapturing = false;
        return;
    }
    this->currentFrame = 0;
    this->renderedFrames = 0;
    this->totalFrames = animduration * FPS;
    this->isCapturing = true;

    saveFramesButton->setName("Cancel");
}

void PNGRenderer::updatePath(string s) {
    if (s == "") return;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

	replace(s.begin(), s.end(), '/', '\\');
	static const std::string slash = "\\";
#else
	static const std::string slash = "/";
#endif

    string file = s.substr(s.find_last_of(slash) + 1);

    // add json extension if needed
    int indexOfPeriod = file.find_last_of(".");
    if (indexOfPeriod == std::string::npos) {
        file += ".json";
        s += ".json";
        indexOfPeriod = file.find_last_of(".");
    }

    string fileWithoutExtension = file.substr(0, indexOfPeriod);

    presetDisplayName.set(fileWithoutExtension);
    presetDisplayNameLabel.set("Preset: " + presetDisplayName.get());
    presetFilePath = s;
}

void PNGRenderer::UpdateResolution(int w, int h) {
    this->resolutionX = w;
    this->resolutionY = h;
}
