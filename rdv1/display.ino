void setupDisplay() {
  display.begin(0x3C, true); // Address 0x3C default
  display.clearDisplay();
  display.display();
  display.setRotation(1);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
}

void refreshDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);

  if(Bluefruit.Periph.connected()) {
    display.print("Connected     ");
  }
  else{
    display.print("Not Connected ");
  }

  display.print("Batt "); display.println(constrain(robotBatt, 0, 99));

  display.print("A:");
  if(buttonAR) display.print("X "); else display.print("O ");
  display.print("B:");
  if(buttonBR) display.print("X "); else display.print("O ");
  display.print("C:");
  if(buttonCR) display.print("X "); else display.print("O ");
  display.print("L:");
  if(buttonL) display.print("X "); else display.print("O ");
  display.print("R:");
  if(buttonR) display.print("X "); else display.print("O ");
  display.println("");
  
  display.print("A:");
  if(!digitalRead(BUTTON_A)) display.print("X "); else display.print("O ");
  display.print("B:");
  if(!digitalRead(BUTTON_B)) display.print("X "); else display.print("O ");
  display.print("C:");
  if(!digitalRead(BUTTON_C)) display.print("X "); else display.print("O ");
  display.println("");

  display.print("LX: "); if(lx<10) display.print(" "); if(lx<100) display.print(" "); display.print(lx); display.print("     ");
  display.print("RX: "); if(rx<10) display.print(" "); if(rx<100) display.print(" "); display.print(rx); display.println("");
  display.print("LY: "); if(ly<10) display.print(" "); if(ly<100) display.print(" "); display.print(ly); display.print("     ");
  display.print("RY: "); if(ry<10) display.print(" "); if(ry<100) display.print(" "); display.print(ry); display.println("");

  display.print("H");
  for(int l=0; l<4; l++){
    display.print(l);
    display.print(":");
    display.print((int) hipAngle[l]);
    display.print(" ");
  }
  display.println("");
  display.print("S");
  for(int l=0; l<4; l++){
    display.print(l);
    display.print(":");
    display.print((int) shoulderAngle[l]);
    display.print(" ");
  }
  display.println("");
  display.print("K");
  for(int l=0; l<4; l++){
    display.print(l);
    display.print(":");
    display.print((int) kneeAngle[l]);
    display.print(" ");
  }
  display.println("");

  //display.println("R: P: Y:");
  //display.println("Xa: Ya: Za:");
  //display.println("Xg: Yg: Zg:");

  //display.println(controlMsg);
  
  display.display();
}
