#include "OptionsScene.h"
#include "../Kernel/Gateway.h"

OptionsScene::OptionsScene(const char* name) : Scene(name)
{
}

bool OptionsScene::initialize()
{
  camDist = Gateway::getConfiguration().camRadius;
  return Scene::initialize();
}

void OptionsScene::beginScene()
{
  Scene::beginScene();
  glClearColor(0,0,0,1);
  //mouseVisible = Gateway::getConfiguration().enableCursor;
}

void OptionsScene::actionPerformed(GUIEvent &evt)
{
  const String &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();
  
  //
  ///GUIButton
  //
  if (widgetType == BUTTON)
  {
    GUIButton *button = (GUIButton*) sourceRectangle;
    
    if (button->isClicked())
      sceneController.execute(callbackString);
  }
  
  //
  ///GUISlider
  //
  if (widgetType == SLIDER)
  {
    GUISlider *slider = (GUISlider*) sourceRectangle;
    
    if (slider->isPressed())
    {
      if (callbackString == "CameraSpeed")
      {
        float cs = slider->getProgress();
        Gateway::getConfiguration().camSpeed = cs;
        slider->setLabelString(String("Speed: ") + cs + " * 100 units/s");
        return;
      }
      
      if (callbackString == "CameraRadius")
      {
        float u = slider->getProgress();
        camDist = u * 100.0f;
        Gateway::getConfiguration().camRadius = camDist + Gateway::getConfiguration().infiniteView;
        slider->setLabelString(String("View distance: ") + u + " * 100 units");
        return;
      }
      
      if (callbackString == "FogDensitySlider")
      {
        float f = slider->getProgress();
        Gateway::getConfiguration().fogDensity = f/10.0f;
        slider->setLabelString(String("Density: f = 1/10^((") + f + " * 0.1 * z)^2)");
        return;
      }
      
      if (callbackString == "FogRedSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("FogColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("FGRLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.x = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().fogColor.x = btncolor.x;
        slider->setLabelString(String("Red: ") + slider->getProgress());
        lbl->setLabelString(String("R: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "FogGreenSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("FogColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("FGGLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.y = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().fogColor.y = btncolor.y;
        slider->setLabelString(String("Green: ") + slider->getProgress());
        lbl->setLabelString(String("G: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "FogBlueSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("FogColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("FGBLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.z = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().fogColor.z = btncolor.z;
        slider->setLabelString(String("Blue: ") + slider->getProgress());
        lbl->setLabelString(String("B: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "LogicRedSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("LogicColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("LRLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.x = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().logicColor.x = btncolor.x;
        slider->setLabelString(String("Red: ") + slider->getProgress());
        lbl->setLabelString(String("R: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "LogicGreenSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("LogicColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("LGLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.y = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().logicColor.y = btncolor.y;
        slider->setLabelString(String("Green: ") + slider->getProgress());
        lbl->setLabelString(String("G: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "LogicBlueSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("LogicColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("LBLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.z = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().logicColor.z = btncolor.z;
        slider->setLabelString(String("Blue: ") + slider->getProgress());
        lbl->setLabelString(String("B: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "FlagRedSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("FlagColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("FLRLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.x = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().flagColor.x = btncolor.x;
        slider->setLabelString(String("Red: ") + slider->getProgress());
        lbl->setLabelString(String("R: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "FlagGreenSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("FlagColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("FLGLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.y = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().flagColor.y = btncolor.y;
        slider->setLabelString(String("Green: ") + slider->getProgress());
        lbl->setLabelString(String("G: ") + slider->getProgress());
        return;
      }
      
      if (callbackString == "FlagBlueSlider")
      {
        GUIButton* btn = (GUIButton*) gui.getWidgetByCallbackString("FlagColorPreview");
        GUILabel* lbl = (GUILabel*) gui.getWidgetByCallbackString("FLBLabel");
        Tuple4f btncolor = btn->getColor();
        btncolor.z = slider->getProgress();
        btn->setColor(btncolor.x, btncolor.y, btncolor.z);
        Gateway::getConfiguration().flagColor.z = btncolor.z;
        slider->setLabelString(String("Blue: ") + slider->getProgress());
        lbl->setLabelString(String("B: ") + slider->getProgress());
        return;
      }
    }
  }
  
  //------------------------
  ///GUICheckBox
  //------------------------
  if (widgetType == CHECK_BOX)
  {
    GUICheckBox *checkBox = (GUICheckBox*) sourceRectangle;
    
    if (checkBox->isClicked())
    {
      //if (callbackString == "CursorCheckbox")
      //{
      //  Gateway::getConfiguration().enableCursor = checkBox->isChecked();
      //  mouseVisible = checkBox->isChecked();
      //  ShowCursor(mouseVisible ? false : true);
      //  return;
      //}
      
      if (callbackString == "DensityCheckbox")
      {
        Gateway::getConfiguration().enableFog = checkBox->isChecked();
        return;
      }
      
      if (callbackString == "InfiniteViewCheckbox")
      {
        float v = checkBox->isChecked() ? 1000.0f : 0.0f;
        Gateway::getConfiguration().infiniteView = v;
        Gateway::getConfiguration().camRadius = camDist + v;
        return;
      }
    }
  }
}

OptionsScene::~OptionsScene()
{}
