#include <M5Unified.h>

static constexpr size_t COLOR_COUNT = 10;
static uint32_t colors[COLOR_COUNT] = {0x390099, 0x9e0059, 0xff0054, 0xff5400, 0xffbd00, 0xffbc42, 0xd81159, 0x8f2d56, 0x218380, 0x73d2de};

#define BG M5.Display.color888(250, 249, 175)
#define RESET_PERIOD 10000.f

void setup(void)
{
  M5.begin();

  M5.Display.init();
  M5.Display.startWrite();
  M5.Display.fillScreen(BG);

  if (M5.Display.isEPD())
  {
    M5.Display.setEpdMode(epd_mode_t::epd_fastest);
  }
  M5.Display.setBrightness(30);

  for (int x = 0; x < COLOR_COUNT; ++x)
  {
    int r=0,g=0,b=0;
    switch (x >> 4)
    {
    case 0:
      b = 255;
      g = x*0x11;
      break;
    case 1:
      b = 255 - (x&15)*0x11;
      g = 255;
      break;
    case 2:
      g = 255;
      r = (x&15)*0x11;
      break;
    case 3:
      r = 255;
      g = 255 - (x&15)*0x11;
      break;
    }
    // colors[x] = M5.Display.color888(r,g,b);
  }

  M5.Display.setFont(&fonts::TomThumb);
  M5.Display.setTextDatum(textdatum_t::middle_center);
  M5.Display.setTextColor(TFT_BLACK, BG);
  M5.Display.setTextSize(2);
  M5.Display.drawString("! CHAOTIC ATTRACTORS !", M5.Display.width()*.5f, M5.Display.height()*.5f);
  delay(1000);

  randomSeed(analogRead(0));
}


// Clifford Attr Params
// float a=-2., b=-2.4, c=1.1, d=-0.9;

// Icon Attractor Params
float l=2.409, a=-2.5, b=0., g=0.9, o=0., d=23.;
struct IconAttractorParams
{
  float params[6];
};

IconAttractorParams params[17] = {{-2.5, 5, -1.9, 1, 0.188, 5}, {1.56, -1, 0.1, -0.82, 0.12, 3}, {-1.806, 1.806, 0, 1, 0, 5}, {-2.195, 10, -12, 1, 0, 3}, {2.5, -2.5, 0, 0.9, 0, 3}, {-2.05, 3, -16.79, 1, 0, 9}, {-2.7, 5, 1.5, 1, 0, 6}, {2.409, -2.5, 0, 0.9, 0, 23}, {-2.08, 1, -0.1, 0.167, 0, 7}, {-2.32, 2.32, 0, 0.75, 0, 5}, {2.6, -2, 0, -0.5, 0, 5}, {-2.34, 2, 0.2, 0.1, 0, 5}, {-1.86, 2, 0, 1, 0.1, 4}, {1.56, -1, 0.1, -0.82, 0, 3}, {1.5, -1, 0.1, -0.805, 0, 3}, {1.455, -1, 0.03, -0.8, 0, 3}, {2.39, -2.5, -0.1, 0.9, -0.15, 16}};

void loop(void)
{
  M5.update();

  static int nextReset=0;
  
  const int h = M5.Display.height();
  const int w = M5.Display.width();
  static int colorIdx;

  static float x=.01;
  static float y=.01;
  float x_n, y_n;

  if (millis()>nextReset || M5.BtnA.wasClicked() || M5.BtnA.wasReleased())
  {
    M5.Display.fillScreen(BG);

    nextReset = millis()+RESET_PERIOD;

    // int i = random(17);
    // int i = 2;
    static int i = -1;
    i = i==16 ? 0 : i+1;
    l = params[i].params[0];
    a = params[i].params[1];
    b = params[i].params[2];
    g = params[i].params[3];
    o = params[i].params[4];
    d = params[i].params[5];
    x=0.01; y=0.01;

    colorIdx = random(COLOR_COUNT-1);
    
    // M5.Display.setTextDatum(textdatum_t::bottom_right);
    // M5.Display.drawString("! CHAOTIC ATTRACTORS !", 0, 0);
    // M5.Display.setTextColor(colors[colorIdx]);
    M5.Display.setCursor(15, 20);
    M5.Display.printf("%d", i);
  }
  
  // dynamical eq
  // Clifford Attr
  // {
  //   x_n = sinf(a*y) + c*cosf(a*x);
  //   y_n = sinf(b*x) + d*cosf(b*y);
  // }
  
  // Icon Attractor
  {
    float zzbar = x*x + y*y;
    float p = a*zzbar + l;
    float zreal = x;
    float zimag = y;

    for (int j=1; j<=d-2; j++)
    {
      float za = zreal*x - zimag*y;
      float zb = zimag*x + zreal*y;
      zreal = za;
      zimag = zb;
    }

    float zn = x*zreal - y*zimag;

    p += b*zn;

    x_n = p*x + g*zreal - o*y;
    y_n = p*y - g*zimag + o*x;
  }

  x = x_n; y=y_n;

  // Scale to M5.Display
  int x_d = x*(h*.4f)+(w*.5f);
  int y_d = y*(h*.4f)+(h*.5f);

  // Draw
  M5.Display.waitDisplay();

  // Attractor output
  M5.Display.fillRectAlpha(x_d, y_d, 1, 1, 18, colors[colorIdx]);
  // Progress Bar
  int progress = h*(1.-(nextReset-millis())/RESET_PERIOD);
  Serial.println(progress);
  M5.Display.fillRect(w-5, 0, 5, progress, colors[colorIdx]);

  M5.Display.display();
}

