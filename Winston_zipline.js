var xPos = 10; // start position


var draw = function() {
    background(12, 232, 232);
    line(0, 215, 400, 215);
    line(0, 115, 400, 115);
    fill(0, 0, 0);
    ellipse(xPos, 200, 30, 30); // a wheel
    fill(255, 255, 255);
    ellipse(xPos, 200, 20, 20);
    fill(0, 0, 0);
    ellipse(xPos, 100, 30, 30); // a wheel
    fill(255, 255, 255);
    ellipse(xPos, 100, 20, 20);
    strokeWeight(5);
    line(xPos, 115, xPos, 325);
    
    xPos += 5;
    
    // face
    fill(255, 255, 0);
    ellipse(xPos, 304, 75, 72);
    
    // eyes
    fill(46, 46, 41);
    ellipse(xPos - 20, 295, 16, 20);
    ellipse(xPos + 10, 296, 16, 20);
    
    // mouth
    fill(252, 10, 65);
    ellipse(xPos + -2, 328, 16, 24);
    
    //arms
    line(xPos - 35, 341, xPos, 232);
    line(xPos + 35, 341, xPos, 232);
    line(xPos + 35, 341, xPos, 361);
    line(xPos - 35, 341, xPos, 361);
    line(xPos, 341, xPos, 380);
    line(xPos, 380, xPos - 35, 388);
    line(xPos, 380, xPos + 35, 388);
    
   
    
    if (xPos > 400)
    
    {
        xPos = 0;
        
    }
        
    
};
