struct Association {
  int outputPin;
  int inputPin;
  int led;
  bool isSolved = false;
};

struct Led {
  int pin;
  int fadeDirection = 0; // 0: idle, 1: fadeIn, -1: fadeOut
  float fadeDuration = 0;
  float brightness = 0;
};

Led leds[] = {
  {27, 0, 0, 0},
  {13, 0, 0, 0},
  {14, 0, 0, 0},
  {12, 0, 0, 0},
};

Association associations[] = {
  {25, 36, 27},
  {33, 34, 13},
  {26, 35, 14},
  {32, 39, 12},
};

const int numAssociations = sizeof(associations) / sizeof(associations[0]);
const int numLeds = sizeof(leds) / sizeof(leds[0]);
bool solved = false;

void setup() 
{
  for (int i = 0; i < numAssociations; i++) 
  {
    Association association = associations[i];

    pinMode(association.outputPin, OUTPUT);
    pinMode(association.inputPin, INPUT);
    pinMode(association.led, OUTPUT);
  }

  Serial.begin(9600);
}

void loop() 
{
  int i = 0;
  ledAnimation();

   delay(5);


  if (solved)
    return;

  for (int i = 0; i < numAssociations; i++) 
  {
    //-- Envoie un signal HIGH sur la broche de sortie
    digitalWrite(associations[i].outputPin, HIGH);
    delay(10);//<- Délai pour stabilité du signal

    //-- Vérifie si la broche d'entrée correspondante est HIGH
    int inputState = digitalRead(associations[i].inputPin);
    
    //-- Affichage du résultat
    if (inputState == HIGH) 
    {
      digitalWrite(associations[i].led, HIGH);
      associations[i].isSolved = true;

      if (isAllSolved())//<- Si toutes les combinaisons sont résolues
      {
        solved = true;
        delay(1500);//<- Pour être sur que ce n'est pas une erreur
        solvedAnimation();
        break;
      }
    } 
    else 
    {
      digitalWrite(associations[i].led, LOW);
      associations[i].isSolved = false;
    }
    
    //-- Couper le signal sur la broche de sortie
    digitalWrite(associations[i].outputPin, LOW);
    delay(10); //<- Attendre 100ms avant de tester la prochaine paire
   }
}

void solvedAnimation()
{
  Serial.println("solvezd!!!!");
  for (int i = 0; i < numLeds; i++)
  {
      leds[i].brightness = 0;
      leds[i].fadeDirection = 1;
      leds[i].fadeDuration = 3500;
  }
}

/*---------------Functions--------------*/
  
/**
 * Allume ou éteint progressivement la LED (effet fade)
 * direction (-1 = fade-out, 1 = fade-in)
 */
void fade(int ledPin, int direction, int duration) 
{
  //-- Cherche la led correspondante
  for (int i = 0; i < numLeds; i++)
  {
    if (leds[i].pin == ledPin)
    {
      leds[i].fadeDirection = direction;
      leds[i].fadeDuration = duration;
    }
  }
}

void ledAnimation()
{
  for (int i = 0; i < numLeds; i++)
  {
    float fadeDelay = 40;
    float maxBrightness = 255;
    float fadeGap = maxBrightness / (leds[i].fadeDuration / fadeDelay);

    if (leds[i].fadeDirection != 0)
    {
      leds[i].brightness = leds[i].brightness + fadeGap * leds[i].fadeDirection;//led.fadeDirection * fadeGap;

      if (leds[i].fadeDirection == 1 && leds[i].brightness >= maxBrightness)
      {
        leds[i].fadeDirection = -1;
        leds[i].brightness = maxBrightness;
      }

      else if (leds[i].fadeDirection == -1 && leds[i].brightness <= 0)
      {
        leds[i].fadeDirection = 1;
        leds[i].brightness = 0;
      }

//Serial.println(leds[i].brightness);

      float easedBrightness = easeInExpo(leds[i].brightness / maxBrightness) * maxBrightness;
      analogWrite(leds[i].pin, easedBrightness);
    }
  }

  // //Animation clignotement pair/impair
  // for (int repeat = 0; repeat < 4; repeat++)//<- répète 4 fois 
  // {
  //   // Allume les LEDs impaires
  //   for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i += 2)
  //   {
  //     fade(1, leds[i], 2);
  //   }
  //   delay(50);
  //   for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i += 2)
  //   {
  //     fade(-1, leds[i], 2);
  //   }

  //   // Allume les LEDs paires
  //   for (int i = 1; i < sizeof(leds) / sizeof(leds[0]); i += 2)
  //   {
  //     fade(1, leds[i], 2);
  //   }
  //   delay(50);
  //   for (int i = 1; i < sizeof(leds) / sizeof(leds[0]); i += 2)
  //   {
  //     fade(-1, leds[i], 2);
  //   }
  // } 

  // //--Animation montée descente
  // for (int repeat = 0; repeat < 4; repeat++) //<- répète 4 fois 
  // {
  //   // Faire le fade-in/out dans l'ordre
  //   for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
  //     if (repeat == 0 && i == 0) //<- Si c'est la première led qu'on allume)
  //     {
  //       fade(1, leds[i], 3);
  //     }
  //     fade(1, leds[i+1], 3);
  //     fade(-1, leds[i], 3);
  //   }

  //   fade(-1, leds[sizeof(leds) / sizeof(leds[0]) - 1], 3);

  //   // Faire le fade-in/out dans l'ordre inverse
  //   for (int i = sizeof(leds) / sizeof(leds[0]) - 1; i >= 0; i--) {
  //     fade(1, leds[i-1], 3);
  //     fade(-1, leds[i], 3);
  //   }

  //   fade(-1, leds[0], 3);

  //   delay(1000);  // Pause avant de recommencer l'animation
  // }

  /*for (int repeat = 0; repeat < 50; repeat++) //<- répète 4 fois 
  {
    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) 
      fade(1, leds[i], 1000);

    delay(5000);

    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) 
      fade(-1, leds[i], 1000);

    delay(5000);

  }*/
}

float easeInOutQuad(float x) 
{
  return x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2;
}
float easeInExpo(float x) {
  return x == 0 ? 0 : pow(2, 10 * x - 10);
}

bool isAllSolved()
{
  for (int i = 0; i < numAssociations; i++)
    if (associations[i].isSolved == false)
      return false; //<- Si une des combinaison n'est pas résolue

  return true;
}