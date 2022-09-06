#include <Arduino.h>
#include <TermHelperClass.h>

String TermHelper::TermString;
int TermHelper::TermSolution;

TermHelper::TermHelper() {
}

TermHelper::~TermHelper() {
}

bool TermHelper::generateTerm() {
  Serial.println("called generateTerm");
  
  randomSeed((uint32_t)millis());
  int termType = random() % 3;
  delay(1);
  randomSeed((uint32_t)millis());
  int numberA = random() % 20;
  int numberB;
  do
  {
    delay(1);
    randomSeed((uint32_t)millis());
    numberB = random() % 20;
  } while (numberA == numberB);
 
  TermString = "";
  
  switch(termType) {
      case 0:
        Serial.print("add");
        Serial.print(numberA);
        TermString = numberA;
        Serial.print(" + ");
        TermString += "+";
        Serial.print(numberB);
        TermString += numberB;
        TermString += "=";
        TermSolution = numberA+numberB;
        Serial.println(TermSolution);
        break;
      case 1:
        Serial.println("Sub");
        if(numberA<numberB) {
          Serial.print(numberB);
          TermString = numberB;
          Serial.print(" - ");
          TermString += "-";
          Serial.print(numberA);
          TermString += numberA;
          TermSolution = numberB-numberA;
        } else {
          Serial.print(numberA);
          TermString += numberA;
          Serial.print(" - ");
          TermString += "-";
          Serial.print(numberB);
          TermString += numberB;
          TermString += "=";
          TermSolution = numberA-numberB;
        }
        Serial.print(" = ");
        Serial.println(TermSolution);
        break;
      case 2:
        Serial.println("Mult");
        Serial.print(numberA);
        TermString = numberA;
        Serial.print(" * ");
        TermString += "*";
        Serial.print(numberB);
        TermString += numberB;
        TermString += "=";
        TermSolution = numberA*numberB;
        Serial.print(" = ");
        Serial.println(TermSolution);
        break;
      default:
        Serial.print("No valid termtype choosen");
        return false;
        break;
    }
    Serial.println(TermSolution);
    delay(500);
    return true;
}

int TermHelper::getSolution() {
  return TermSolution;
}

String TermHelper::getTerm() {
  return TermString;
}